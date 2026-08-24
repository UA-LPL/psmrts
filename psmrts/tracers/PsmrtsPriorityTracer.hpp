/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsPriorityTracer_hpp
#define PsmrtsPriorityTracer_hpp

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include <Eigen/Geometry>
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/PsmrtsSharedCache.hpp>

namespace psmrts {

  class PsmrtsPriorityTracer : public PsmrtsProduct {
    public:
      using UIDType         = PsmrtsUID::UIDType;
      using TracerUIDList   = std::vector<UIDType>;
      using TracerInventory = PsmrtsSharedCache<UIDType, PsmrtsTracer>;
      using SharedTracer    = TracerInventory::SharedType;
      using TracerList      = std::vector<SharedTracer>;
      using PriorityFunc    = std::function<TracerList(const TracerList &current,
                                                       const TracerInventory &inventory)>;


      PsmrtsPriorityTracer( ) : PsmrtsProduct("prioritytracer") { init(); }
      PsmrtsPriorityTracer( const std::string &name ) : PsmrtsProduct( name ) { init(); }
      
      PsmrtsPriorityTracer( const SharedTracer &tracer,
                            const std::string &name = "" ) : 
                            PsmrtsProduct( name ) { 
        init();
        if ( !tracer ) return;
        if ( name.length() == 0 ) set_name( tracer->name() );
        this->add( tracer );
      }
      PsmrtsPriorityTracer(  const std::string &name ,
                             const TracerList &tracers ) : 
                             PsmrtsProduct( name ) { 
        init();
        for ( const auto &tracer : tracers ) {
          this->add( tracer );
        }
      }
      PsmrtsPriorityTracer(  const std::string &name ,
                             const TracerInventory &tracers ) : 
                             PsmrtsProduct( name ) { 
        m_inventory_t = tracers;
        m_uids_t      = tracers.keys();
        m_tracers     = tracers.values();
      }

      virtual ~PsmrtsPriorityTracer() = default;

      /** Returns validity state of Priority Tracer, if empty or not */
      inline bool isValid() const {
        return ( m_tracers.size() > 0 );
      }

      /** Returns size of Priority Tracer list */
      inline size_t size() const {
        return ( m_tracers.size() );
      }

      /** Adds a valid tracer to Priority Tracer list */
      inline void add( const SharedTracer &tracer ) {

        // Sanity check
        if ( !tracer ) return;

        if ( PsmrtsUID::is_valid_uid( tracer->uid() ) ) {
          m_uids_t.push_back( tracer->uid() );
          m_tracers.push_back( tracer );
          if ( !m_inventory_t.contains( tracer->uid() ) ) {
            m_inventory_t.add( tracer->uid(), tracer );
          }
        }
      }
      
      inline bool process ( PRQRayTrace &ray ) const {
        // Trace through list as ordered in the current UID set
        for ( const auto &tracer : tracers() ) {
          if ( tracer->process( ray )  ) {
            return ( ray.isValid() );
          }
        }
        return ( false );
      }

      inline bool process ( PRQRayTraceArray &tracelist ) const {
        // Trace through list as ordered in the current UID set
        size_t ngood = 0;
        for ( auto &ray : tracelist.traces() ) {
          for ( const auto &tracer : tracers() ) {
            if ( tracer->process( ray ) == true ) {
              ngood++;
              break;
            }
          }
        }
        return ( ngood > 0 );
      }     
      

     inline bool process ( PRQPhotometricTrace &ray_p ) const {
        // Trace through list as ordered in the current UID set
        for ( const auto &tracer : tracers() ) {
          if ( tracer->process( ray_p ) == true ) return ( true );
        }
        return ( false ); // Not a one intercepted
      }     

      
      inline bool process ( PRQPhotometricTraceArray &tracelist ) const {
        // Trace through list as ordered in the current UID set
        size_t ngood = 0;
        for ( auto &ray : tracelist.traces() ) {
          for ( const auto &tracer : tracers() ) {
            if ( tracer->process( ray ) == true ) {
              ngood++;
              break;
            } 
          }
        }
        return ( ngood > 0 );
      } 
      
      
      /** Report all remaining features not available  */
      PSMRTS_PROCESS_CATCHALL( "PsmrtsPriorityTracer" )


      /**
       * @brief Ray Trace method for tracers in Priority Tracer list
       * 
       * The method will return the first tracer model in the Priority list that has a hit
       * using the corresponding trace.
       * 
       * The PsmrtsRayTrace class contains the results of the ray trace and can be used in
       * subsequent operations.
       * 
       * @param ray PsmrtsRayTrace returns the results of the trace
       */
      inline bool ray_trace( PRQRayTrace &ray ) const {
        for ( const auto &tracer : m_tracers ) {
          if ( tracer->process( ray ) == true ) {
            return ( ray.isValid() );
          }
        }
        return ( false );
      }

      /** Return list of tracer uids contained in the inventory */
      inline const TracerUIDList &tracer_uids() const {
        return ( m_uids_t );
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
      inline SharedTracer get_tracer( const UIDType &uid ) const {
        if ( m_inventory_t.contains( uid ) ) {
          return( m_inventory_t.find( uid ) );
        }

        // Return an invalid shared tracer
        return ( nullptr );
      }

      /** Find the tracer of a valid ray trace result */
      inline SharedTracer get_tracer( const PsmrtsRayTrace &ray ) const {
        return ( get_tracer( ray.get_tracer_id() ) );
      }

      /** Empties the priority list not the inventory */
      inline void clear() {
        m_tracers.clear();
      }

      /**
       * @brief Prioritization functor processing method
       * 
       * Users can create/recompute the preferred priority of the tracers
       * contained in this priority tracer. Each tracer has a unique ID that is
       * contained in the local PSMRTS inventory of tracers. This method
       * provides a safe way to reestablish a tracer process with the list of
       * currently available tracers.
       * 
       * @see reverse().
       * 
       * @param processor Priority function to establish new order
       * @return size_t   Number of tracers in the result of processor()
       */
      inline size_t prioritize( PriorityFunc processor ) {
        m_tracers = processor( m_tracers, m_inventory_t );
        return ( m_tracers.size() );
      }

      /**
       * @brief Reverse the order of the tracer list
       * 
       * This method will reverse the order of the list of tracers which
       * essentially reverses the order in which the tracers are executed for
       * every ray trace.
       * 
       * This method demonstrates the use of the prioritize() functor method.
       * 
       * @return size_t Number of tracers in the resulting list
       */
      inline size_t reverse_priority() {
        auto reverse_tracers = []( const TracerList &current_order, 
                                   const TracerInventory &inventory ) -> TracerList {
          TracerList reversed;
          reversed.reserve( current_order.size() );
          std::transform( current_order.rbegin(), current_order.rend(), 
                          std::back_insert_iterator( reversed ),
                         []( const SharedTracer &t ) { return ( t ); } );
          return ( reversed );
        };
       
        return ( prioritize( reverse_tracers ) );
      }

      /** Find the maximum radius from all tracers */
      inline double maximum_radius() const {
        double max_r ( psmrts::null() );

        if ( m_tracers.size() > 0 ) {
          max_r = m_tracers[0]->maximum_radius();
          for ( size_t ith = 1 ; ith < m_tracers.size() ; ith++ ) {
            double radius_t =  m_tracers[ith]->maximum_radius();
            if ( radius_t > max_r ) max_r = radius_t;
          }
        }
        return ( max_r ); 
      }
      
      /** Find the minimum radius from all tracers */
      inline double minimum_radius() const {
        double min_r ( psmrts::null() );

        if ( m_tracers.size() > 0 ) {
          min_r = m_tracers[0]->minimum_radius();
          for ( size_t ith = 1 ; ith < m_tracers.size() ; ith++ ) {
            double radius_t = m_tracers[ith]->minimum_radius();
            if ( radius_t < min_r ) min_r = radius_t;
          }
        }
        return ( min_r );
      }

      
    private:
      TracerUIDList   m_uids_t;
      TracerList      m_tracers;
      TracerInventory m_inventory_t;

      inline void init( ) {
        m_uids_t.clear();
        m_tracers.clear();
        m_inventory_t.clear();
      }
  };

} // namespace psmrts

#endif
