#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsTracer_hpp
#define PsmrtsTracer_hpp

#include <string>
#include <variant>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsCache.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductModelDispatch.hpp>
#include <psmrts/core/products/ProductVoidVariant.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/bullet/BulletTracer.hpp>
#include <psmrts/tracers/ellipsoid/EllipsoidTracer.hpp>
#include <psmrts/tracers/naifdsk/NaifDskTracer.hpp>

namespace psmrts {

  /**
   * @brief PsmrtsTracer container for all PSMRTS tracers
   * 
   * This class contains all the active/supported tracers in the PSMRTS system.
   * All the tracer business is in the ProductProcessDispath, a generic process
   * dispatch container class that uses templated process( Product T ) methods
   * to execute the requested operation.
   * 
   * Any one of the supported tracers can be passed to the constructor. Its
   * instance is copied into the ProductModelDispatch class. Each tracer
   * implements all the process( T ) method it supports. The design of the
   * process dispatcher actually accepts any call made to this object, where
   * non-existant process( T ) methods are trapped and an error is recorded in
   * the corresponding T class. The T class must contain a PRQRequest base class
   * to properly handle this feature.
   * 
   * To use this class, the following sequence can be used:
   * @code 
   *   psmrts::PsmrtsTracer tracer( psmrts::PsmrtsTracer::sphere( 100 ) );
   *   psmrts::PRQRayTrace ray( observer, lookdir );
   *   bool status = tracer.process( ray ); // == ray.isValid() = ray.trace().hasHit()
   *   if ( true == status ) {
   *     // Trace successfully intercepts surface
   *   }
   *   elseif ( false == ray.was_invoked() ) {
   *      // Indicates the PRQ has no process(PRQRayTrace) method
   *   }
   *   elseif ( ray.error_count() > 0 ) {
   *      // An error was encountered and can be retrieved using
   *      // ray.errors_to_string()     
   *   }
   * @endcode
   * 
   * See PsmrtsRequest.hpp and ProductModelDispatch.hpp for details.
   */
  class PsmrtsTracer : public ProductModelDispatch< ProductVoidVariant,
                                                      EllipsoidTracer, 
                                                      BulletTracer, 
                                                      NaifDskTracer> {
    public:
      using Tracer   = ProductModelDispatch::Model;
      using Variants = Tracer;  // Standardization for ProductMaker
      using UIDType = PsmrtsUID::UIDType;

      PsmrtsTracer( ) : ProductModelDispatch ( ProductVoidVariant( "void" ) ) {
        init_product();
      }
      PsmrtsTracer( const std::string &name ) : 
                    ProductModelDispatch ( ProductVoidVariant( name ) ) { 
        init_product();
      }
      PsmrtsTracer( const Tracer &tracer ) : ProductModelDispatch( tracer ) { 
        init_product();
       }
      virtual ~PsmrtsTracer() { }

      inline static PsmrtsTracer sphere( const double radius_km, 
                                         const std::string &name="sphere" ) {
        return ( PsmrtsTracer( EllipsoidTracer( radius_km, name ) ) );
      }

      inline static PsmrtsTracer spheroid( const double a_km, 
                                           const double c_km, 
                                           const std::string &name="spheroid" ) {
        return ( PsmrtsTracer( EllipsoidTracer( a_km, c_km, name ) ) );
      }

      inline static PsmrtsTracer ellipsoid( const double a_km,
                                            const double b_km,
                                            const double c_km, 
                                            const std::string &name="ellipsoid" ) {
        Eigen::Vector3d radii( { a_km, b_km, c_km } );
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ) ) ); 
      }

      inline static PsmrtsTracer ellipsoid( const Eigen::Vector3d radii, 
                                            const std::string &name="ellipsoid" ) {
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ) ) ); 
      }

      inline static PsmrtsTracer bullet( const std::string &meshfile ) {
        return ( PsmrtsTracer( BulletTracer( PsmrtsShape( meshfile ) ) ) );
      }

      inline static PsmrtsTracer naifdsk( const std::string &dskfile ) {
        return ( PsmrtsTracer( NaifDskTracer( dskfile ) ) ); 
      }

      inline bool isValid() const {
        return ( !std::holds_alternative<ProductVoidVariant>( m_model ) );
      }

      inline const PsmrtsProduct &product() const {
        return ( m_product ); 
      }

      inline const std::string &name() const {
        return ( this->product().name() ); 
      }  

      inline const std::string &type() const {
        return ( this->product().type() ); 
      } 

      inline const std::string &model() const {
        return ( this->product().model() ); 
      }  

      inline const UIDType &uid() const {
        return ( this->product().uid() ); 
      }      

      inline ProductSpecification specs() const {
        ZoneScoped;
        const auto visitor = overload{            
                  [](auto &&tracer ) -> ProductSpecification {
                       return ( tracer.product_specifications() ); 
                  }
        };
       
        return ( std::visit(visitor, m_model ) ); 
      } 

      inline const ProductConfiguration &config() const {
        ZoneScoped;
        const auto visitor = overload{            
                  [](auto &&tracer ) -> const ProductConfiguration & {
                       return ( tracer.config() ); 
                  }
        };
       
        return ( std::visit(visitor, m_model ) ); 
      }        

      inline bool matches( const ProductConfiguration &conf ) const {
        ZoneScoped;
        return ( this->config().matches( conf ) );
      }

      inline double maximum_radius() const {
        ZoneScoped;
        const auto visitor = overload{            
                  [](auto &&tracer ) ->double {
                       return ( tracer.maximum_radius() ); 
                  }
        };
       
        return ( std::visit(visitor, m_model ) ); 
      }
      
      inline double minimum_radius() const {
        ZoneScoped;
        const auto visitor = overload{            
                  [](auto &&tracer ) ->double {
                       return ( tracer.minimum_radius() ); 
                  }
        };
       
        return ( std::visit(visitor, m_model ) ); 
      }

    private:
      PsmrtsProduct m_product;

      inline void init_product() {
        const auto visitor = overload{            
          [](auto &&tracer ) ->PsmrtsProduct {
              return ( tracer.product() ); 
          }
        };
        
        m_product = std::visit(visitor, m_model );
      }
        
  };

} // namespace psmrts

#endif
