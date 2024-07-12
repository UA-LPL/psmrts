#ifndef PsmrtsBulletTracerModel_hpp
#define PsmrtsBulletTracerModel_hpp

#include <exception>
#include <string>
#include <vector>
#include <map>
#include <mutex>

#include <Eigen/Geometry>

#include <PsmrtsTracerModel.hpp>
#include <PsmrtsBulletWorldModel.hpp>

namespace psmrts  {
  /**
   * @brief Abstract base class interface for ray tracing models
   * 
   * This abstract class provides a generic interface to ray tracing systems
   * that can be used in the PSMRTS system. These method function declarations
   * defines the minimum set of functions that any PSMRTS-compatible ray
   * tracer must implement.
   * 
   * Note this class does not attempt to specify constructors as there's going
   * to be many. Higher level s/w architectures can manage construction and 
   * usage of models that use this ABC.
   * 
   * @see PsmrtsShapeTracerAdaptor.hpp
   * 
   */
  class PsmrtsBulletTracerModel : public PsmrtsTracerModel {
    public:
      typedef bullet::PsmrtsBulletWorldModel  BulletWorldModel;

      PsmrtsBulletTracerModel( ) {  }
      PsmrtsBulletTracerModel( const BulletWorldModel &bworld ) : 
                               PsmrtsTracerModel( ),
                               m_model( bworld ) {  }

      virtual ~PsmrtsBulletTracerModel() { }

      /* Name of tracer system (PSMRTS) */
      virtual std::string tracer_model_type() const {
        return ( "psmrts" );
      };

      /** Name of tracer model such as  "naifdsk" and "bullet" */
      virtual std::string tracer_model_name() const {
        return ( "bullet" );
      }

      /** Name of the shape model source */
      virtual std::string shapefile() const {
        return ( m_model.name() );
      };

      /** Total number of plates/facets in model */
      virtual size_t plate_count()  const {
        return ( m_model.plate_count() );
      };

      /** Total verticies in the model */
      virtual size_t vertex_count() const {
        return ( m_model.plate_count() );
      };

      /** Returns the maximum radius in the modek */
      virtual double maximum_radius() const {
        return ( m_model.maximum_radius() );
      };

      /**
       * @brief Ray trace method on the shape model in this tracer
       * 
       * The main method used to run individual body-fixed ray traces from 
       * an observer point and a look direction vector. The origin of the
       * "observer" vector is the origin of the planet body and extends
       * outward, presumeably, beyond the maximum radius of the surface in
       * this model. From that point, is the origin of the "lookdir" vector
       * from which to trace for an intersection with the shape model
       * surface. 
       * 
       * The PsmrtsRayTrace class contains the results of the ray trace and can
       * be used in subsequent operations.
       * 
       * @param observer Location of the observer (s/c) relative to the
       *                   center of the target body
       * @param lookdir  Look direction of the ray from the observer to
       *                   trace for intersections
       * @param ray      PsmrtsRayTrace returns the results of the trace
       * @return true    If the trace intercepts the shape
       * @return false   If no ray trace intercept was found
       */
      virtual bool ray_trace( const Eigen::Vector3d &observer,
                              const Eigen::Vector3d &lookdir,
                              PsmrtsRayTrace &ray ) const {
        return ( m_model.ray_trace( observer, lookdir, ray ) );
      }

      /**
       * @brief Get the facet object at the ray intersection
       * 
       * @param ray 
       * @return true 
       * @return false 
       */
      virtual bool get_facet( const PsmrtsRayTrace &ray,
                              PsmrtsRayTrace::FacetDatum &facet ) const {
        return ( m_model.get_facet( ray, facet ) );
      }


      /** Clone a copy of this shape tracer model */
      virtual PsmrtsTracerModel *clone() const {
        return ( new PsmrtsBulletTracerModel( m_model ) );
      }

      /** Return an ellipsoid tracer for the shape */
      virtual PsmrtsTracerModel *ellipsoid() const {
        return ( new PsmrtsBulletTracerModel( m_model.radii() ) );
      }
    
    protected:
      BulletWorldModel m_model;

    private:
      typedef std::map<std::string, BulletWorldModel>  BulletInventory;
      inline static BulletInventory s_bullet_inventory = { };

      inline bool is_model_in_inventory( const std::string &source ) const {
        return ( s_bullet_inventory.find( source ) != s_bullet_inventory.end() );
      }

      inline void add_model_to_inventory( const BulletWorldModel &model ) {
        s_bullet_inventory.insert_or_assign( model.name(), model );
        return;
      }

      inline BulletWorldModel *get_model_from_inventory( const std::string &source ) const {
        auto bt_search = s_bullet_inventory.find( source );
        if ( bt_search != s_bullet_inventory.end() ) {
          return ( &bt_search->second );
        }

        // Not found
        return ( nullptr );
      }
      
  };

} // namespace psmrts

#endif
