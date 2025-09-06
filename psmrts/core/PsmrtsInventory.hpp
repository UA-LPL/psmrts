#ifndef PsmrtsInventory_hpp
#define PsmrtsInventory_hpp
#pragma once

#include <exception>
#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/ProductInventory.hpp>
#include <psmrts/core/ProductOption.hpp>
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
     * The TracerInventory contains products from the shape 
     * 
    */
    class PsmrtsInventory : public PsmrtsProduct {
      public:
        using UIDType         = PsmrtsUID::UIDType;
        using PsmrtsParameter = ProductOption;

        using ShapeInventory          = ProductInventory<UIDType, PsmrtsShape>;
        using TracerInventory         = ProductInventory<UIDType, PsmrtsTracer>;
        using PriorityTracerInventory = ProductInventory<UIDType, PsmrtsPriorityTracer>;
        using ParameterInventory      = ProductInventory<std::string, PsmrtsParameter, lowercase_key_id<std::string>>;
        using EnvInventory            = ProductInventory<std::string, std::string>;


        PsmrtsInventory( ) : PsmrtsProduct( "product", "inventory" ) {
          this->init();
        }

        PsmrtsInventory( const std::string &inventory_name,
                         const std::string &itype = "inventory" ) : 
                         PsmrtsProduct( inventory_name, itype ) {
          this->init();
        }

        virtual ~PsmrtsInventory() { }


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
          return ( m_parameters );
        }        

        inline ParameterInventory &parameters( ) {
          return ( m_parameters );
        }

        inline const EnvInventory &env( ) const {
          return ( m_env );
        }        

        inline EnvInventory &env( ) {
          return ( m_env );
        }        

        inline void remove( const UIDType &uid) {
          this->shapes().remove( uid );
          this->tracers().remove( uid );
          this->prioritytracers().remove( uid );
        }

        /** Remove a keyword from the parameter set */
        inline void remove( const std::string &uid) {
          this->parameters().remove( uid );
        }

        /** Clear everything out, liquidate */
        inline void clear() {
          this->shapes().clear();
          this->tracers().clear();
          this->prioritytracers().clear();
          this->parameters().clear();
          this->env().clear();
        }

        /** Reinitialize everything  */
        inline void init( ) {
          m_shapes          = ShapeInventory( this->product().name(), "shapes") ;
          m_tracers         = TracerInventory( this->product().name(), "tracers") ;
          m_prioritytracers = PriorityTracerInventory( this->product().name(), "prioritytracers") ;
          m_parameters      = ParameterInventory( this->product().name(), "parameters") ;
          m_env             = EnvInventory( this->product().name(), "env") ;
        }

      private:
        ShapeInventory           m_shapes;
        TracerInventory          m_tracers;
        PriorityTracerInventory  m_prioritytracers;
        ParameterInventory       m_parameters;
        EnvInventory             m_env;
    };

} // namespace psmrts

#endif
