/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductModelDispatch_hpp
#define ProductModelDispatch_hpp

#include <exception>
#include <variant>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>

namespace psmrts {


  template <typename... Ts>
    class ProductModelDispatch {
      public:
        using Model = std::variant<Ts...>;

        ProductModelDispatch( )         = default;
        ProductModelDispatch( const Model &model ) : m_model( model ) {  }
        virtual ~ProductModelDispatch() = default;

        /** Returns the product info of the model */
        inline const PsmrtsProduct &product() const {
          auto get_product_info = []( auto &model ) -> const PsmrtsProduct& {
            return ( model.product() );
          };
          return ( std::visit( get_product_info, m_model ) );
        }

        /**
         * @brief Product dispatch template runs the PSMRTS product line interface
         * 
         * This product process dispatch class that will execute all requested
         * ProductType::process( PRQ ) methods. 
         * 
         * Note that if the requested process( PRQ ) does not exist in any of the
         * ProductTypes, a compiler error would ensure. However, a macro is provided
         * to catch all requested PRQs that have not been implemented, trap these
         * calls and return an error condition that identifies the process( PRQ )
         * is missing. For any PSMRTS Just enter the following Line in your class
         * at the bottom of public section of the class.
         * 
         * @code
         *  // Report all remaining features not available.
         * //  E.g. PRQFacet not relevant in the Ellipsoid tracer
         *  PSMRTS_PROCESS_CATCHALL( "Ellipsoid" )
         * @endcode 
         * 
         * **Note** that there is no semicolon that ends this statement and you will
         * a compiler error if you forget that!
         * 
         * The NoProcessRequestHandler class can be used as nifty variant noop
         * product dispatcher. It traps all calls where no process methods are
         * present.
         * 
         * To disable this catchall you can define the macro
         * PSMRTS_DISABLE_PROCESS_CATCHALL at build time and the compiler will 
         * report all missing process( PRQ ) methods. @see PsmrtsRequest.hpp.
         * 
         * @tparam PRQ     Template type of a PRQ dispatch/run
         * @param request  Request functor to dispatch to process function
         * @return true    If the execution of the process function was successful
         * @return false   If the execution failed
         */
        template <typename PRQ>
          bool process( PRQ &request ) const {

            /** lambda to run the trace with proper tracking */
            auto dispatch_model = [] ( const auto &model, auto &request ) -> bool {
              bool status = false;
              try {
                request.reset();  // Resets the timer to this moment
                request.process_running();   // Hits the counter, logs entry        
                status = model.process( request ); // Execute
                request.process_complete( status );  // No thrown errors
              }
              catch ( const std::exception &e ) {
                request.add_error( e ); // Report errors 
                status = false;
                // return ( status  );
              } 
              catch ( ... ) {
                // Report unknown errors 
                request.add_error( std::runtime_error( "PsmrtsRequest::" + request.name() + " - Unknown exception caught!" ) );
                status = false;                
              }

              return ( status );
            };

            // Dispatch the process method to the modelType
            return ( std::visit( [&] ( auto &&model ) -> bool { return ( dispatch_model( model, request ) ); }, this->model() ) );
          }

      protected:
        Model  m_model;

        inline const Model &model() const {
          return ( m_model );
        }

        inline void set_model( const Model &model ) {
          m_model = model;
        }


    };

  } // namespace psmrts

#endif
