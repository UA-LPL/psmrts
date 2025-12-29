#ifndef PsmrtsPriorityTracer_hpp
#define PsmrtsPriorityTracer_hpp

#include <string>
#include <vector>

#include <Eigen/Geometry>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/ProductInventory.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>

namespace psmrts {

  class PsmrtsPriorityTracer : public PsmrtsProduct {
    public:
      using UIDType         = PsmrtsUID::UIDType;
      using TracerList      = std::vector<UIDType>;
      using TracerInventory = ProductInventory<UIDType, PsmrtsTracer>;

      PsmrtsPriorityTracer( ) : PsmrtsProduct("prioritytracer") { init(); }
      
      PsmrtsPriorityTracer( const PsmrtsTracer &tracer ) : PsmrtsProduct("prioritytracer") { 
        init();
        this->add_tracer( tracer );
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
      inline void add_tracer( const PsmrtsTracer &tracer ) {
        m_tracers.push_back( tracer.uid() );
        if ( !m_inventory_t.contains( tracer.uid() ) ) {
          m_inventory_t.add_product( tracer );
        }
      }
      
      inline bool process ( PRQRayTrace &ray ) const {
        // Trace through list as ordered in the current UID set
        for ( const auto &uid : tracers() ) {
          const auto &tracer = m_inventory_t.find( uid );
          if ( tracer.process( ray )  ) {
            return ( ray.isValid() );
          }
        }
        return ( false );
      }

      inline bool process ( PRQRayTraceArray &tracelist ) const {
        // Trace through list as ordered in the current UID set
        size_t ngood = 0;
        for ( auto &ray : tracelist.traces() ) {
          for ( const auto &uid : tracers() ) {
            const auto &tracer = m_inventory_t.find( uid );
            if ( tracer.process( ray ) == true ) ngood++;
          }
        }
        return ( ngood > 0 );
      }     
      


     inline bool process ( PRQPhotometricTrace &ray_p ) const {
        // Trace through list as ordered in the current UID set
        for ( const auto &uid : tracers() ) {
          const auto &tracer = m_inventory_t.find( uid );
          if ( tracer.process( ray_p ) == true ) return ( true );
        }
        return ( false ); // Not a one intercepted
      }     

      
      inline bool process ( PRQPhotometricTraceArray &tracelist ) const {
        // Trace through list as ordered in the current UID set
        size_t ngood = 0;
        for ( auto &ray : tracelist.traces() ) {
          for ( const auto &uid : tracers() ) {
            const auto &tracer = m_inventory_t.find( uid );
            if ( tracer.process( ray ) == true ) ngood++;
          }
        }
        return ( ngood > 0 );
      } 
      
      
      /** Report all remaining features not available  */
      PSMRTS_PROCESS_CATCHALL( "PsmrtsPriorityTracer" )


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
       */
      inline bool ray_trace( const PsmrtsTracer &tracer, PRQRayTrace &ray ) const {
        return ( tracer.process( ray ) );
      }

      /** Run a trace on the priority list */
      inline bool ray_trace( PRQRayTrace &ray ) const {
        for ( const auto &uid : tracers() ) {
          const auto &tracer = m_inventory_t.find( uid );
          if ( this->ray_trace( tracer, ray ) == true ) {
            return ( ray.isValid() );
          }
        }
        return ( ray.isValid() );
      }

      /** Return list of tracers in this object */
      inline const TracerList &tracers() const {
        return ( m_tracers );
      }

      /** Return list of tracers in this object */
      inline const TracerInventory &inventory() const {
        return ( m_inventory_t );
      }

      /** Find and return pointer to tracer in list, otherwise an invalid
       * tracer is returned */
      inline PsmrtsTracer get_tracer( const UIDType &uid ) const {
        if ( m_inventory_t.contains( uid ) ) {
          return( m_inventory_t.find( uid ) );
        }

        // Return an invalid tracer
        return ( PsmrtsTracer( "invalid" ) );
      }

      /** Empties the priority list not the inventory */
      inline void clear() {
        m_tracers.clear();
      }
      
    private:
      TracerList      m_tracers;
      TracerInventory m_inventory_t;

      inline void init( ) {
        m_tracers.clear();
        m_inventory_t.clear();
      }
  };

} // namespace psmrts

#endif
