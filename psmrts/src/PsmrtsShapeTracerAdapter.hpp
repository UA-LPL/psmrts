#ifndef PsmrtsShapeTracerAdapter_hpp
#define PsmrtsShapeTracerAdapter_hpp

#include <exception>
#include <string>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsTracerModel.hpp>
#include <PsmrtsRayTrace.hpp>

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

        virtual std::string tracer_model_type() const {
          return ( m_model_pst.tracer_model_type() );
        }

        virtual std::string tracer_model_name() const  {
          return ( m_model_pst.tracer_model_name() );
        }
        virtual std::string shape_tracer_id()   const  {
          return ( m_model_pst.shape_tracer_id() );
        }
        virtual std::string shapefile()         const  {
          return ( m_model_pst.shapefile() );
        }

        virtual size_t plate_count()  const  {
          return ( m_model_pst.plate_count() );
        }

        virtual size_t vertex_count() const  {
          return ( m_model_pst.vertex_count() );
        }

        virtual double maximum_radius() const {
          return ( m_model_pst.maximum_radius() );
        }

        virtual size_t use_count() const {
          return ( m_model_pst.use_count() );
        }        

        virtual bool ray_trace( const Eigen::Vector3d &observer,
                                const Eigen::Vector3d &lookdir,
                                PsmrtsRayTrace &ray ) const  {
          return ( m_model_pst.ray_trace(  observer, lookdir, ray ) );
        }

        virtual bool get_facet( const PsmrtsRayTrace &ray,
                                PsmrtsRayTrace::FacetDatum &facet ) const {
          return ( m_model_pst.get_facet( ray, facet ) );
        }

        virtual PsmrtsTracerModel *clone() const  {
          return ( new PsmrtsShapeTracerAdapter<MODEL>( m_model_pst.clone() ) );
        }

        virtual PsmrtsTracerModel *ellipsoid() const  {
          return ( nullptr );
          // return ( new PsmrtsShapeTracerAdapter<MODEL>( m_model_pst.ellipsoid() );
        }

        inline const MODEL &model() const {
          return ( m_model_pst );
        }

        private:
          MODEL   m_model_pst;
      
    };

} // namespace psmrts

#endif
