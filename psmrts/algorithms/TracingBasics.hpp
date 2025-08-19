#pragma once
#include <string>

#include <psmrts/core/PsmrtsRequest.hpp>

namespace psmrts::algorithms  {



  /**
   * @brief Reusable algorithms for tracing operations
   * 
   * The functions in this set of algorithms require several methods for the
   * TRACER arguments. The TRACER types must have a ray trace function of
   * the following calling parameters:
   * 
   * TRACER::ray_trace( PSMRTSRayTrace &ray );
   * TRACER::get_facet( PSMRTSRayTrace &ray, PSMRTSRayTrace::FacetDatum &datum );
   * 
   * Essentually, if there is any questions as to whether a tracer has the
   * process() functionality, users should call the general process( PRQ )
   * of the form rather than these functions or an error will ensue.
   *
   * @author 2025-08-19 Kris J Becker
   */


  /** Concatenate two strings into one convenience fuction */
  inline std::string concate_s(const std::string &s1, const std::string &s2 ) {
    return ( s1 + s2 );
  }

  /* @brief Tracer Ray Trace Processor
  * 
  * This function accepts a PRQRayTrace, which is instantiated with 
  * values necessary for a Ray Trace (observer, look direction), 
  * runs the trace, and saves the result back into the PRQRayTrace
  * object - accessible through its' trace() function. It returns
  * true if the trace resulted in an intercept.
  * 
  * @param trace   PRQRayTrace provides observer, lookdir values,
  *                  and stores trace result
  * @return true   If trace intercepts the shape
  * @return false  If no ray trace intercept was found
  */
  template <typename TRACER>
    bool process_basic_trace( const TRACER &tracer, PRQRayTrace &trace ) {
      try { 
        trace.trace().validate_lookdir();
        return ( tracer.ray_trace( trace.trace() ) );
      }
      catch ( const std::exception &e ) {
        trace.add_error( std::runtime_error( concate_s("*** process_basic_trace() error: ", e.what() ) ) );
        return ( false );
      }

      return ( false );
  }

  /**
   * @brief Tracer  Ray Trace Array Processor
   * 
   * This method accepts a PRQRayTraceArray, which represents
   * multiple PRQRayTraces in an array object, runs each trace,
   * and stores the results back into their associated PRQRayTraces
   * in the array.
   * 
   * If at least one of the traces results in an intercept, the
   * method returns true.
   * 
   * @param tracelist PRQRayTraceArray provides individual PRQRayTraces,
   *                    each holding both values and post-trace results.
   * @return true     If at least one trace intercepts the shape
   * @return false    If no trace intercepts were found
   */
  template <typename TRACER>
    bool process_basic_trace_array ( const TRACER &tracer, PRQRayTraceArray &tracelist ) {
      size_t n_good = 0;
      for ( auto &trace : tracelist.traces() ) {
        try { 
          if ( process_basic_trace(tracer, trace ) ) {
            n_good++;
          }
        }
        catch ( const std::exception &e ) {
          trace.add_error( std::runtime_error( concate_s("*** process_basic_trace_array() error: ", e.what() ) ) );
        }
      }
      
      return ( n_good > 0 );
    }

  /**
   * @brief Tracer Facet Processor
   * 
   * This method accepts a PRQFacet, usually instantiated with a ray
   * trace, and processes it - storing the facet associated with the 
   * trace's intercept back into the PRQFacet. The resulting facet
   * can be accessed using the PRQFacet's facet() function.
   * 
   * It returns true if the process results in a valid facet.
   * 
   * @param facet   PRQFacet provides desired ray trace, and stores
   *                  resulting facet data
   * @return true   If process results in valid facet intercept
   * @return false  If process fails to find facet/intercept
   */
  template <typename TRACER>
    bool process_basic_facet( const TRACER &tracer, PRQFacet &facet ) {
      try { 
        return ( tracer.get_facet( facet.trace(), facet.facet() ) );
      }
      catch ( const std::exception &e ) {
        facet.add_error( std::runtime_error( concate_s("*** process_basic_facet() error: ", e.what() ) ) );
      }      
      return ( false );
    }

  /**
   * @brief Tracer Photometric Trace Processor
   * 
   * This method accepts a PRQPhotometricTrace, which is instantiated with 
   * values necessary for a Photometric Trace (observer, look direction,
   * sun position), runs the traces relative to both observer/look direction
   * and the sun's position, and saves the result back into the PRQPhotometricTrace
   * object. It returns true if the trace resulted in both intercepting.
   * 
   * @param trace   PRQRayTrace provides observer, lookdir values,
   *                  and stores trace result
   * @return true   If observer/look direction and sun traces intercepts
   *                  the shape
   * @return false  If either does not intercept the shape
   */
  template <typename TRACER>
    bool process_basic_photometric_trace( const TRACER &tracer, PRQPhotometricTrace &trace_p ) {
      try { 
        if ( process_basic_trace( tracer, trace_p.observer() ) ) {
          if ( trace_p.compute_sun_lookdir() ) {
            return ( process_basic_trace( tracer, trace_p.sunpos() ) );
          }
        }
      }
      catch ( const std::exception &e ) {
        trace_p.add_error( std::runtime_error( concate_s("*** process_basic_photometric_trace() error: ", e.what() ) ) );
      }  
      return ( false );
    }

  /**
   * @brief Bullet Photometric Trace Array Processor
   * 
   * This method accepts a PRQPhotomericTraceArray, which represents
   * multiple PRQPhotometricTraces in an array object, runs each trace,
   * and stores the results back into their associated PRQPhotometricTraces
   * in the array.
   * 
   * If at least one of the traces results in a photometrically appropriate 
   * intercept, the method returns true.
   * 
   * @param tracelist PRQPhotometricTraceArray provides individual PRQPhotometricTraces,
   *                    each holding both values and post-trace results.
   * @return true     If at least one of the traces intercepts the shape
   * @return false    If no appropriate trace intercepts were found
   */
  template <typename TRACER>
    bool process_basic_photometric_trace_array ( const TRACER &tracer, PRQPhotometricTraceArray &tracelist ) {
      size_t n_good = 0;
      for ( auto &trace : tracelist.traces() ) {
        try { 
          if ( process_basic_photometric_trace( tracer, trace ) ) {
          n_good++;
        }
        }
        catch ( const std::exception &e ) {
          trace.add_error( std::runtime_error( concate_s("*** process_basic_photometric_trace_array() error: ", e.what() ) ) );
        }
      }
      
      return ( n_good > 0 );
    }

} // namespace psmrts
