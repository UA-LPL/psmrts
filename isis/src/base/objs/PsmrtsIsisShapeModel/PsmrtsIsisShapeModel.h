#ifndef PsmrtsIsisShapeModel_h
#define PsmrtsIsisShapeModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <vector>

#include <Eigen/Geometry>
#include <RayTrace.hpp>
#include <PsmrtsUtilities.hpp>
#include <PsmrtsTracerModel.hpp>
#include <PsmrtsShapeTracerAdapter.hpp>
#include <PsmrtsPriorityTracer.hpp>
#include <NaifEllipsoidShape.hpp>


namespace Isis {
  /**
   * @brief PSMRTS Shape Model & Ray Trace Implementation in ISIS
   *
   * This class implements a generic interface to the Planaetary Shape Model
   * and Ray Tracing System (PSMRTS) in the ISIS environment. 
   *
   * @author 2024-03-16 Kris J. Becker, University of Arizona
   *
   * @internal
   */
  class PsmrtsIsisShapeModel : public ShapeModel {
    public:
      typedef enum { NoNormal, ObserverNormal, EllipsoidNormal, LightSourceNormal, BackCheckNormal } ActiveNormal;

       /** Constructors */ 
      PsmrtsIsisShapeModel( );
      PsmrtsIsisShapeModel( Target *target, Pvl &pvl );
      PsmrtsIsisShapeModel( psmrts::PsmrtsTracerModel *model,
                            Target *target = nullptr, Pvl *pvl = nullptr );
      PsmrtsIsisShapeModel( const psmrts::PsmrtsPriorityTracer &tracer,
                            Target *target = nullptr, Pvl *pvl = nullptr );                            
      // PsmrtsIsisShapeModel( const json &psmrts_config );


      /** Destructor */  
      virtual ~PsmrtsIsisShapeModel();

      // Intersect the shape model
      virtual bool intersectSurface( std::vector<double> observerPos,
                                     std::vector<double> lookDirection );

      virtual bool intersectSurface( const Latitude &lat, const Longitude &lon,
                                     const std::vector<double> &observerPos,
                                     const bool &backCheck = true );

      virtual bool intersectSurface( const SurfacePoint &surfpt,
                                     const std::vector<double> &observerPos,
                                     const bool &backCheck = true );



      // Calculate the default normal of the current intersection point
      virtual void calculateDefaultNormal();

      // Calculate the local normal of the current intersection point
      // (relative to neighbor points)
      virtual void calculateLocalNormal( QVector<double *> neighborPoints );

      // Calculate the surface normal of the current intersection point
      // (relative to ellipsoid)
      virtual void calculateSurfaceNormal();

      // Clear current point
      virtual void clearSurfacePoint();

      // Calculate the emission angle of the current intersection point
      virtual double emissionAngle( const std::vector<double> & sB );

      // Calculate the incidence angle of the current intersection point
      virtual double incidenceAngle( const std::vector<double> &uB );

      // Calculate the phase angle of the current intersection point
      virtual double phaseAngle( const std::vector<double> &sB,
                                 const std::vector<double> &uB);

      // Return local radius from shape model
      virtual Distance localRadius( const Latitude &lat, const Longitude &lon );

      /**
       * Indicates whether this shape model is from a DEM. This method is used to
       * determine whether the Camera class will calculate the local normal using
       * neighbor points. This method is pure virtual and must be implemented by
       * all ShapeModel classes.  The parent implementation returns false.
       *
       * @return bool Indicates whether this is a DEM shape model.
       */
      virtual bool isDEM() const;

      // Set current surface point
      // virtual void setSurfacePoint( const SurfacePoint &surfacePoint );

      // Return the normal (surface or local) of the current intersection point
      virtual std::vector<double>  normal();

      // Determine if the internal intercept is occluded from the observer/lookdir
      virtual bool isVisibleFrom( const std::vector<double> observerPos,
                                  const std::vector<double> lookDirection );




      ///***--->>> PSMRTS Specializations <<<---*** ////
      inline const psmrts::PsmrtsPriorityTracer &tracer() const {
        return ( m_priority_tracer );
      }

      inline const psmrts::PsmrtsTracerModel &ellipsoid() const {
        return ( m_ellipsoid_tracer );
      }

      inline double tolerance() const {
        return ( m_intercept_tolerance_km );
      }

      inline double set_tolerance( const double intercept_precision = InterceptTolerance_km ) {
        double old_tolerance = m_intercept_tolerance_km;
        m_intercept_tolerance_km = intercept_precision;
        return ( old_tolerance );
      }

      inline const psmrts::PsmrtsTracerModel *surface_model_with_intercept() const {
        return ( m_model_used );
      }

      inline const psmrts::RayTrace &observer_ray() const {
        return ( m_observer_to_target_trace );
      }

      inline const psmrts::RayTrace &light_source_ray() const {
        return ( m_light_source_trace );
      }

      inline const psmrts::RayTrace &ellipsoid_ray() const {
        return ( m_ellipsoid_trace );
      }

      inline const psmrts::RayTrace &backcheck_ray() const {
        return ( m_ellipsoid_trace );
      }

      inline bool has_surface_intercept( ) const {
        return ( this->observer_ray().hasHit() );
      }

      inline SurfacePoint init_isis_surface_point_from_ray( const psmrts::RayTrace &ray ) const {
        SurfacePoint point;
        point.FromNaifArray( ray.xyz().data() );
        return ( point );
      }
      
      inline bool compare_ray_initial_conditions( const psmrts::RayTrace &ray,
                                                  const Eigen::Vector3d &observer, 
                                                  const Eigen::Vector3d &lookdir ) {

        if ( !ray.hasHit() )                                    return ( false ); 
        if ( !psmrts::isEqual( ray.observer(), observer ) ) return ( false );
        if ( !psmrts::isEqual( ray.lookdir(),  lookdir   ) ) return ( false );                                                  
        return ( true );  // Looks good!
      }

      inline Eigen::Vector3d get_xyz_vector( const SurfacePoint &point ) const {
        Eigen::Vector3d v_e;
        point.ToNaifArray( v_e.data() );
        return ( v_e );
      }

      inline std::vector<double> isis_std_vector(const Eigen::Vector3d &v_e ) const {
        std::vector<double> isis_v( { v_e[0], v_e[1], v_e[2] } );
        return ( isis_v );
      }

      inline std::vector<double> isis_normal(const psmrts::RayTrace &ray ) const {
        return ( isis_std_vector( ray.normal() ) );
      }

      inline std::vector<double> observer_normal() const {
        return ( this->isis_normal( this->observer_ray() ) );
      }

      inline std::vector<double> ellipsoid_normal() const {
        return ( this->isis_normal( this->ellipsoid_ray() ) );
      }      

      inline ActiveNormal whos_active() const {
        return ( m_active_normal );
      }

      inline ActiveNormal set_active( const ActiveNormal &actor ) {
        ActiveNormal current_s = m_active_normal;
        m_active_normal = actor;
        return ( current_s ); 
      }

    protected:
    // This routine is actually intersectEllipsoid()...
      psmrts::RayTrace run_ellipsoid_trace( const psmrts::RayTrace &ray ) const;        


    private:
      typedef psmrts::PsmrtsShapeTracerAdapter<naif::NaifEllipsoidShape> PsmrtsAdaptedEllipsoidShape;
      inline static const double InterceptTolerance_km = 1.0e-6;

      psmrts::PsmrtsPriorityTracer m_priority_tracer;
      PsmrtsAdaptedEllipsoidShape  m_ellipsoid_tracer;
      psmrts::PsmrtsTracerModel    *m_model_used;
      double                       m_intercept_tolerance_km;

      psmrts::RayTrace             m_observer_to_target_trace;
      psmrts::RayTrace             m_light_source_trace;
      psmrts::RayTrace             m_ellipsoid_trace;
      psmrts::RayTrace             m_backcheck_trace;
      ActiveNormal                 m_active_normal;


      inline void reset_all_tracers( ) {
        m_model_used       = nullptr;
        m_observer_to_target_trace.reset();
        m_light_source_trace.reset();
        m_ellipsoid_trace.reset();
        m_backcheck_trace.reset();
        m_active_normal = NoNormal;
        return;
      }

      inline void reset_psmrts_tracer( ) {
        m_priority_tracer  = psmrts::PsmrtsPriorityTracer();
        m_ellipsoid_tracer = PsmrtsAdaptedEllipsoidShape();
        m_intercept_tolerance_km = InterceptTolerance_km;

        reset_all_tracers();

        // Now some ISIS internals
        setName("psmrts");
        return;
      }


      inline const psmrts::RayTrace &get_active_trace( ) const {
        if ( ObserverNormal    == this->whos_active() ) {  return ( this->observer_ray() ); }
        if ( EllipsoidNormal   == this->whos_active() ) {  return ( this->ellipsoid_ray() ); }
        if ( LightSourceNormal == this->whos_active() ) {  return ( this->light_source_ray() ); }
        if ( BackCheckNormal   == this->whos_active() ) {  return ( this->backcheck_ray() ); }
        return ( this->observer_ray() );
      } 

      bool update_target_intersection( const psmrts::RayTrace &raytrace, 
                                       const psmrts::PsmrtsTracerModel *model_used,
                                       const bool activate = true );

      bool update_ellipsoid_intersection( const psmrts::RayTrace &raytrace,
                                          const psmrts::PsmrtsTracerModel *model_used,
                                          const bool activate = true );
      
  };
};

#endif
