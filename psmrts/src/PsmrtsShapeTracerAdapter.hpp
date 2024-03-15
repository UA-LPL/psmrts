#ifndef PsmrtsShapeTracerAdapter_hpp
#define PsmrtsShapeTracerAdapter_hpp

#include <exception>
#include <string>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsTracerModel.hpp>
#include <RayTrace.hpp>

namespace psmrts {

  template<typename MODEL> 
    class PsmrtsShapeTracerAdapter : public PsmrtsTracerModel {
      public:
        PsmrtsShapeTracerAdapter( ) {  }
        
        PsmrtsShapeTracerAdapter( const MODEL &model ) {  
          m_model_pst = model;
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

        virtual size_t use_count() const {
          return ( m_model_pst.use_count() );
        }        

        virtual bool ray_trace( const Eigen::Vector3d &observer,
                                const Eigen::Vector3d &lookdir,
                                RayTrace &ray ) const  {
          return ( m_model_pst.ray_trace(  observer, lookdir, ray.datum() ) );
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
