#pragma once

#include <deque>
#include <string>
#include <memory>
#include <type_traits>
#include <exception>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsParameters.hpp>
#include <PsmrtsRayTrace.hpp>

namespace psmrts { 

  namespace traits { 

      template <class, class = void> 
        struct has_type_member : std::false_type {};

      template <class T> 
        struct has_type_member<T, std::void_t<typename T::type>> : std::true_type {};

      template <class, class = void> 
        struct has_valid_member : std::false_type {};

      template <class T> 
        struct has_valid_member<T, std::void_t<typename T::type>> : std::true_type {};

      template < typename = bool, typename = void, typename... Args> 
      struct is_process_callable : std::false_type { };

      template < typename T, typename... Args>
      struct is_process_callable< bool, T, std::void_t<decltype( std::declval<T>().process( std::declval<Args>()... ))>, Args...> : std::true_type {};

      template < typename R, typename T, typename... Args>
      inline constexpr bool has_process_method = is_process_callable<R, T, Args...>::value;

      // Used to detects functions of the following form 
      //   void Compute(int, int&) { std::cout << "Computing int\n"; }
      //   void Compute(double, double&) { std::cout << "Computing double\n"; }
      //
      // See http://coliru.stacked-crooked.com/a/dff6ff04ab058c29

      //template < typename T, typename = void>
      // struct is_process_available : std::false_type {};
      // template <typename C, typename T>
      // struct is_process_available<C, std::void_t<decltype(std::declval<C>().process( std::declval<T&>() ) ) >> : std::true_type {};

      // helper variable template
      // template< typename C, typename T> 
      // inline constexpr bool is_process_available_v = is_process_available<C,T>::value;

  } // namespace traits




  /**
   * @brief Producer request submit dispatch function
   * 
   * This template function will attempt to call a method in PRODUCER that has
   * a pattern of PRODUCER::process( REQUEST &request ). This dispatch function
   * will determine if the function can be called and invokes/runs it with the
   * given REQUEST parameter, returning the value returned from the
   * PRODUCER::process(). 
   * 
   * This function provides compile-time detection of any
   * PRODUCER::process(REQUEST&) method and will run the method if it exists.
   * This function can be used in virtually any class even if no valid process()
   * method exists. It will compile and return status where the run count is
   * incremented but REQUEST::was_invoked() will return false for every call.
   * 
   * This is designed to take avantage of the PsmrtsRequest base class features
   * that provides a tracking mechnism. It counts the number of times the request
   * has been invoked (which could serve as a unique ID), and if a process() 
   * methods was actually invoked. The follwing methods are requred to exist
   * in REQUEST, which is found in the PsmrtsRequest base class:
   * 
   * @code {.language-id}
   *   REQUEST::process_running();
   *   REQUEST::process_complete( bool );
   *   REQUEST::add_error( std::runtime_error );
   * @endcode
   *  
   * This function traps exceptions and adds them to the REQUEST object. Derived
   * classes can call REQUEST::reset() to clear status.
   * 
   * @tparam PRODUCER Any class PRODUCER that may contain a process( REQUEST&) method
   * @tparam REQUEST  The PMSRTS request to be processed by the PRODUCER 
   * @param producer  PRODUCER object that will be called with the REQUEST object, request
   * @param request   A functioniod-type state object that contains resources to receive
   *                   and/or process PSMRST requests.
   * @return true     If the function excuted successfuly
   * @return false    If the function does not exist or the process() method returned
   *                   a false condition
   */
  #if 1
  template <typename PRODUCER, typename REQUEST>
    bool submit_producer_request( PRODUCER &producer, REQUEST &request ) {
      bool retval   = false;
      try {

        // See https://en.cppreference.com/w/cpp/types/is_invocable (c++17)
        if constexpr ( psmrts::traits::has_process_method< PRODUCER, REQUEST > ) {
          request.process_running();
          request.process_complete( false );
          retval = std::invoke<bool, std::declval( producer.process ), request >;
          request.process_complete( true );
          // request.process_finished();
        }

      }
      catch ( const std::runtime_error &rte ) {
        request.add_error( rte );
      }
      catch ( const std::exception &e ) {
        request.add_error( std::runtime_error( e.what() ) );
      }
      catch ( ... ) {
        std::string mess = "Undefined exception occured in " + request.name();
        request.add_error( std::runtime_error( mess ) ); 
      }
      
      return ( retval );
    }

#endif

  /** Versatile noop process catchall template for unimplemented requests */
  class MissingProcessRequestHandler {
    public:
      MissingProcessRequestHandler() : m_name("Product" ) { }
      MissingProcessRequestHandler(const std::string &name ) : m_name ( name ) { }
      virtual ~MissingProcessRequestHandler() = default;
      
      inline const std::string name() const {
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

// #define PSMRTS_DISABLE_PROCESS_CATCHALL 1
#if !defined( PSMRTS_DISABLE_PROCESS_CATCHALL )
#define PSMRTS_PROCESS_CATCHALL( producer_name ) \
      /** Catch all unimplemnted producer_name::process( PRQ ) methods - e.g., PRQFacet not relevant to Ellipsoid format */ \
      template <typename PRQ>  \
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

      inline void add_error( const std::runtime_error &e ) {
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

      inline const std::deque<std::runtime_error> &errors() const {
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

      PsmrtsThreadSafeCounter m_tracker;
      double                  m_runtime_ms;
      std::string             m_name;
      bool                    m_success_status;
      bool                    m_is_present;
      size_t                  m_times_run;
      std::deque<std::runtime_error> m_errors;


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
      PRQRayTraceArray( const std::vector<PRQRayTrace> &traces ) : 
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

      inline void add_trace( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir ) {
        m_traces.push_back( PRQRayTrace( observer, lookdir ) );
      }

      inline void add_trace( const PRQRayTrace &trace ) {
        m_traces.push_back( trace );
      }

      inline const PRQRayTraceList &traces() const {
        return ( m_traces );
      }

      inline PRQRayTraceList &traces() {
        return ( m_traces );
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

      inline void add_trace( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir,
                             const Eigen::Vector3d &sunpos ) {
        m_traces.push_back( PRQPhotometricTrace( observer, lookdir, sunpos ) );
      }

      inline void add_trace( const PRQPhotometricTrace &trace ) {
        m_traces.push_back( trace );
      }

      inline const PRQPhotometricTraceList &traces() const {
        return ( m_traces );
      }

      inline PRQPhotometricTraceList &traces() {
        return ( m_traces );
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
