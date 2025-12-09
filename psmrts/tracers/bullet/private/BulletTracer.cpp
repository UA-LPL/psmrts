#include <string>
#include <memory>


#include "PsmrtsBulletWorldModel.hpp"
#include "BulletTracerModel.hpp"
#include "../BulletTracer.hpp"

#include "PsmrtsBulletMeshMap.hpp"

namespace psmrts {

  /**
   * @brief Implementation of the PIMPL class for fully private Bullet usage
   * 
   * This PIMPL class fully contains the Bullet implementation in the PSMRTS
   * API. 
   * 
   */
  class BulletTracer::BulletTracerImpl {
    public:
      BulletTracerImpl( ) : m_bullet_model() { }
      BulletTracerImpl( const PsmrtsShape &shape  ) :
        m_bullet_model( bullet::PsmrtsBulletMeshMap( shape.get_mesh(), shape.name(), 0), shape.name() ) { 
      }
      ~BulletTracerImpl() = default;

      inline const std::string &name() const {
        return ( m_bullet_model.shapefile() );
      }

      inline double maximum_radius() const { 
        return ( m_bullet_model.maximum_radius() );
      }

      inline bool ray_trace( PsmrtsRayTrace &ray ) const {
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
  BulletTracer::BulletTracer( ) : m_model( std::make_shared<BulletTracerImpl>() ) {  }

  BulletTracer::BulletTracer( const PsmrtsShape &shape ) {
    m_model = std::make_shared<BulletTracerImpl> (shape );
  }                       
  
  BulletTracer::~BulletTracer() = default;

  /** Return the name of the shape file */
  const std::string &BulletTracer::name() const {
    return ( m_model->name() );
  }

      
  double BulletTracer::maximum_radius() const {
    return ( m_model->maximum_radius() );
  }

  bool BulletTracer::ray_trace( PsmrtsRayTrace &ray ) const {
    return ( m_model->ray_trace( ray ) );

  }
  bool BulletTracer::get_facet(  const PsmrtsRayTrace &ray, 
                                 PsmrtsRayTrace::FacetDatum &facet) const {
    return ( m_model->get_facet( ray, facet ) );                                 
  }

} // namespace psmrts