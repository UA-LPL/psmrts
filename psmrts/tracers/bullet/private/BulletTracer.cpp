/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <memory>

#include "PsmrtsBulletWorldModel.hpp"
#include "BulletTracerModel.hpp"
#include "PsmrtsBulletMeshMap.hpp"
#include "../BulletTracer.hpp"
#include <psmrts/core/ProductMaker.hpp>

namespace psmrts {

  /**
   * @brief Implementation of the PIMPL class for fully private Bullet usage
   * 
   * This PIMPL class fully contains the Bullet implementation in the PSMRTS
   * API. 
   * 
   */
  class BulletTracer::BulletTracerImpl : public PsmrtsProduct {
    public:
      BulletTracerImpl( ) : PsmrtsProduct( "bullet", "bullet" ),
                            m_shape(), 
                            m_bullet_model() { }
      BulletTracerImpl( const PsmrtsShape &shape,
                        const bool useCompression = true,
                        const bool buildBvh = true   ) : 
                        PsmrtsProduct( shape.name(), "bullet" ),
                        m_shape( shape ),
                        m_bullet_model( bullet::PsmrtsBulletMeshMap( shape.get_mesh(), 
                                                                     shape.name(), 0), 
                                                                     shape.name(),
                                                                     useCompression,
                                                                     buildBvh ) { 
      }
      ~BulletTracerImpl() = default;

      inline bool useCompression() const {
        return ( m_bullet_model.model().useCompression() );
      }

      inline bool useBuildBvh() const {
        return ( m_bullet_model.model().useBuildBvh() );
      }

      inline bool useThreadSafety() const {
        return ( m_bullet_model.model().useThreadSafety() );
      }

      inline bool ray_trace( PsmrtsRayTrace &ray ) const {
        ray.set_tracer_id( this->uid() );
        return ( m_bullet_model.ray_trace( ray ) );
      }

      inline bool get_facet(  const PsmrtsRayTrace &ray, 
                       PsmrtsRayTrace::FacetDatum &facet) const {
        return ( m_bullet_model.get_facet( ray, facet ) );
      } 

      inline const PsmrtsShape &shape() const {
        return ( m_shape );
      }

     private:
        PsmrtsShape m_shape;
        bullet::BulletTracerModel m_bullet_model;
  };

  /**
   * @brief BulletTracer implementation using Bullet
   * 
   * 
   */
  BulletTracer::BulletTracer( ) : PsmrtsProduct( "bullet", "bullet" ),
                                  m_config("bullet"),
                                  m_model( std::make_shared<BulletTracerImpl>() ) {  }

  BulletTracer::BulletTracer( const PsmrtsShape &shape ) : 
                              PsmrtsProduct( shape.config().name(), "bullet"),
                              m_config("bullet") {
    m_model = std::make_shared<BulletTracerImpl> ( shape );
    m_config.merge( shape.config() );
    m_config.add( ProductOption( "tracer", "bullet" ) );
    m_config.add( ProductOption( "bullet_optimize_bvh", m_model->useBuildBvh() ) );
    m_config.add( ProductOption( "bullet_compressed",  m_model->useCompression() ) );
    m_config.add_metadata( ProductOption( "bullet_thread_safety", m_model->useThreadSafety() ) );
  }
  
  BulletTracer::BulletTracer( const ProductCart &processed_cart ) {
    this->set_name( processed_cart.name() );
    this->set_type( "bullet" );    
    this->create( processed_cart );
  }     
  
  BulletTracer::~BulletTracer() = default;

      
  
  double BulletTracer::maximum_radius() const {
    return ( shape().maximum_radius() );
  }

  double BulletTracer::minimum_radius() const { 
    return ( shape().minimum_radius() );
  }

  bool BulletTracer::ray_trace( PsmrtsRayTrace &ray ) const {
    bool status = m_model->ray_trace( ray );
    ray.set_tracer_id( this->uid() );
    return ( status );

  }
  bool BulletTracer::get_facet(  const PsmrtsRayTrace &ray, 
                                 PsmrtsRayTrace::FacetDatum &facet) const {
    return ( m_model->get_facet( ray, facet ) );                                 
  }

  const PsmrtsShape &BulletTracer::shape() const {
    return ( m_model->shape() );
  }

  void BulletTracer::create(const ProductCart &cart ) {

    PsmrtsTranslations trans_t = PsmrtsTranslations::create();

    // Check for valid shape type
    if (cart.error_count() > 0 ) {
      std::string mess = "BulletTracer::create(" + cart.name() + 
                        ") has config/spec processing errors: \n" +
                          cart.errors_to_string();
      throw std::runtime_error( mess );          
    }

    ProductConfiguration v_conf = cart.configuration();
    if (cart.error_count() > 0 ) {
      std::string mess = "BulletTracer::create(" + cart.name() + ") has errors: " +
                          cart.errors_to_string();
      throw std::runtime_error( mess );          
    }

    m_config = cart.configuration();

    // std::cout << "Bullet::config: " << order_b.config().to_json().dump(-1) << std::endl;
    // std::cout << "Bullet::residual: " << order_b.residual().to_json().dump(-1) << std::endl;
    // Parse out and validate the shape config
    ProductMaker<PsmrtsShape> shape_m( "shape" );
    ProductConfiguration shape_c( "shape", cart.residual_config() );

    bool shape_made = shape_m.process_config( shape_c,  trans_t );
    ProductCart cart_s = shape_m.cart();
    if ( cart_s.error_count() > 0 ) {
      std::string mess = "BulletTracer::create(" + cart_s.name() + 
                         ") errors constructing shape with config: \n" +
                          cart_s.errors_to_string();
      throw std::runtime_error( mess );          
    }
    // std::cout << "Shape::config: " << order_s.config().to_json().dump(-1) << std::endl;
    // std::cout << "Shape::residual: " << order_s.residual().to_json().dump(-1) << std::endl;    

    // Confirm all is well 
    if (cart_s.error_count() > 0 ) {
      std::string mess = "BulletTracer::create(" + cart.name() + ") has errors: " +
                          cart_s.errors_to_string();
      throw std::runtime_error( mess );          
    }

    // Ensure all options have been parsed/consumed
    if( !cart_s.isvalid() ) {
      std::string mess = "BulletTracer::create(" + cart.name() + ") residual config options present: " +
                          cart_s.to_json().dump(-1);
      throw std::runtime_error( mess );          
    }

    m_config = cart.configuration();
    if ( m_config.contains( "tracer" ) ) {
      if ( m_config.find( "tracer" ).to_string() != "bullet" ) {
        std::string mess = "BulletTracer::create() - tracer type must be \"bullet\""
                            " but found \"" + m_config.find("tracer").to_string() +
                            "\"";
        throw std::runtime_error( mess );
      }
    }

    // Get defaults from specs
    ProductSpecification spec_b = cart.specification();
    bool useCompression = psmrts::is_bool( spec_b.find( "bullet_compression" ).find("default").to_string() );
    if ( m_config.contains( "bullet_compression" ) ) {
      useCompression  = psmrts::is_bool( OptionStringsExtractor( m_config.find( "bullet_compression" ) ).get() );
    }   
    
    bool useBuildBvh = psmrts::is_bool( spec_b.find( "bullet_optimize_bvh" ).find("default").to_string() );
    if ( m_config.contains( "bullet_optimize_bvh" ) ) {
      useBuildBvh  = psmrts::is_bool( OptionStringsExtractor( m_config.find( "bullet_optimize_bvh" ) ).get() );
    } 

    // Create the bullet tracer
    m_model = std::make_shared<BulletTracerImpl> ( shape_m.product(), useCompression, useBuildBvh );
    return;
  }  

} // namespace psmrts
