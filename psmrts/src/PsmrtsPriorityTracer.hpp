#ifndef PsmrtsPriorityTracer_hpp
#define PsmrtsPriorityTracer_hpp

#include <exception>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include <Eigen/Geometry>
#include <PsmrtsUtilities.hpp>
#include <RayTraceModel.hpp>
#include <PsmrtsTracerModel.hpp>

namespace psmrts {

  class PsmrtsPriorityTracer {
    public:
      PsmrtsPriorityTracer( ) { init(); }
      virtual ~PsmrtsPriorityTracer() { }

      inline bool isValid() const {
        return ( this->size() > 0 );
      }

      inline size_t size() const {
        return ( tracers().size() );
      }

      inline void add_tracer( const PsmrtsTracerModel *tracer ) {
        m_tracers.push_back( std::make_shared<PsmrtsTracerModel>( tracer ) );
      }

      inline const PsmrtsTracerModel *ray_trace( const Eigen::Vector3d &observer,
                                                 const Eigen::Vector3d &lookdir,
                                                 RayTraceModel &ray ) const {

        for ( auto const &tracer : tracers() ) {
          if ( tracer->ray_trace( observer, lookdir, ray ) ) {
            return ( tracer.get() );
          }
        }

        return ( nullptr );
      }

      inline const PsmrtsTracerModel *find_model_by_name( const std::string &name_t ) const {
        for ( auto const &tracer : tracers() ) {
          if ( tracer->shapefile() == name_t ) {
            return ( tracer.get() );
          }
        }
        return ( nullptr );
      }

      inline const PsmrtsTracerModel *find_model_by_id( const std::string &id_t ) const {
        for ( auto const &tracer : tracers() ) {
          if ( tracer->shape_tracer_id() == id_t ) {
            return ( tracer.get() );
          }
        }
        return ( nullptr );
      }      


      inline std::vector<std::string> get_model_shapefiles( const std::string &model_type = "",
                                                            const std::string &model_name = "" ) const {
        std::vector<std::string> model_files;

        for ( auto const &tracer : tracers() ) {
          if ( !model_type.empty() && ( tracer->tracer_model_type() != model_type ) ) continue;
          if ( !model_name.empty() && ( tracer->tracer_model_name() != model_name ) ) continue;
          model_files.push_back( tracer->shapefile() );
        }

        return ( model_files );
      }

    protected:
      inline const TracerModelList &tracers() const {
        return ( m_tracers() );
      }

    private:
      typedef std::shared_ptr<PsmrtsTracerModel>   SharedTracerModel;
      typedef std::vector<SharedTracerModel>       TracerModelList;

      TracerModelList    m_tracers;

      inline void init( ) {
        m_tracers.clear();
      }
  };

} // namespace psmrts

#endif
