/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "ShapeModel.h"

#include <algorithm>
#include <cfloat>
#include <iostream>
#include <iomanip>
#include <vector>

#include <cmath>

#include "Distance.h"
#include "SurfacePoint.h"
#include "IException.h"
#include "IString.h"
#include "NaifStatus.h"
#include "Spice.h"
#include "Target.h"

#include "PsmrtsIsisShapeModel.h"

namespace Isis {
  /**
   * Default constructor creates ShapeModel object, initializing name to an
   * empty string, surface point to an empty surface point, has intersection to
   * FALSE, has normal to FALSE, has ellipsoid intersection to FALSE, normal
   * vector size to 3, and target to NULL.
   */
  PsmrtsIsisShapeModel::PsmrtsIsisShapeModel() : ShapeModel() {
    reset_psmrts_tracer();
  }


  /**
   * Constructs and loads a shape model from a target only
   *
   * This constructor creates PsmrtsIsisShapeModel object, initializing name to an
   * empty string, surface point to an empty surface point, has intersection to
   * FALSE, has normal to FALSE, has ellipsoid intersection to FALSE, normal
   * vector size to 3, and  target to the given target.
   *
   * @param target A pointer to a valid ISIS target.
   */
  PsmrtsIsisShapeModel::PsmrtsIsisShapeModel( Target *target, Pvl &pvl) : 
                                              ShapeModel( target ) {

    reset_psmrts_tracer();

    // Lambda extracts Isis::Distances from a std::vector and returns an Eigen vector
    auto get_radii_vector = [] ( const std::vector<Distance> &d3 ) -> Eigen::Vector3d {
      return ( Eigen::Vector3d( { d3[0].kilometers(), d3[1].kilometers(), d3[2].kilometers() } ) );
    };
    m_ellipsoid_tracer = PsmrtsAdaptedEllipsoidShape( get_radii_vector( target->radii() ) );
  }

  /**
   * @brief Construct a priority model with a tracer (others can be added)
   * 
   * @param model 
   * @param target 
   * @param pvl 
   */
  PsmrtsIsisShapeModel::PsmrtsIsisShapeModel( psmrts::PsmrtsTracerModel *model,
                                              Target *target, Pvl *pvl) : 
                                              ShapeModel( target ) {
                                                
    reset_psmrts_tracer();

    // Add the model to the priorty tracer
    m_priority_tracer.add_tracer( model );

    // Lambda extracts Isis::Distances from a std::vector and returns an Eigen vector
    auto get_radii_vector = [] ( const std::vector<Distance> &d3 ) -> Eigen::Vector3d {
      return ( Eigen::Vector3d( { d3[0].kilometers(), d3[1].kilometers(), d3[2].kilometers() } ) );
    };
    m_ellipsoid_tracer = PsmrtsAdaptedEllipsoidShape( get_radii_vector( target->radii() ) );
  }

  PsmrtsIsisShapeModel::PsmrtsIsisShapeModel( const psmrts::PsmrtsPriorityTracer &tracer,
                                              Target *target, Pvl *pvl ) : 
                                              ShapeModel( target ) {
                                                
    reset_psmrts_tracer();

    // Set the tracer
    m_priority_tracer = tracer;

    // Lambda extracts Isis::Distances from a std::vector and returns an Eigen vector
    auto get_radii_vector = [] ( const std::vector<Distance> &d3 ) -> Eigen::Vector3d {
      return ( Eigen::Vector3d( { d3[0].kilometers(), d3[1].kilometers(), d3[2].kilometers() } ) );
    };
    m_ellipsoid_tracer = PsmrtsAdaptedEllipsoidShape( get_radii_vector( target->radii() ) );
  }


  //! Virtual destructor to destroy the PsmrtsIsisShapeModel object.
  PsmrtsIsisShapeModel::~PsmrtsIsisShapeModel() {  }


  bool PsmrtsIsisShapeModel::intersectSurface( std::vector<double> observerPos,
                                                std::vector<double> lookDirection) {

    this->clearSurfacePoint();

    psmrts::RayTrace raytrace;
    auto v_model_used = this->tracer().ray_trace( Eigen::Vector3d( observerPos.data() ), 
                                                  Eigen::Vector3d( lookDirection.data() ),
                                                  raytrace );
    // Update the intersection info
    return ( update_target_intersection( raytrace, v_model_used ) );
  }


  /**
   * @brief Compute surface intersection with optional occlusion check
   *
   * This method sets the surface point at the given latitude, longitude. The
   * derived model is called to get the radius at that location to complete the
   * accuracy of the surface point, then the derived method is called to complete
   * the intersection.
   *
   * @author 2017-03-23 Kris Becker
   *
   * @param lat          Latitude of the surface point
   * @param lon          Longitude of the surface point
   * @param observerPos  Position of the observer
   * @param backCheck    Flag to indicate occlusion check
   *
   * @return bool        True if the intersection point is valid (visable)
   */
  bool PsmrtsIsisShapeModel::intersectSurface(const Latitude &lat, const Longitude &lon,
                                                const std::vector<double> &observerPos,
                                                const bool &backCheck) {

    this->clearSurfacePoint();
    
    SurfacePoint s_point( lat, lon, Distance( this->ellipsoid().maximum_radius(), Distance::Kilometers ) );
    Eigen::Vector3d observer_pos = this->get_xyz_vector( s_point ) * 2.0;
    Eigen::Vector3d lookdir      = -observer_pos;

    psmrts::RayTrace raytrace;
    auto v_model_used = this->tracer().ray_trace( observer_pos, lookdir, raytrace );

    // Not entirely clear if the lon/lat location is the final observer point or is observerPos??
    const bool SetActiveTrace = true;
    bool status_t = update_target_intersection( raytrace, v_model_used, SetActiveTrace );

    if ( ( true == status_t ) && ( true == backCheck ) ) {
      Eigen::Vector3d b_observer( observerPos.data() );      
      Eigen::Vector3d b_lookdir = this->observer_ray().xyz() - b_observer;
      this->tracer().ray_trace( b_observer, b_lookdir, m_backcheck_trace );  

      // Does this intercept become the final observer intersection??
      status_t = this->observer_ray().isNear( this->backcheck_ray(), m_intercept_tolerance_km );         
    }

    return ( status_t );
  }


  /**
   * @brief Compute surface intersection with optional occlusion check
   *
   * This method sets the surface point at the given latitude, longitude. The
   * derived model is called to get the radius at that location to complete the
   * accuracy of the surface point, them the derived method is called to complete
   * the intersection.
   *
   * @author 2017-03-23 Kris Becker
   *
   * @param surfpt        Absolute point on the surface to check
   * @param observerPos  Position of the observer
   * @param backCheck    Flag to indicate occlusion check
   *
   * @return bool        True if the intersection point is valid (visable)
   */
  bool PsmrtsIsisShapeModel::intersectSurface(const SurfacePoint &surfpt,
                                              const std::vector<double> &observerPos,
                                              const bool &backCheck) {

    this->clearSurfacePoint();

    bool status_t = surfpt.Valid();

    if ( true == status_t  ) {

      Eigen::Vector3d observer_pos = this->get_xyz_vector( surfpt ) * 2.0;
      Eigen::Vector3d lookdir      = -observer_pos;  

      psmrts::RayTrace raytrace;
      auto v_model_used = this->tracer().ray_trace( observer_pos, lookdir, raytrace );      

      // Update the intersection info
      const bool SetActiveTrace = true;
      status_t = update_target_intersection( raytrace, v_model_used, SetActiveTrace );

      if ( (true == status_t ) && ( true == backCheck ) ) {
        Eigen::Vector3d b_observer( observerPos.data() );      
        Eigen::Vector3d b_lookdir = this->observer_ray().xyz() - b_observer;
        this->tracer().ray_trace( b_observer, b_lookdir, m_backcheck_trace );
        status_t = this->observer_ray().isNear( this->backcheck_ray(), m_intercept_tolerance_km );        
      }   
      
    }

    // setSurfacePoint(surfpt);  -- now managed in update_*() methods...
    return ( status_t );
  }


  void PsmrtsIsisShapeModel::calculateDefaultNormal() {
    this->set_active( EllipsoidNormal );
    this->setNormal( this->isis_normal( this->get_active_trace() ) );    
    this->setHasNormal( this->ellipsoid_ray().hasHit() );
    return;
  }


  void PsmrtsIsisShapeModel::calculateLocalNormal(QVector<double *> neighborPoints) {
    this->set_active( ObserverNormal );
    this->setNormal( this->isis_normal( this->get_active_trace() ) );
    this->setHasNormal( this->observer_ray().hasHit() );
    return;
  }

  void PsmrtsIsisShapeModel::calculateSurfaceNormal() {
    calculateDefaultNormal();
    return;
  }

 /**
   * Clears or resets the current surface point.
   */
  void PsmrtsIsisShapeModel::clearSurfacePoint() {
    ShapeModel::clearSurfacePoint();    
    this->reset_all_tracers();
  }


  /**
   * Computes and returns emission angle, in degrees, given the observer
   * position.
   *
   * Emission Angle: The angle between the surface normal vector at the
   * intersection point and the vector from the intersection point to the
   * observer (usually the spacecraft). The emission angle varies from 0 degrees
   * when the observer is viewing the sub-spacecraft point (nadir viewing) to 90
   * degrees when the intercept is tangent to the surface of the target body.
   * Thus, higher values of emission angle indicate more oblique viewing of the
   * target.
   *
   * @param observerBodyFixedPosition  Three dimensional position of the observer,
   *                                   in the coordinate system of the target body.
   *
   * @return The emission angle, in decimal degrees.
   *
   */
  double PsmrtsIsisShapeModel::emissionAngle(const std::vector<double> &observerBodyFixedPosition) {

    Eigen::Vector3d observer_pos( observerBodyFixedPosition.data() );
    Eigen::Vector3d obslookdir = this->observer_ray().xyz() - observer_pos;

    double phase = psmrts::RayTrace::separation_angle( this->observer_ray().normal(), -obslookdir );

    return ( psmrts::radians_to_degrees( phase ) );
  }


  /**
   * Computes and returns incidence angle, in degrees, given the illuminator position.
   *
   * Incidence Angle: The angle between the surface normal vector at the intersection
   * point and the vector from the intersection point to the illuminator (usually the
   * sun).
   *
   * Note: this method does not use the surface model.
   *
   * @param illuminatorBodyFixedPosition Three dimensional position for the illuminator,
   *                                     in the body-fixed coordinate system.
   *
   * @return @b double Incidence angle, in degrees.
   */
  double PsmrtsIsisShapeModel::incidenceAngle(const std::vector<double> &illuminatorBodyFixedPosition) {

    Eigen::Vector3d light_source_pos( illuminatorBodyFixedPosition.data() );
    Eigen::Vector3d sunlookdir = this->observer_ray().xyz() - light_source_pos;

    double incidence = psmrts::RayTrace::separation_angle( this->observer_ray().normal(), -sunlookdir );

    return ( psmrts::radians_to_degrees( incidence ) );
  }

  /**
   * Computes and returns phase angle, in degrees, given the positions of the
   * observer and illuminator.
   *
   * Phase Angle: The angle between the vector from the intersection point to
   * the observer (usually the spacecraft) and the vector from the intersection
   * point to the illuminator (usually the sun).
   *
   * @param observerBodyFixedPosition  Three dimensional position of the observer,
   *                                   in the coordinate system of the target body.
   * @param illuminatorBodyFixedPosition Three dimensional position for the illuminator,
   *                                     in the body-fixed coordinate system.
   *
   * @return @b double Phase angle, in degrees.
   */
  double PsmrtsIsisShapeModel::phaseAngle(const std::vector<double> &observerBodyFixedPosition,
                                          const std::vector<double> &illuminatorBodyFixedPosition) {

    Eigen::Vector3d observer_pos( observerBodyFixedPosition.data() );
    Eigen::Vector3d light_source_pos( illuminatorBodyFixedPosition.data() );

    Eigen::Vector3d obs_lookdir = this->observer_ray().xyz() - observer_pos;
    Eigen::Vector3d sun_lookdir = this->observer_ray().xyz() - light_source_pos;

    double phase = psmrts::RayTrace::separation_angle( -obs_lookdir, -sun_lookdir );

    return ( psmrts::radians_to_degrees( phase ) );
  }


  /**
   * @brief Compute the local radius w/o affecting internal state
   * 
   * @param lat       Desired latitude to get the surface intersection
   * @param lon       Desired longitude to get the surface intersection
   * @return Distance Radius at the given lat/lon 
   */
  Distance PsmrtsIsisShapeModel::localRadius(const Latitude &lat, const Longitude &lon) {

    Distance l_radius( this->ellipsoid().maximum_radius(), Distance::Kilometers );

    SurfacePoint s_point( lat, lon, l_radius );
    Eigen::Vector3d observer_pos = this->get_xyz_vector( s_point ) * 2.0;
    Eigen::Vector3d lookdir      = -observer_pos;

    psmrts::RayTrace raytrace;
    this->tracer().ray_trace( observer_pos, lookdir, raytrace );

    // If we got a hit, set the radius
    if ( raytrace.hasHit() ) {
      l_radius.setKilometers( raytrace.radius() );
    }
    else {
      l_radius = Distance();
    }

    return ( l_radius );
  }


  /** This is not a DEM! */
  bool PsmrtsIsisShapeModel::isDEM() const {
    return ( false );
  }

  /** Returns the normal of the currently active trace */
  std::vector<double>  PsmrtsIsisShapeModel::normal() {
    return ( this->isis_std_vector( this->get_active_trace().normal() ) ) ;
  }


/**
 * @brief Default occulsion implementation
 *
 *  This method is originally copied from Sensor::SetLocalGround(bool
 *  backCheck). This version checks for the emission angle from the observer to
 *  be less than or equal to 90 degrees.
 *
 *  It is recommended that models derived from this base class reimplement this
 *  method if a more robust, efficent test can be made.
 *
 *  Note this implementation does not handle occlusion!
 *
 * @author 2017-03-17 Kris Becker
 *
 * @param observerPos   Position of the observer in body fixed coordinates
 * @param lookDirection Look direction from the observer
 *
 * @return bool True if the point is not visable, false if it can be seen
 */
  bool PsmrtsIsisShapeModel::isVisibleFrom(const std::vector<double> observerPos,
                                           const std::vector<double> lookDirection)  {

    bool isvisible = false;

    if ( this->observer_ray().hasHit() ) {
      Eigen::Vector3d observer_pos( observerPos.data() );
      Eigen::Vector3d obslookdir( lookDirection.data() );

      if ( !compare_ray_initial_conditions( this->backcheck_ray(), observer_pos, obslookdir ) ) {
        this->tracer().ray_trace( observer_pos, obslookdir, m_backcheck_trace );
      }

      isvisible = this->observer_ray().isNear( this->backcheck_ray(), m_intercept_tolerance_km );
    }

    // Return status
    return ( isvisible );
  }


  /**
   * Finds the intersection point on the ellipsoid model using the given
   * position of the observer (spacecraft) and direction vector from the
   * observer to the target (body).
   *
   * @param observerBodyFixedPosition  Three dimensional position of the observer,
   *                                   in the coordinate system of the target body.
   * @param observerLookVectorToTarget Three dimensional direction vector from
   *                                   the observer to the target.
   *
   * @return @b bool Indicates whether this shape model found a valid ellipsoid intersection.
   */
  psmrts::RayTrace PsmrtsIsisShapeModel::run_ellipsoid_trace( const psmrts::RayTrace &ray ) const {

    psmrts::RayTrace ray_e( ray.observer(), ray.lookdir() );
    if ( ray.hasHit() ) {
      this->ellipsoid().ray_trace( ray.observer(), ray.lookdir(), ray_e );
    }

    return ( ray_e );
  }

  bool PsmrtsIsisShapeModel::update_target_intersection( const psmrts::RayTrace &raytrace, 
                                                         const psmrts::PsmrtsTracerModel *model_used,
                                                         const bool activate ) {

    m_observer_to_target_trace = raytrace;
    this->setHasIntersection( raytrace.hasHit() );
    this->setSurfacePoint( this->init_isis_surface_point_from_ray( raytrace ));

    if ( activate ) this->set_active( ObserverNormal );

    const bool DoNotActivateTrace = false;
    this->update_ellipsoid_intersection( run_ellipsoid_trace( raytrace ), nullptr, DoNotActivateTrace );
    return ( raytrace.hasHit() );
  }

  bool PsmrtsIsisShapeModel::update_ellipsoid_intersection( const psmrts::RayTrace &raytrace,
                                                            const psmrts::PsmrtsTracerModel *model_used,
                                                            const bool activate ) {

    m_ellipsoid_trace = raytrace;
    if ( activate ) this->set_active( EllipsoidNormal );

    return ( raytrace.hasHit() );
  }
}
