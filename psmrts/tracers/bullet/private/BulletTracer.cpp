#include <string>
#include <memory>

#include "PsmrtsBulletWorldModel.hpp"
#include "BulletTracerModel.hpp"
#include "PsmrtsBulletMeshMap.hpp"
#include "../BulletTracer.hpp"

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
      BulletTracerImpl( ) : PsmrtsProduct( "bulletmodel", "tracer" ), 
                            m_bullet_model() { }
      BulletTracerImpl( const PsmrtsShape &shape  ) : 
                        PsmrtsProduct( shape.name(), "tracer" ),
                        m_bullet_model( bullet::PsmrtsBulletMeshMap( shape.get_mesh(), 
                                                                     shape.name(), 0), 
                                                                     shape.name() ) { 
      }
      ~BulletTracerImpl() = default;


      inline bool ray_trace( PsmrtsRayTrace &ray ) const {
        ray.set_tracer_id( this->uid() );
        return ( m_bullet_model.ray_trace( ray ) );
      }

      inline bool get_facet(  const PsmrtsRayTrace &ray, 
                       PsmrtsRayTrace::FacetDatum &facet) const {
        return ( m_bullet_model.get_facet( ray, facet ) );
      } 

     private:
        bullet::BulletTracerModel m_bullet_model;
  };

  /**
   * @brief BulletTracer implementation using Bullet
   * 
   * 
   */
  BulletTracer::BulletTracer( ) : PsmrtsProduct( "bullet", "tracer" ),
                                  m_shape(),
                                  m_configured("bullet"),
                                  m_model( std::make_shared<BulletTracerImpl>() ) {  }

  BulletTracer::BulletTracer( const PsmrtsShape &shape ) : 
                              PsmrtsProduct( shape.config().name(), "tracer"),
                              m_shape( shape ),
                              m_configured("bullet") {
    m_model = std::make_shared<BulletTracerImpl> ( shape );
    m_configured.add( ProductOption( "bullet_optimize_bvh", "false" ) );
    m_configured.add( ProductOption( "bullet_compressed", "false" ) );
    m_configured.add( ProductOption( "bullet_thread_safety", "false" ) );
  }                       
  
  BulletTracer::~BulletTracer() = default;

      
  double BulletTracer::maximum_radius() const {
    return ( m_shape.maximum_radius() );
  }

  double BulletTracer::minimum_radius() const { 
    return ( m_shape.minimum_radius() );
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

} // namespace psmrts