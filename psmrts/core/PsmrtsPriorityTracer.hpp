#ifndef PsmrtsPriorityTracer_hpp
#define PsmrtsPriorityTracer_hpp

#include <exception>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include <Eigen/Geometry>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsTracerModel.hpp>

namespace psmrts {

  class PsmrtsPriorityTracer {
    public:
      PsmrtsPriorityTracer( ) { init(); }
      
      PsmrtsPriorityTracer( PsmrtsTracerModel *tracer ) { 
        init(); 
        add_tracer( tracer );
      }

      PsmrtsPriorityTracer( const std::shared_ptr<PsmrtsTracerModel>  &tracer ) { 
        init();
        m_tracers.push_back( tracer );
      }

      virtual ~PsmrtsPriorityTracer() { }

      /** Returns validity state of Priority Tracer, if empty or not */
      inline bool isValid() const {
        return ( this->size() > 0 );
      }

      /** Returns size of Priority Tracer list */
      inline size_t size() const {
        return ( tracers().size() );
      }

      /** Adds a tracer to Priority Tracer list */
      inline void add_tracer( PsmrtsTracerModel *tracer ) {
        m_tracers.push_back( this->make_shared( tracer ) );
      }

      /** Adds a tracer to Priority Tracer list, using a shared_ptr of a tracer */
      inline void add_tracer( const std::shared_ptr<PsmrtsTracerModel>  &tracer ) {
        m_tracers.push_back(  tracer );
      }
      
      /**
       * @brief Ray Trace method for tracers in Priority Tracer list
       * 
       * This method is used to run a body-fixed ray trace from an observer point and 
       * look direction vector. The origin of the "observer" vector is the origin of the 
       * planet body and presumeably extends outward beyond the maximum radius of the surface
       * of the models. From that point, is the origin of the "lookdir" vector from which to 
       * trace for an intersection with the shape models' surfaces. 
       * 
       * The method will return the first tracer model in the Priority list that has a hit
       * using the corresponding trace.
       * 
       * The PsmrtsRayTrace class contains the results of the ray trace and can be used in
       * subsequent operations.
       * 
       * @param observer                    Location of the observer relative to the center
       *                                     of the target body
       * @param lookdir                     Look direction of the ray from the observer to 
       *                                     trace for intersections
       * @param ray                         PsmrtsRayTrace returns the results of the trace
       * @return const PsmrtsTracerModel*   Returns a pointer to the first Tracer Model to 
       *                                     register a hit with the provided trace. Otherwise,
       *                                     returns a nullptr.
       */
      inline const PsmrtsTracerModel *ray_trace( const Eigen::Vector3d &observer,
                                                 const Eigen::Vector3d &lookdir,
                                                 PsmrtsRayTrace &ray ) const {

        for ( auto const &tracer : tracers() ) {
          if ( tracer->ray_trace( observer, lookdir, ray ) ) {
            return ( tracer.get() );
          }
        }

        return ( nullptr );
      }

      /** Returns pointer to Tracer Model in list with same name as input parameter */
      inline const PsmrtsTracerModel *find_model_by_name( const std::string &name_t ) const {
        for ( auto const &tracer : tracers() ) {
          if ( tracer->shapefile() == name_t ) {
            return ( tracer.get() );
          }
        }
        return ( nullptr );
      }

      /** Returns pointer to Tracer Model in list with same shape tracer ID as input parameter */
      inline const PsmrtsTracerModel *find_model_by_id( const std::string &id_t ) const {
        for ( auto const &tracer : tracers() ) {
          if ( tracer->shape_tracer_id() == id_t ) {
            return ( tracer.get() );
          }
        }
        return ( nullptr );
      }      

      /**
       * @brief Get the shapefile names currently in Priority list
       * 
       * This method returns a vector list containing the names of all the Tracer Models
       * currently in the Priority list. The parameters allow the exclusion of desired 
       * model types or names from appearing in the output vector.
       * 
       * @param model_type                 String designation for any desired exclusion of
       *                                    specific model types.
       * @param model_name                 String designation for any desired exclusion of
       *                                    specific model names.
       * @return std::vector<std::string>  Returns a vector of strings designating the shapefile
       *                                    names of Tracer Models in the Priority list.
       */
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

      /** Empties the Priority list */
      inline void clear() {
        m_tracers.clear();
      }

      
    protected:
      typedef std::shared_ptr<PsmrtsTracerModel>  SharedTracerModel;
      typedef std::vector<SharedTracerModel>      TracerModelList;

      inline const TracerModelList &tracers() const {
        return ( m_tracers );
      }

    private:
      TracerModelList    m_tracers;

      inline void init( ) {
        m_tracers.clear();
      }

      inline SharedTracerModel make_shared( PsmrtsTracerModel *tracer ) const {
        return ( SharedTracerModel ( tracer ) );
      }
  };

} // namespace psmrts

#endif
