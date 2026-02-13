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

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/ProductInventory.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/PsmrtsPriorityTracer.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>

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
     * The TracerInventory contains products from the shape 
     * 
    */
    class PsmrtsInventory : public PsmrtsProduct {
      public:
        using UIDType         = PsmrtsProduct::UIDType;
        using PsmrtsParameter = ProductOption;

        using ShapeInventory          = ProductInventory<UIDType, PsmrtsShape>;
        using TracerInventory         = ProductInventory<UIDType, PsmrtsTracer>;
        using PriorityTracerInventory = ProductInventory<UIDType, PsmrtsPriorityTracer>;
        using ParameterInventory      = PsmrtsTranslations::ParameterInventory;        
        using EnvironmentInventory    = PsmrtsTranslations::EnvironmentInventory;        


        PsmrtsInventory( ) : PsmrtsProduct( "product", "inventory" ) {
          this->init();
        }

        PsmrtsInventory( const std::string &inventory_name,
                         const std::string &itype = "inventory" ) : 
                         PsmrtsProduct( inventory_name, itype ) {
          this->init();
        }

        virtual ~PsmrtsInventory() { }

        /** Returns the number of all products excluding parameters and envs */
        inline size_t size() const {
          size_t n_products = this->shapes().size() + 
                              this->tracers().size() +
                              this->prioritytracers().size();
          return ( n_products );
        }

        inline const ShapeInventory &shapes( ) const {
          return ( m_shapes );
        }

        inline  ShapeInventory &shapes( )  {
          return ( m_shapes );
        }

        inline const TracerInventory &tracers( ) const {
          return ( m_tracers );
        }

        inline TracerInventory &tracers( ) {
          return ( m_tracers );
        }        

        inline const PriorityTracerInventory &prioritytracers( ) const {
          return ( m_prioritytracers );
        }

        inline PriorityTracerInventory &prioritytracers( ) {
          return ( m_prioritytracers );
        }

        inline const ParameterInventory &parameters( ) const {
          return ( m_translations.parameters() );
        }

        inline const EnvironmentInventory &environment( ) const {
          return ( m_translations.environment() );
        }     
        
        inline const PsmrtsTranslations &translations() const {
          return ( m_translations );
        }

        /** Merge a PsmrtsInventory into another inventory */
        inline size_t merge( const PsmrtsInventory &other ) {
          size_t n_merged = 0;
          if ( this != &other ) {
            n_merged += this->shapes().merge( other.shapes() );
            n_merged += this->tracers().merge( other.tracers() );
            n_merged += this->prioritytracers().merge( other.prioritytracers() );
            n_merged += m_translations.merge( other.translations() );
          }
          return ( n_merged );
        }
                
        inline void remove( const UIDType &uid) {
          this->shapes().remove( uid );
          this->tracers().remove( uid );
          this->prioritytracers().remove( uid );
        }

        /** Clear everything out, liquidate */
        inline void clear() {
          this->shapes().clear();
          this->tracers().clear();
          this->prioritytracers().clear();
        }

        /** Reinitialize everything  */
        inline void init( ) {
          m_shapes          = ShapeInventory{ this->product().name(), "shapes" } ;
          m_tracers         = TracerInventory{ this->product().name(), "tracers" } ;
          m_prioritytracers = PriorityTracerInventory{ this->product().name(), "prioritytracers" };
          m_translations    = PsmrtsTranslations::create();
        }

      private:
        ShapeInventory           m_shapes;
        TracerInventory          m_tracers;
        PriorityTracerInventory  m_prioritytracers;
        PsmrtsTranslations       m_translations;

    };

} // namespace psmrts

#endif
