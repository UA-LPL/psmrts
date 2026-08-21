/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsInventory_hpp
#define PsmrtsInventory_hpp

#include <exception>
#include <string>
#include <cstdio>
#include <tuple>
#include <algorithm>
#include <memory>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/PsmrtsSharedCache.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/PsmrtsPriorityTracer.hpp>

namespace psmrts {

  /**
     * @brief PSMRTS product inventory container that maintains all product caches 
     * 
     * This class contains a collection of shapes, tracers, priority tracers and
     * a set of parameters/options that are maintained for each instance of 
     * a PsmrtsInventory.
     * 
     * Typically, priority is given to the existance of a priority tracer.
     * Priority tracers are created by the contents of the TracerInventory.
     * The TracerInventory contains products from the ShapeInventory. Because
     * PsrmtsPriorityTracers can have the order and content of the their tracer
     * priority modified, they cannot be passed around a shared pointer. Hence,
     * the PsmrstPriortyTracer interface will provide copies of them for safe
     * use in instances.
     * 
     * The PsmrtsTranslations contains a state of the users shell environment
     * variables and parameters typcially used to translate file paths. There is
     * one copy of the translations maintained for this inventory.
     * 
     * Copies of these environments result in completely shared resources across
     * all instances as they are shared pointers internally.
     * 
     * Thread safety is deferred to the PsmrtsSharedCache objects that store
     * the tracers, shapes and priority tracers as well as the PsmrtsCache for
     * the translation variables.
     * 
    */
    class PsmrtsInventory : public PsmrtsProduct {
      public:
        using UIDType         = PsmrtsProduct::UIDType;
        using PsmrtsParameter = ProductOption;

        using ShapeInventory          = PsmrtsSharedCache<UIDType, PsmrtsShape>;
        using TracerInventory         = PsmrtsSharedCache<UIDType, PsmrtsTracer>;
        using PriorityTracerInventory = PsmrtsSharedCache<UIDType, PsmrtsPriorityTracer>;
        using ParameterInventory      = PsmrtsTranslations::ParameterInventory;        
        using EnvironmentInventory    = PsmrtsTranslations::EnvironmentInventory;

        using ShapeCacheMap           = ShapeInventory::CacheMap;
        using TracerCacheMap          = TracerInventory::CacheMap;

        PsmrtsInventory( ) : PsmrtsProduct( "product", "inventory" ) {
          this->init();
        }

        PsmrtsInventory( const std::string &inventory_name,
                         const std::string &itype = "inventory" ) : 
                         PsmrtsProduct( inventory_name, itype ) {
          this->init();
        }

        virtual ~PsmrtsInventory() = default;

        /** Returns the number of all products excluding parameters and envs */
        inline size_t size() const {
          size_t n_products = m_shapes->size() + 
                              m_tracers->size() +
                              m_prioritytracers->size();
          return ( n_products );
        }

        inline const ShapeInventory &shapes( ) const {
          return ( *m_shapes );
        }

        inline ShapeInventory &shapes( )  {
          return ( *m_shapes );
        }

        inline const TracerInventory &tracers( ) const {
          return ( *m_tracers );
        }

        inline TracerInventory &tracers( ) {
          return ( *m_tracers );
        }        

        inline size_t priority_tracer_size() const {
          return ( m_prioritytracers->size() );
        }

        inline PsmrtsPriorityTracer priority_tracer_contains( const UIDType &uid ) {
          return ( m_prioritytracers->contains( uid ) );
        }

        inline PsmrtsPriorityTracer priority_tracer_find( const UIDType &uid ) {
          auto pt_t = m_prioritytracers->find( uid );
          if ( !pt_t ) return ( PsmrtsPriorityTracer() );
          return ( *pt_t );
        }

        inline void priority_tracer_add( const PsmrtsPriorityTracer &pt ) {
          if ( pt.isValid() ) {
            m_prioritytracers->add( pt.uid(), pt );
          }
          return;
        }

        /** Thread-safe process shapes with function/lambda/object */
        template <typename Functor>
          bool process_shapes( Functor function ) const {
            return ( m_shapes->process( function ) );
          }
           
        /** Thread-safe process tracers with function/lambda/object */
        template <typename Functor>
          bool process_tracers( Functor function ) const {
            return ( m_tracers->process( function ) );
          }

        inline const ParameterInventory &parameters( ) const {
          return ( m_translations->parameters() );
        }

        inline const EnvironmentInventory &environment( ) const {
          return ( m_translations->environment() );
        }     
        
        inline const PsmrtsTranslations &translations() const {
          return ( *m_translations );
        }

        /** Merge a PsmrtsInventory into another inventory */
        inline size_t merge( const PsmrtsInventory &other ) {
          size_t n_merged = 0;
          if ( this != &other ) {
            n_merged += m_shapes->merge( *other.m_shapes );
            n_merged += m_tracers->merge( *other.m_tracers );
            n_merged += m_prioritytracers->merge( *other.m_prioritytracers );
            n_merged += m_translations->merge( *other.m_translations );
          }
          return ( n_merged );
        }
                
        inline void remove( const UIDType &uid) {
          m_shapes->remove( uid );
          m_tracers->remove( uid );
          m_prioritytracers->remove( uid );
        }

        /** Clear everything out, liquidate */
        inline void clear() {
          m_shapes->clear();
          m_tracers->clear();
          m_prioritytracers->clear();
        }

        /** Reinitialize everything  */
        inline void init( ) {
          m_shape.reset( make_shared_copy( ShapeInventory( m_name ) );
          m_tracers.reset( make_shared_copy(TracerInventory( m_name ) );
          m_prioritytracers.reset( make_shared_copy( PriorityTracerInventory{ m_name ) );
          m_translations.reset( make_shared_copy( PsmrtsTranslations::create() );
        }

      private:
        std::shared_ptr<ShapeInventory>          m_shapes;
        std::shared_ptr<TracerInventory>         m_tracers;
        std::shared_ptr<PriorityTracerInventory> m_prioritytracers;
        std::shared_ptr<PsmrtsTranslations>      m_translations;

    };

} // namespace psmrts

#endif
