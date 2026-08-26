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
#include <psmrts/core/PsmrtsSharedCache.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
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
   * priority modified, they cannot be passed around by shared pointer. Hence,
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
  class PsmrtsInventory {
    public:
      using UIDType               = PsmrtsProduct::UIDType;
      using ShapeInventory        = PsmrtsSharedCache<UIDType, PsmrtsShape>;
      using TracerInventory       = PsmrtsSharedCache<UIDType, PsmrtsTracer>;
      using SharedShapeInventory  = std::shared_ptr<ShapeInventory>;
      using SharedTracerInventory = std::shared_ptr<TracerInventory>;
      using ShapeCacheMap         = ShapeInventory::CacheMap;
      using TracerCacheMap        = TracerInventory::CacheMap; 
      using ParameterInventory    = PsmrtsTranslations::ParameterInventory;        
      using EnvironmentInventory  = PsmrtsTranslations::EnvironmentInventory;      

      PsmrtsInventory( ) {
        m_name         = "inventory";
        m_shapes       = make_shared_copy( ShapeInventory( this->name() ) );
        m_tracers      = make_shared_copy( TracerInventory( this->name() ) );
        m_translations = make_shared_copy( PsmrtsTranslations::create() );
      }
      PsmrtsInventory( const std::string &name ) {
        m_name         = name;
        m_shapes       = make_shared_copy( ShapeInventory( name ) );
        m_tracers      = make_shared_copy( TracerInventory( name) );
        m_translations = make_shared_copy( PsmrtsTranslations::create() );
      }
      PsmrtsInventory( const std::string &name,
                       const PsmrtsTranslations &translator ) {
        m_name         = name;
        m_shapes       = make_shared_copy( ShapeInventory( name ) );
        m_tracers      = make_shared_copy( TracerInventory( name) );
        m_translations = make_shared_copy( translator );
      }
      PsmrtsInventory( const std::string &name,
                       const SharedTranslations &translator ) {
        m_name         = name;
        m_shapes       = make_shared_copy( ShapeInventory( name ) );
        m_tracers      = make_shared_copy( TracerInventory( name) );
        m_translations = translator;
      }      
      virtual ~PsmrtsInventory() = default;


      /** Return the inventory name */
      inline const std::string &name() const {
        return ( m_name );
      }

      /** Returns the number of all products excluding parameters and envs */
      inline size_t size() const {
        size_t n_products = m_shapes->size() + 
                            m_tracers->size();
        return ( n_products );
      }

      inline size_t size_shapes() const {
        return ( m_shapes->size() );
      }

      inline const SharedShapeInventory &shapes( ) const {
        return ( m_shapes );
      }

      inline SharedShapeInventory &shapes( )  {
        return ( m_shapes );
      }

      inline size_t size_tracers() const {
        return ( m_tracers->size() );
      }

      inline UIDType add( const SharedShape &shape ) {
        if ( !shape ) return ( psmrts::PsmrtsUID::null_uid() );
        m_shapes->add(shape->uid(), shape );
        return ( shape->uid() );
      }


      inline const SharedTracerInventory &tracers( ) const {
        return ( m_tracers );
      }

      inline SharedTracerInventory &tracers( ) {
        return ( m_tracers );
      }        

      inline UIDType add( const SharedTracer &tracer ) {
        if ( !tracer ) return ( psmrts::PsmrtsUID::null_uid() );
        m_tracers->add( tracer->uid(), tracer );
        return ( tracer->uid() );
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

      inline ConstSharedTranslations translations() const {
        return ( m_translations );
      }

      inline void set_translations( const PsmrtsTranslations &translator ) {
        m_translations = make_shared_copy( translator );
      }

      inline void set_translations( const SharedTranslations &translator ) {
        m_translations =  translator;
      }

      inline std::string translate_path( const std::string &filepath ) const {
        if ( !m_translations ) return ( filepath );
        return ( m_translations->translate_path( filepath ) );
      }

      inline PsmrtsPriorityTracer create_priority_tracer( const std::string &name = "" ) const {
        std::string name_p( name );
        if ( name_p.length() == 0 ) name_p = this->name();
        return ( PsmrtsPriorityTracer( name, *m_tracers ) );
      }

      /** Merge a PsmrtsInventory into another inventory */
      inline size_t merge( const PsmrtsInventory &other ) {
        size_t n_merged = 0;
        if ( this != &other ) {
          n_merged += m_shapes->merge( *other.m_shapes );
          n_merged += m_tracers->merge( *other.m_tracers );
        }
        return ( n_merged );
      }
              
      inline void remove_shape( const UIDType &uid) {
        m_shapes->remove( uid );
      }

      inline void remove_tracer( const UIDType &uid) {
        m_tracers->remove( uid );
      }

      /** Clear everything out, liquidate */
      inline void clear( const bool preserve_translator = true ) {
        m_shapes->clear();
        m_tracers->clear();
        if ( !preserve_translator ) {
          m_translations  = make_shared_copy( PsmrtsTranslations::create() );
        }
      }

    private:
      std::string                      m_name;
      std::shared_ptr<ShapeInventory>  m_shapes;
      std::shared_ptr<TracerInventory> m_tracers;
      ConstSharedTranslations          m_translations;
  };

  // Declare a shared pointer type for tracers
  using SharedInventory      = std::shared_ptr<PsmrtsInventory>;
  using ConstSharedInventory = std::shared_ptr<const PsmrtsInventory>;

} // namespace psmrts

#endif
