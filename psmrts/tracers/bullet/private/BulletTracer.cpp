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
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/PsmrtsFactory.hpp>

namespace psmrts {

  /**
   * @brief Implementation of the PIMPL class for fully private Bullet usage
   * 
   * This PIMPL class fully contains the Bullet implementation in the PSMRTS
   * API. 
   * 
   */
  // class BulletTracer::BulletTracerImpl : public PsmrtsProduct {
  class BulletTracer::BulletTracerImpl {
    public:
      BulletTracerImpl( ) : 
                            m_shape(), 
                            m_bullet_model() { }
      BulletTracerImpl( const SharedShape &shape,
                        const bool useCompression = true,
                        const bool buildBvh = true   ) : 
                        m_shape( shape ),
                        m_bullet_model( bullet::PsmrtsBulletMeshMap( shape->get_mesh(), 
                                                                     shape->name(), 0), 
                                                                     shape->name(),
                                                                     useCompression,
                                                                     buildBvh ) { 
      }
      ~BulletTracerImpl() = default;

      inline bool useCompression() const {
        return ( m_bullet_model.model()->useCompression() );
      }

      inline bool useBuildBvh() const {
        return ( m_bullet_model.model()->useBuildBvh() );
      }

      inline bool useThreadSafety() const {
        return ( m_bullet_model.model()->useThreadSafety() );
      }

      inline bool ray_trace( PsmrtsRayTrace &ray ) const {
        bool status =  m_bullet_model.ray_trace( ray );
        return ( status );
      }

      inline bool get_facet(  const PsmrtsRayTrace &ray, 
                       PsmrtsRayTrace::FacetDatum &facet) const {
        return ( m_bullet_model.get_facet( ray, facet ) );
      } 

      inline const SharedShape &shape() const {
        return ( m_shape );
      }

     private:
        SharedShape m_shape;
        bullet::BulletTracerModel m_bullet_model;
  };

  /**
   * @brief BulletTracer implementation using Bullet
   * 
   * 
   */
  BulletTracer::BulletTracer( ) : PsmrtsProduct( "bullet", "tracer", "bullet" ),
                                  m_model( std::make_shared<BulletTracerImpl>() ),
                                  m_config("bullet") {  }

  BulletTracer::BulletTracer( const SharedShape &shape ) : 
                              PsmrtsProduct( shape->config().name(), "tracer", "bullet"),
                              m_config("bullet") {

    if ( !shape ) {
      std::string mess = "BulletTracer - Invalid PsmrtsShape in constructor!";
      throw std::runtime_error( mess );
    }

    m_model = std::make_shared<BulletTracerImpl> ( shape );
    m_config.merge( shape->config() );
    m_config.add( ProductOption( "tracer", "bullet" ) );
    m_config.add_metadata( ProductOption( "tracer_uid", PsmrtsUID::to_string( this->uid() ) ) );
    m_config.add( ProductOption( "bullet_optimize_bvh", m_model->useBuildBvh() ) );
    m_config.add( ProductOption( "bullet_compressed",  m_model->useCompression() ) );
    m_config.add_metadata( ProductOption( "bullet_thread_safety", m_model->useThreadSafety() ) );
  }
  
  BulletTracer::BulletTracer( const ProductCart &processed_cart ) :
                              PsmrtsProduct( processed_cart.configuration().name(), "tracer", "bullet") {
    this->create( processed_cart );
  }
  
  BulletTracer::BulletTracer( const ProductCart &processed_cart,
                              const SharedShape &shape ) :
                              PsmrtsProduct( processed_cart.configuration().name(), "tracer", "bullet") {
    this->create( processed_cart, shape );
  }    
  
  BulletTracer::~BulletTracer() = default;

      
  
  double BulletTracer::maximum_radius() const {
    return ( shape()->maximum_radius() );
  }

  double BulletTracer::minimum_radius() const { 
    return ( shape()->minimum_radius() );
  }

  bool BulletTracer::ray_trace( PsmrtsRayTrace &ray ) const {
    bool status = m_model->ray_trace( ray );
    if ( true == status ) ray.set_tracer_id( this->uid() );
    return ( status );

  }
  bool BulletTracer::get_facet(  const PsmrtsRayTrace &ray, 
                                 PsmrtsRayTrace::FacetDatum &facet) const {
    return ( m_model->get_facet( ray, facet ) );                                 
  }

  const SharedShape &BulletTracer::shape() const {
    return ( m_model->shape() );
  }

  void BulletTracer::create(const ProductCart &cart,
                            const SharedShape &shape ) {
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

    // Check to see if the cart contains a valid shape id and see if its in the
    // factory. We prefer use of that one over creating many others. We can
    // validate that the shape associated with this ID is consistent with the 
    // configuration of the shape  and the current passed one. Not this is
    // currently only possible because this is a .cpp file.
    SharedShape shape_t;
    ProductCart::UIDType shape_uid = cart.get_uid();
    std::string name_t = cart.configuration().name();
    if ( shape ) {
      shape_t = shape;
    }
    else if ( PsmrtsUID::is_valid_uid( shape_uid ) ) {
      // See if its in the default factory and use it!
      shape_t = PsmrtsFactory().find_shape( shape_uid );
    }
    else {
      // Parse out and validate the shape config
      ProductMaker<PsmrtsShape> shape_m( name_t );
      ProductConfiguration shape_c( name_t, cart.residual_config() );

      (void) shape_m.process_config( shape_c,  trans_t );
      ProductCart cart_s = shape_m.cart();

      if ( cart_s.error_count() > 0 ) {
        std::string mess = "BulletTracer::create(" + cart_s.name() + 
                          ") errors constructing shape with config: \n" +
                            cart_s.errors_to_string();
        throw std::runtime_error( mess );          
      }

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
      shape_t = shape_m.product();

      // Since we can lets put the shape in the factory!
      PsmrtsFactory().add( shape_t );
    }


    // Initialize with the shape configuration
    m_config = shape->config();

    if ( v_conf.contains( "tracer" ) ) {
      if ( v_conf.find( "tracer" ).to_string() != "bullet" ) {
        std::string mess = "BulletTracer::create() - tracer type must be \"bullet\""
                            " but found \"" + v_conf.find("tracer").to_string() +
                            "\"";
        throw std::runtime_error( mess );
      }
    }
    m_config.add( v_conf.find("tracer") );

    // Add the tracer UID to the metadata
    m_config.add_metadata( ProductOption( "tracer_uid", PsmrtsUID::to_string( this->uid() ) ) );

    // Get defaults from specs
    ProductSpecification spec_b = cart.specification();
    bool useCompression = psmrts::is_bool( spec_b.find( "bullet_compression" ).find("default").to_string() );
    if ( v_conf.contains( "bullet_compression" ) ) {
      useCompression  = psmrts::is_bool( OptionStringsExtractor( v_conf.find( "bullet_compression" ) ).get() );
      m_config.add( ProductOption( "bullet_compression", useCompression) );
    }   
    
    bool useBuildBvh = psmrts::is_bool( spec_b.find( "bullet_optimize_bvh" ).find("default").to_string() );
    if ( v_conf.contains( "bullet_optimize_bvh" ) ) {
      useBuildBvh  = psmrts::is_bool( OptionStringsExtractor( m_config.find( "bullet_optimize_bvh" ) ).get() );
      m_config.add( ProductOption( "bullet_optimize_bvh", useBuildBvh) );
    } 

    // Create the bullet tracer
    m_model = std::make_shared<BulletTracerImpl> ( shape_t, useCompression, useBuildBvh );
    return;
  }

} // namespace psmrts
