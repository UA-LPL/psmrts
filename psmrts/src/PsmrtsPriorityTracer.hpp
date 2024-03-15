#ifndef PsmrtsPriorityTracer_hpp
#define PsmrtsPriorityTracer_hpp

#include <exception>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include <Eigen/Geometry>
#include <PsmrtsUtilities.hpp>
#include <RayTrace.hpp>
#include <PsmrtsTracerModel.hpp>

namespace psmrts {

  class PsmrtsPriorityTracer {
    public:
      PsmrtsPriorityTracer( ) { init(); }
      PsmrtsPriorityTracer( const PsmrtsTracerModel *tracer ) { 
        init(); 
        add_tracer( tracer );
      }
      PsmrtsPriorityTracer( const std::shared_ptr<PsmrtsTracerModel>  &tracer ) { 
        init();
        m_tracers.push_back( tracer );
      }

      virtual ~PsmrtsPriorityTracer() { }

      inline bool isValid() const {
        return ( this->size() > 0 );
      }

      inline size_t size() const {
        return ( tracers().size() );
      }

      inline void add_tracer( const PsmrtsTracerModel *tracer ) {
        m_tracers.push_back( this->make_shared( tracer ) );
      }

      inline const PsmrtsTracerModel *ray_trace( const Eigen::Vector3d &observer,
                                                 const Eigen::Vector3d &lookdir,
                                                 RayTrace &ray ) const {

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


      inline std::vector<std::string> get_shapefile_names( const std::string &model_type = "",
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
      typedef std::shared_ptr<const PsmrtsTracerModel>   SharedTracerModel;
      typedef std::vector<SharedTracerModel>       TracerModelList;

      inline const TracerModelList &tracers() const {
        return ( m_tracers );
      }

    private:
      TracerModelList    m_tracers;

      inline void init( ) {
        m_tracers.clear();
      }

      inline SharedTracerModel make_shared( const PsmrtsTracerModel *tracer ) const {
        return ( SharedTracerModel ( tracer ) );
      }
  };

} // namespace psmrts

#endif
