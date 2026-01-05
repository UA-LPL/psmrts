#ifndef PsmrtsInventory_hpp
#define PsmrtsInventory_hpp
#pragma once

#include <exception>
#include <string>
#include <cstdio>
#include <tuple>

// Setup for environment variable support
#if defined(WIN32) || defined(_MSC_VER) || defined(__CYGWIN__)
#define NOMINMAX
#include <windows.h>
#else
  extern char **environ;
#endif
#include <algorithm>

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
        using UIDType         = PsmrtsProduct::UIDType;
        using PsmrtsParameter = ProductOption;

        using ShapeInventory          = ProductInventory<UIDType, PsmrtsShape>;
        using TracerInventory         = ProductInventory<UIDType, PsmrtsTracer>;
        using PriorityTracerInventory = ProductInventory<UIDType, PsmrtsPriorityTracer>;
        using ParameterInventory      = ProductInventory<std::string, PsmrtsParameter>;
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
        
        /** Merge a PsmrtsInventory into another inventory */
        inline size_t merge( const PsmrtsInventory &other ) {
          size_t n_merged = 0;
          if ( this != &other ) {
            n_merged += this->shapes().merge( other.shapes() );
            n_merged += this->tracers().merge( other.tracers() );
            n_merged += this->prioritytracers().merge( other.prioritytracers() );
            n_merged += this->parameters().merge( other.parameters() );
            n_merged += this->env().merge( other.env() );
          }
          return ( n_merged );
        }
                
        inline void remove( const UIDType &uid) {
          this->shapes().remove( uid );
          this->tracers().remove( uid );
          this->prioritytracers().remove( uid );
        }

        /** Remove a keyword from the parameter set */
        inline void remove_param( const std::string &uid) {
          this->parameters().remove( uid );
        }

        inline void remove_env( const std::string &env ) {
          this->env().remove( env );
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
          m_shapes          = ShapeInventory{ this->product().name(), "shapes" } ;
          m_tracers         = TracerInventory{ this->product().name(), "tracers" } ;
          m_prioritytracers = PriorityTracerInventory{ this->product().name(), "prioritytracers" };
          m_parameters      = ParameterInventory{  this->product().name(), "parameters",
                                                  &ParameterInventory::case_insensitive_key } ;
          m_env             = EnvInventory{  this->product().name(), "env", &EnvInventory::get_real_map_key } ;
        }

        inline const EnvInventory &load_and_merge_env() {
          this->env().merge( PsmrtsInventory::getenv( this->product().name() ) );
          return ( this->env() );
        }

        static inline EnvInventory getenv( const std::string &name_p = "env ") {
          return ( PsmrtsInventory::get_environment_variables( name_p ) );
        }

      private:
        ShapeInventory           m_shapes;
        TracerInventory          m_tracers;
        PriorityTracerInventory  m_prioritytracers;
        ParameterInventory       m_parameters;
        EnvInventory             m_env;

      static inline std::tuple<std::string,std::string> parse_env_string( const std::string &env_s ) {
        size_t eq_pos = env_s.find( "=" );
        if ( std::string::npos != eq_pos ) {
          return ( std::make_tuple( env_s.substr(0, eq_pos), env_s.substr( eq_pos+1, std::string::npos ) ) );
        }
        else {
          return ( std::make_tuple( env_s, std::string("") ) );
        }
      }

              
      /** Load all the environment variables */
      static inline EnvInventory get_environment_variables( const std::string &name_p ) {
          EnvInventory env_t{ name_p, "env", &EnvInventory::get_real_map_key };

#if defined(WIN32) || defined(_MSC_VER) || defined(__CYGWIN__)
          // **** Windows implementation *****/
          LPCH envStrings = GetEnvironmentStringsA();
          if ( nullptr == envStrings ) {
            return ( env_t );
          }

          LPCH env = envStrings;
          while ( *env != '\0' ) {
            std::string env_entry(env);
            auto [ key, value ] = PsmrtsInventory::parse_env_string( env_entry );            
            env_t.add(key, value);

            env += strlen(env) + 1;
          }
          
           FreeEnvironmentStringsA(envStrings);           
#else
          // **** Linux implementation *****/
          char **env = environ;
          while ( *env != nullptr ) {
            auto [ key, value ] = PsmrtsInventory::parse_env_string(  *env );
            env_t.add( key, value );
            env++;
          }
#endif

          return ( env_t );
        }

    };

} // namespace psmrts

#endif
