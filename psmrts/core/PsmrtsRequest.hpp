#pragma once

#include <vector>
#include <deque>
#include <string>
#include <exception>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>

namespace psmrts { 

  /**
   * @brief A specialized NOOP class process handler
   * 
   * This class is designed to be used as "no" product proxy. This is useful
   * to use as a default variant type for any product variant use as the
   * first element listed in the variant declaration. 
   * 
   * @see PsmrtsTracer.hpp for an example of this technique.
   */
  class MissingProcessRequestHandler {
    public:
      MissingProcessRequestHandler() : m_name("Product" ) { }
      MissingProcessRequestHandler(const std::string &name ) : m_name ( name ) { }
      virtual ~MissingProcessRequestHandler() = default;
      
      inline const std::string &name() const {
        return ( m_name );
      }

      template <class PRQ>
        bool process( PRQ &request ) const {
          request.reset();
          request.process_running();
          request.add_error( std::runtime_error( this->name() + "::process(" + request.name() + ") is not implemented/available!" ) );
          request.set_process_presence( false );
          request.process_complete( false );
          return ( false );
        }

    private:
      std::string m_name;
  };

  /**
   * @brief MACRO to catch all unimplemented calls to process( PRQ )
   * 
   * This macro is designed to be added at the bottom of any class that may
   * be subjected to PSMRTS process( PRQ ) calls. It "captures" all
   * unimplemented process( PRQ ) calls in any class this is included
   * in.
   * 
   * Notice it implements a NOOP strategy that will indicate the requested
   * process does not exist in this class. In this case, the process
   * method template in MissingProcessRequestHandler is called that will
   * configure the NOOP process request call.
   * 
   * Classes that do not have a PRQ feature can ignore implementing an
   * empty process() method. Product classes now only need to provide
   * the implementation of process( PRQ ) visitors that can provide the
   * requested data or operation.
   * 
   * The following code should be added at the end of the public section of
   * the class:
   * 
   * @code 
   *  // Report all remaining features not available - e.g., 
   *  // PRQFacet is not relevant to an Ellipsoid tracer or 
   *  // other non-mesh tracers.
      PSMRTS_PROCESS_CATCHALL( "EllipsoidTracer" )
   * @endcode
   * 
   * Note that there is no ; at the end of that statement. A compiler error
   * will occur if one is added.
   * 
   * Also, during compilation, if the macro PSMRTS_DISABLE_PROCESS_CATCHALL
   * is defined, this statement is not compiled. This will reveal, through
   * ensuing compiler errors, all process( PRQ ) calls issued against
   * your class object that are not physically iimplemented. It may be
   * helpful to get a summary of all potential data return PRQ options which
   * the product class developers may find useful. To turn this on for your
   * class only you could use the following code:
   * 
   * @code
   * #define PSMRTS_DISABLE_PROCESS_CATCHALL 1
   *   PSMRTS_PROCESS_CATCHALL( "EllipsoidTracer" )
   * #undef PSMRTS_DISABLE_PROCESS_CATCHALL
   * @endcode
   * 
   * This technique provides a framework which allows you to add your own
   * product classes and unique requests without affecting/disrupting any
   * other products. Nor are you required to write all process( PRQ )
   * methods that may exist.
   * 
   * Note this technique is applied/resolved at compile time due to the
   * use of templates.
   * 
   */
#if !defined( PSMRTS_DISABLE_PROCESS_CATCHALL )
#define PSMRTS_PROCESS_CATCHALL( producer_name ) \
      /* Catch all unimplemnted producer_name::process( PRQ ) methods - e.g., */ \
      /* PRQFacet not relevant to Ellipsoid format */ \
      template <typename PRQ> \
       bool process( PRQ &prq_t ) const { \
         return ( MissingProcessRequestHandler( producer_name ).process( prq_t ) ); \
      } 
#else 
#define PSMRTS_PROCESS_CATCHALL( producer_name ) 
#endif

  /**
   * @brief Base class of all PSMRTS requests
   * 
   * This class provides the fundamental base class of all PSMRTS requests.
   * It tracks counts of execution attempts, if a method was actually executed
   * and returns any exceptions that may have been thrown during processing.
   * 
   * This is to be used in conjuction with the psmrts::submit_producer_request()
   * function that is to be used to dispatch 
   * 
   */
  class PsmrtsRequest {
    public:
      PsmrtsRequest( ) { this->init( ); }

      PsmrtsRequest( const std::string &name ) { 
        this->init( name );
      }

      virtual ~PsmrtsRequest() { }

      inline const std::string &name () const {
        return ( m_name );
      }

      inline void process_running() {
        m_tracker.hitme();
        m_times_run++;
        set_process_presence( true   );
        return;
      }

      inline void process_complete( const bool status = true ) {
        m_runtime_ms = m_tracker.runtime_ms();
        m_success_status = status;
        return;
      }

      inline void add_error( const std::exception &e ) const {
      // Monitor the cache size of the error queue
        if ( m_errors.size() >= MaxQueuedErrors ) {
          (void) m_errors.pop_front();
        }

        m_errors.push_back( e );
        return;
      }

      inline size_t run_count() const {
        return ( m_times_run );
      }

      /** Returns runtime for the last process */
      inline double runtime_ms() const {
        return ( m_runtime_ms );
      }

      /** Return the status of the last run */
      inline bool process_status( ) const {
        return ( m_success_status );
      }

      /** Was the process method invoked on the previous run */
      inline bool was_invoked( ) const {
        return ( m_is_present );
      }

      inline size_t error_count() const {
        return ( m_errors.size() );
      }

      inline const std::deque<std::exception> &errors() const {
        return ( m_errors );
      }

      inline std::string errors_to_string() const {
        std::string mess("");
        if ( this->error_count() > 0 ) {
          // mess = "*** " + this->name() + " has encountered errors!\n";
          for ( const auto &e : this->errors() ) {
            mess += std::string( e.what() ) + "\n"; 
          }
        }
        return ( mess );
      }

      inline void throw_errors() const {
        if ( this->error_count() > 0 ) {
          throw std::runtime_error( this->errors_to_string() );
        }
        return;
      }

      /** Return a reference to the request tracker */
      inline const PsmrtsThreadSafeCounter &tracker() const {
        return ( m_tracker );
      }

      inline void clear_errors() {
        m_errors.clear();
        return;
      }

      inline void set_process_presence( const bool present = true   ) {
        m_is_present = present;
        return;
      }

      inline void reset() {
        m_success_status = false;
        m_is_present     = false;
        m_times_run      = 0;
        m_errors.clear();
        m_tracker.reset_timer();
        m_runtime_ms = 0.0;
        return;
      }

      inline size_t max_error_cache_size() const {
        return ( MaxQueuedErrors );
      }

    protected:
      inline static const size_t MaxQueuedErrors = 20;  // Limit cached error size

      PsmrtsThreadSafeCounter    m_tracker;
      double                     m_runtime_ms;
      std::string                m_name;
      bool                       m_success_status;
      bool                       m_is_present;
      size_t                     m_times_run;
      mutable std::deque<std::exception> m_errors;


    private:
      inline void init( const std::string &name = "PsmrtsRequest" ) {
        m_name = name;
        this->reset( );
        return;
      }
  };




  class PRQVersion : public PsmrtsRequest {
    public:
      PRQVersion( ) : PsmrtsRequest( "PsmrtsVersion" ), 
                         m_version(  ) {  }
      virtual ~PRQVersion() { }

      inline const std::string &system_version ( ) const {
        return ( psmrts::psmrts_version() );
      }

      inline const std::string &version() const {
        return ( m_version );
      }

      inline bool set_version ( const std::string &v ) {
        m_version = v;
        return ( true );
      }

    public:
      /** This scope may be changed if necessary */
      std::string m_version;
  };

  class PRQFeatures : public PsmrtsRequest {
    public:
      PRQFeatures( ) : PsmrtsRequest( "PRQFeatures" ), 
                      m_features(  ) {  }
      virtual ~PRQFeatures() { }

      inline void add_feature ( const psmrts_json &feature ) {
        m_features += feature;
        return;
      }

      inline std::string to_string( ) const {
        return ( std::string( m_features.dump() ) );
      }

      inline const psmrts_json &config() const {
        return ( m_features );
      }

    public:
      /** This scope may be changed if necessary */
      psmrts_json  m_features;
  };



  class PRQRayTrace : public PsmrtsRequest {
    public:
    /** default constructable */
      PRQRayTrace() : PsmrtsRequest( "PRQRayTrace" ),
                                 m_sc_to_surface( ) { }
      PRQRayTrace( const Eigen::Vector3d &observer, 
                   const Eigen::Vector3d &lookdir ) : 
                   PsmrtsRequest( "PRQRayTrace" ),
                   m_sc_to_surface( observer, lookdir ) { }                                 
      PRQRayTrace( const PsmrtsRayTrace &observer_and_lookdir ) : 
                   PsmrtsRequest( "PRQRayTrace" ),
                   m_sc_to_surface( observer_and_lookdir ) { }
      virtual ~PRQRayTrace() { }
 
      using PsmrtsRequest::name;
      using PsmrtsRequest::run_count;
      using PsmrtsRequest::was_invoked;
      using PsmrtsRequest::error_count;
      using PsmrtsRequest::errors;

      inline bool isValid() const {
        return ( m_sc_to_surface.hasHit() );
      }

      inline double incidence( const PsmrtsRayTrace &other ) const {
        return ( m_sc_to_surface.incidence( other ) );
      }

      inline double emission( ) const {
        return ( m_sc_to_surface.emission( ) );
      }

      inline double phase( const PsmrtsRayTrace &other ) const {
        return ( m_sc_to_surface.phase( other ) );
      }

      inline const PsmrtsRayTrace &trace() const {
        return ( m_sc_to_surface );
      }

      inline PsmrtsRayTrace &trace() {
        return ( m_sc_to_surface );
      }

    public:
      /** This scope may be changed if necessary */
     PsmrtsRayTrace m_sc_to_surface;
     // PsmrtsShapeTracer *m_tracer;

  };

class PRQRayTraceArray : public PsmrtsRequest {
    public:
      using PRQRayTraceList = std::vector<PRQRayTrace>;

    /** default constructable */
      PRQRayTraceArray() : PsmrtsRequest( "PRQRayTraceArray" ),
                           m_traces( ) { }
      PRQRayTraceArray( const PRQRayTraceList &traces ) : 
                        PsmrtsRequest( "PRQRayTraceArray" ),
                        m_traces( traces ) { }                                                     
      virtual ~PRQRayTraceArray() { }
 
      using PsmrtsRequest::name;
      using PsmrtsRequest::run_count;
      using PsmrtsRequest::was_invoked;
      using PsmrtsRequest::error_count;
      using PsmrtsRequest::errors;

      inline size_t size() const {
        return ( m_traces.size() );
      }

      inline size_t add_trace( const Eigen::Vector3d &observer,
                             const Eigen::Vector3d &lookdir ) {
        m_traces.push_back( PRQRayTrace( observer, lookdir ) );
        return ( m_traces.size() - 1 ); // returns index of added trace
      }

      inline size_t add_trace( const PRQRayTrace &trace ) {
        m_traces.push_back( trace );
        return ( m_traces.size() - 1 ); // returns index of added trace
      }

      inline void clear() {
        m_traces.clear();    // calls std::vector::clear()
        reset();             // this resets tracker/timer
      }

      inline const PRQRayTraceList &traces() const {
        return ( m_traces );
      }

      inline PRQRayTraceList &traces() {
        return ( m_traces );
      }

      inline const PRQRayTrace &get_trace( size_t index ) const {
        if ( index >= this->size() ) {
          throw std::out_of_range( "PRQRayTraceArray::get_trace() - Index out of bounds for array access." );
        }
        return ( traces().at( index ) );
      }

    public:
      /** This scope may be changed if necessary */
     PRQRayTraceList m_traces;
     // PsmrtsShapeTracer *m_tracer;
  };

  class PRQPhotometricTrace : public PsmrtsRequest {
    public:
    /** default constructable */
      PRQPhotometricTrace() : PsmrtsRequest( "PRQPhotometricTrace" ),
                              m_sc_to_surface( ),
                              m_sun_to_surface( ) { }
      PRQPhotometricTrace( const PsmrtsRayTrace &observer_and_lookdir,
                           const Eigen::Vector3d &sunpos ) : 
                           PsmrtsRequest( "PRQPhotometricTrace" ),
                           m_sc_to_surface( observer_and_lookdir ),
                           m_sun_to_surface( sunpos, { 0, 0, 0 } ) { }
      PRQPhotometricTrace( const Eigen::Vector3d &observer, 
                           const Eigen::Vector3d &lookdir,
                           const Eigen::Vector3d &sunpos ) : 
                           PsmrtsRequest( "PRQPhotometricTrace" ),
                           m_sc_to_surface( observer, lookdir ),
                           m_sun_to_surface( sunpos, { 0, 0, 0 } ) { }                           
      virtual ~PRQPhotometricTrace() { }

      using PsmrtsRequest::name;
      using PsmrtsRequest::run_count;
      using PsmrtsRequest::was_invoked;
      using PsmrtsRequest::error_count;
      using PsmrtsRequest::errors;

      inline bool isValid() const {
        return ( m_sc_to_surface.isValid() && m_sun_to_surface.isValid() );
      }

      inline double incidence( ) const {
        return ( m_sc_to_surface.trace().incidence( m_sun_to_surface.trace() ) );
      }

      inline double emission( ) const {
        return ( m_sc_to_surface.trace().emission( ) );
      }

      inline double phase( ) const {
        return ( m_sc_to_surface.trace().phase( m_sun_to_surface.trace() ) );
      }

      inline const PsmrtsRayTrace &observer_trace() const {
        return ( m_sc_to_surface.trace() );
      }

      inline const PsmrtsRayTrace &sun_trace() const {
        return ( m_sun_to_surface.trace() );
      }      

      inline PsmrtsRayTrace &observer_trace() {
        return ( m_sc_to_surface.trace() );
      }

      inline PsmrtsRayTrace &sun_trace() {
        return ( m_sun_to_surface.trace() );
      } 

      inline bool compute_sun_lookdir( ) {
        if ( this->observer_trace().hasHit() ) {
          Eigen::Vector3d lookdir_s = this->observer_trace().xyz() - this->sun_trace().observer();
          this->sun_trace().datum().m_lookdir = lookdir_s;
          return ( !psmrts::isnull( lookdir_s[0]) );
        }
        return ( false );
      }

      inline const PRQRayTrace &observer() const {
        return ( m_sc_to_surface );
      }

      inline const PRQRayTrace &sunpos() const {
        return ( m_sun_to_surface );
      }


      inline PRQRayTrace &observer()  {
        return ( m_sc_to_surface );
      }

      inline PRQRayTrace &sunpos()  {
        return ( m_sun_to_surface );
      }


    public:
      /** This scope may be changed if necessary */
     PRQRayTrace m_sc_to_surface;
     PRQRayTrace m_sun_to_surface;
     // PsmrtsShapeTracer *m_tracer;

  };

class PRQPhotometricTraceArray : public PsmrtsRequest {
    public:
      using PRQPhotometricTraceList = std::vector<PRQPhotometricTrace>;

    /** default constructable */
      PRQPhotometricTraceArray() : PsmrtsRequest( "PRQPhotometricTraceArray" ),
                           m_traces( ) { }
      PRQPhotometricTraceArray( const PRQPhotometricTraceList &traces ) : 
                        PsmrtsRequest( "PRQPhotometricTraceArray" ),
                        m_traces( traces ) { }                                 
      virtual ~PRQPhotometricTraceArray() { }
 
      using PsmrtsRequest::name;
      using PsmrtsRequest::run_count;
      using PsmrtsRequest::was_invoked;
      using PsmrtsRequest::error_count;
      using PsmrtsRequest::errors;

      inline size_t size() const {
        return ( m_traces.size() );
      }

      inline size_t add_trace( const Eigen::Vector3d &observer,
                               const Eigen::Vector3d &lookdir,
                               const Eigen::Vector3d &sunpos ) {
        m_traces.push_back( PRQPhotometricTrace( observer, lookdir, sunpos ) );
        return ( m_traces.size() - 1 ); // returns index of added trace
      }

      inline size_t add_trace( const PRQPhotometricTrace &trace ) {
        m_traces.push_back( trace );
        return ( m_traces.size() - 1 ); // returns index of added trace
      }

      inline void clear() {
        m_traces.clear();    // calls std::vector::clear()
        reset();             // this resets tracker/timer
      }

      inline const PRQPhotometricTraceList &traces() const {
        return ( m_traces );
      }

      inline PRQPhotometricTraceList &traces() {
        return ( m_traces );
      }

      inline const PRQPhotometricTrace &get_trace( size_t index ) const {
        if ( index >= this->size() ) {
          throw std::out_of_range( "PRQPhotometricTraceArray::get_trace() - Index out of bounds for array access." );
        }
        return ( traces().at( index ) );
      }

    public:
      /** This scope may be changed if necessary */
     PRQPhotometricTraceList m_traces;
     // PsmrtsShapeTracer *m_tracer;
  };

  class PRQFacet : public PsmrtsRequest {
    public:
      typedef PsmrtsRayTrace::FacetDatum  FacetDatum;

    /** default constructable */
      PRQFacet() : PsmrtsRequest( "PRQFacet" ),
                                 m_trace( ) { }
      PRQFacet( const PRQRayTrace &ray ) : 
                   PsmrtsRequest( "PRQFacet" ),
                   m_trace( ray ) { }                                 
      PRQFacet( const PsmrtsRayTrace &observer_and_lookdir ) : 
                   PsmrtsRequest( "PRQFacet" ),
                   m_trace( observer_and_lookdir ) { }
      virtual ~PRQFacet() { }
 
      using PsmrtsRequest::name;
      using PsmrtsRequest::run_count;
      using PsmrtsRequest::was_invoked;
      using PsmrtsRequest::error_count;
      using PsmrtsRequest::errors;

      inline bool isValid() const {
        return ( m_trace.isValid() );
      }

      inline const PsmrtsRayTrace &trace() const {
        return ( m_trace.trace() );
      }

      inline const PRQRayTrace &prq_trace() const {
        return ( m_trace);
      }

      inline const FacetDatum &facet() const {
        return ( m_facet );
      }

      inline FacetDatum &facet() {
        return ( m_facet );
      }      

    public:
      /** This scope may be changed if necessary */
     PRQRayTrace m_trace;
     FacetDatum  m_facet;
     // PsmrtsShapeTracer *m_tracer;

  };



} // namespace psmrts
