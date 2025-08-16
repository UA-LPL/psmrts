#ifndef PsmrtsShapeTracerAdapter_hpp
#define PsmrtsShapeTracerAdapter_hpp

#include <exception>
#include <string>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsTracerModel.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>

namespace psmrts {

  template<typename MODEL> 
    class PsmrtsShapeTracerAdapter : public PsmrtsTracerModel {
      public:
        PsmrtsShapeTracerAdapter( ) {  }
        
        PsmrtsShapeTracerAdapter( const std::string &source ) {  
          m_model_pst = MODEL( source );  // Must provide a string-type constructor
        }

        PsmrtsShapeTracerAdapter( const std::string &source, const Eigen::Vector3d &radii ) {  
          m_model_pst = MODEL( source, radii );  // Must provide a string-type constructor with radii
        }

        PsmrtsShapeTracerAdapter( const MODEL &model ) {  
          m_model_pst = model;  // Must be copyable and recommended to have a small footprint
        }

        virtual ~PsmrtsShapeTracerAdapter() { }

        /** Returns type of tracer model, ie. naifdsk */
        virtual std::string tracer_model_type() const {
          return ( m_model_pst.tracer_model_type() );
        }

        /** Returns tracer model name, ie. bullet, DskKernelModel */
        virtual std::string tracer_model_name() const  {
          return ( m_model_pst.tracer_model_name() );
        }

        /** Returns identifying information of tracer model - type and name */
        virtual std::string shape_tracer_id()   const  {
          return ( m_model_pst.shape_tracer_id() );
        }

        /** Returns related file or shape related information, eg. referred file name or specific Ellipsoid shape */
        virtual std::string shapefile()         const  {
          return ( m_model_pst.shapefile() );
        }

        /** Returns number of plates, if relevant to format */
        virtual size_t plate_count()  const  {
          return ( m_model_pst.plate_count() );
        }

        /** Returns number of vertices, if relevant to format  */
        virtual size_t vertex_count() const  {
          return ( m_model_pst.vertex_count() );
        }

        /** Returns maximum radius value for shape */
        virtual double maximum_radius() const {
          return ( m_model_pst.maximum_radius() );
        }

        virtual size_t use_count() const {
          return ( m_model_pst.use_count() );
        }        

        /**
         * @brief General model ray trace method
         * 
         * The main method used to run individual body-fixed ray traces from
         * an observer point and look direction vector. The origin of the "observer"
         * vector is the origin of the planet body and presumeably extends beyond the
         * maximum radius of the surface in the models. From that point, is the origin 
         * of the "lookdir" vector from which to trace or an intersection with the 
         * shape model surface.
         * 
         * Refer to PsmrtsRayTrace class for results of the ray trace and related
         * accessible data.
         * 
         * @param observer Location of the observer (s/c) relative to the center of
         *                  the target body
         * @param lookdir  Look direction of the ray from the observer to trace for 
         *                  intersections
         * @param ray      PsmrtsRayTrace returns the results of the trace
         * @return true    If trace intercepts shape
         * @return false   If trace fails to intercept
         */
        virtual bool ray_trace( const Eigen::Vector3d &observer,
                                const Eigen::Vector3d &lookdir,
                                PsmrtsRayTrace &ray ) const  {
          return ( m_model_pst.ray_trace(  observer, lookdir, ray ) );
        }

        /** Returns the intercepted facet resulting from a ray trace */
        virtual bool get_facet( const PsmrtsRayTrace &ray,
                                PsmrtsRayTrace::FacetDatum &facet ) const {
          return ( m_model_pst.get_facet( ray, facet ) );
        }

        /** Creates a clone of the related shape tracer */
        virtual PsmrtsTracerModel *clone() const  {
          return ( new PsmrtsShapeTracerAdapter<MODEL>( m_model_pst.clone() ) );
        }

        /** Returns a null pointer - WIP, will be adjusted in the future to return ellipsoid shape model */
        virtual PsmrtsTracerModel *ellipsoid() const  {
          return ( nullptr );
          // return ( new PsmrtsShapeTracerAdapter<MODEL>( m_model_pst.ellipsoid() );
        }

        /** Returns the model object */
        inline const MODEL &model() const {
          return ( m_model_pst );
        }

        private:
          MODEL   m_model_pst;
      
    };

} // namespace psmrts

#endif
