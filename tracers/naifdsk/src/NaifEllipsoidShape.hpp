#ifndef NaifEllipsoidShape_hpp
#define NaifEllipsoidShape_hpp


#include <string>
#include <exception>
#include <Eigen/Geometry>

#include <NaifUtilities.hpp>
#include <PsmrtsRayTrace.hpp>
#include <PsmrtsTracerModel.hpp>

namespace naif {


  class NaifEllipsoidShape {
    public:
      // Constructors
      NaifEllipsoidShape() : m_a_radius( 1.0), 
                             m_b_radius( 1.0 ), 
                             m_c_radius( 1.0 ),
                             m_body( "UnitSpheroid" ) {  }
      NaifEllipsoidShape( const Eigen::Vector3d &radii,
                          const std::string &name = "TriaxialEllipsoid" ) : m_a_radius( radii[0] ), 
                                                          m_b_radius( radii[1] ), 
                                                          m_c_radius( radii[2] ),
                                                          m_body( name ) { 
        validate();
      }
      NaifEllipsoidShape(const double radius,
                         const std::string &name = "Spheroid" ) : m_a_radius( radius ), 
                                                 m_b_radius( radius ), 
                                                 m_c_radius( radius ),
                                                 m_body( name ) { 
        validate();
      }
      NaifEllipsoidShape(const double &a, const double &c,
                         const std::string &name = "Ellipsoid" ): m_a_radius( a ), 
                                                             m_b_radius( a ), 
                                                             m_c_radius( c ),
                                                             m_body( name ) { 
        validate();
      }
      NaifEllipsoidShape(const double &a, const double &b, const double &c,
                         const std::string &name = "TriaxialEllipsoid" ) : 
                         m_a_radius( a ), m_b_radius( b ), m_c_radius( c ), m_body( name ) {  
        validate();
      }

      // Destructor
      virtual ~NaifEllipsoidShape() { }

      /** Returns the model type, ie. psmrts */
      inline std::string tracer_model_type() const {
        return ( std::string( "psmrts" ) );
      }

      /** Returns the model name, ie. NaifEllipsoid */
      inline std::string tracer_model_name() const {
        return ( std::string( "NaifEllipsoid" )) ;
      }

      /** Returns combination of model type, model name, and the shape name */
      inline std::string shape_tracer_id() const {
        std::string shapename = shapefile();
        if ( shapename.length() == 0 ) shapename = "none";
        return ( tracer_model_type() + "::" + tracer_model_name() + "::" + shapename );
      }      

      /** Returns name of the shapefile, indicated by name in constructor used above */
      inline std::string shapefile() const {
        return ( m_body ); 
      }

      /** Returns value of a */
      const double &a() const {
        return ( m_a_radius );
      }

      /** Returns value of b */
      const double &b() const {
        return ( m_b_radius );
      }

      /** Returns value of c */
      const double &c() const {
        return ( m_c_radius );
      }

      /** Returns radii vector */
      inline Eigen::Vector3d radii() const {
        return ( Eigen::Vector3d( { a(), b(), c() } ) );
      }

      /** Returns minimum radius */
      inline double minimum_radius() const {
        return ( std::min( a(), std::min( b(), c() ) ) );
      }

      /** Returns maximum radius */
      inline double maximum_radius() const {
        return ( std::max( a(), std::max( b(), c() ) ) );
      }

      /** Returns number of plates (0 due to nature of Ellipsoid format) */
      inline size_t plate_count() const {
        return ( 0 );
      }

      /** Returns number of vertices (0 due to nature of Ellipsoid format) */
      inline size_t vertex_count() const {
        return ( 0 );
      }

      /**
       * @brief Ray Trace method for Ellipsoid Shape - Point Result
       * 
       * This method is used to run individual body-fixed ray traces
       * from an observer point and look direction. The origin of the
       * "observer" vector is the origin of the planet body and presumeably
       * extends outward beyond the maximum radius of the surface. From 
       * that point, is the origin of the "lookdir" vector from which
       * to trace for an intersection with the shape model.
       * 
       * In this function, the results of the ray are stored into
       * an Eigen::Vector3d, point.
       * 
       * @param observer Location of the observer (s/c) relative to the
       *                   center of the target body
       * @param lookdir Look direction of the ray from the observer to
       *                   trace for intersections
       * @param point   Eigen::Vector3d holding the trace results 
       * @return true   If the trace intercepts the shape 
       * @return false  If the trace fails to intercept
       */
      inline bool ray_trace( const Eigen::Vector3d &observer,
                             const Eigen::Vector3d &lookdir,
                             Eigen::Vector3d &point) const {
        
        SpiceBoolean found;
        (void) surfpt_c( observer.data(), lookdir.data(), a(), b(), c(), point.data(), &found );
        check_naif_errors();

        return ( found == SPICETRUE );
      }

      /** Retuns the vector normal of the input point */
      inline Eigen::Vector3d normal( const Eigen::Vector3d &point ) const {  
        Eigen::Vector3d normvec;
        (void) surfnm_c( a(), b(), c(), point.data(), normvec.data() );
        return ( normvec );
      }

      /**
       * @brief Ray Trace method for Ellipsoid Shape - PsmrtsRayTrace Result
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
      inline bool ray_trace( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir,
                             psmrts::PsmrtsRayTrace &ray ) const {

        ray.reset( observer, lookdir );
        psmrts::PsmrtsRayTrace::RayTraceDatum &datum_r = ray.datum();

        datum_r.m_hit = this->ray_trace( observer, lookdir, datum_r.m_xyz );
        if ( datum_r.hasHit() ) {
          datum_r.m_normal = this->normal( datum_r.m_xyz );
        }

        // Returns intercept state
        return ( ray.hasHit() );
      }

      /** Virtual facet getter method - generally inapplicable to Ellipsoid models */
      virtual bool get_facet( const psmrts::PsmrtsRayTrace &ray,
                              psmrts::PsmrtsRayTrace::FacetDatum &facet ) const {

      // Sanity check validity of raytrace
        facet.m_has_facet = false;
        return ( facet.isValid() );
      }

      /** Returns clone of Ellipsoid shape */
      inline NaifEllipsoidShape clone() const {
        return ( *this );
      }

      /** There are no shared instances */
      inline size_t use_count() const {
        return (0 );
      }

    private:
      double      m_a_radius;
      double      m_b_radius;
      double      m_c_radius;
      std::string m_body;

      inline void validate() const {
        if ( minimum_radius() <= 0.0 ) {
          std::string mess = "Invalid radii (" + std::to_string(a()) + "," + 
                              std::to_string(b()) + "," + std::to_string(c()) + " - must be > 0";
          throw std::runtime_error( mess );
        }
        return;
      }
  };
}

#endif
