#ifndef PsmrtsTracerSystem_h
#define PsmrtsTracerSystem_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <vector>
#include <string>
#include <mutex>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/tracers/PsmrtsPriorityTracer.hpp>
#include <psmrts/core/PsmrtsInvoice.hpp>


namespace psmrts {

  /**
   * @brief This class provides production and maintenance of a PSMRTS tracer system
   * 
   * The objective of this class is to simplify the creation and management of a
   * single body shape tracer that may consist of more than one shape and a
   * reference ellipsoid. This addresses situations where more than one shape
   * model many be needed to calculate cartographic gemetric data from
   * spacecraft instruemts to surface with a high degree of accuracy. 
   * 
   * Many cartograhic calculations require the determination of surface
   * intercept coordinates from an instrument element. This is particularly
   * common on CRT imaging devices where devices with pixel arrays compute a
   * vector from a pixel element presumably toward a surface body of interest
   * with the intent to compute a highly accurate intersection point on the
   * traget body. Once this point is determined, this enables the
   * derivation/computation of surface properties for mapping and analysis.
   * 
   * This class provides the means to assemble complex system of shape models
   * that are ray traced to compute these surface intercepts. The ability to
   * achieve high precision geometric data is use of sufficiently high
   * resolution and accurace surface models that have been previosly derived
   * from independent sources of surface observations. Many of the best
   * shape model products involve sets of indivual high resolution, high
   * precision models store in various triangluar based mesh file formats. This
   * presents serious challenges to utlize such data sets for efficient
   * cartopgraphic calculations. This capability is precisely what PSRMTS
   * aspires to provide developers that desire to utlize in an efficient and
   * organized method.
   * 
   * This class is designed to model a ray tracing system for a single target
   * body that may be comprised of one or more shape models and a reference
   * ellipsoid. This situation is common in the ISIS system and applies to many
   * other cartographic processing software architectures.
   * 
   * Note that all shape, ellipsoids, observer, sun and look direction vectors
   * are represented in the target body-fixed coordinate system using kilometer
   * data units. Kilometer units are required since this has been the standard
   * in planetary science largely established by the NAIF system. However,
   * PSMRTS is designed to produce user selectable precision where the default
   * is millimeter precision.
   * 
   * This class contains a reference ellipsoid, common for systems such as ISIS
   * to use for situations where shapes are regional in nature. This ellipsoid
   * is treated as a triaxial ellipsoid and is not added to the priority tracer
   * by default. It can be added easily by getting a copy of the priority tracer
   * created here and appending it to the copy. The copy can then be used to
   * make traces.
   *
   * @author 2026-03-05 Kris J. Becker
   * @history 2026-03-05 - Kris J. Becker - Original Version
   */
  class PsmrtsTracerSystem {
    public:
      using UIDType     = PsmrtsUID::UIDType;
      using ProductSet  = PsmrtsInvoice::ProductSet;      

      // Constructors
      PsmrtsTracerSystem() : m_invoice( ), 
                             m_tracer_p( ), 
                             m_ellipsoid_r( ) { 
      }
      PsmrtsTracerSystem( const std::string &name,
                          const PsmrtsTranslations &trans_t = PsmrtsTranslations::create( ) ) :
                          m_invoice( name, trans_t ), 
                          m_tracer_p( ), 
                          m_ellipsoid_r( ) {
      }
      PsmrtsTracerSystem( const std::string &name,
                          const std::vector<std::string> &shapes, 
                          const PsmrtsTranslations &trans_t = PsmrtsTranslations::create( ) ) :
                          m_invoice( name, trans_t ), 
                          m_tracer_p( ), 
                          m_ellipsoid_r( ) {
        process_shape_list( shapes );
      }      

      // Destructor
      inline ~PsmrtsTracerSystem() = default;


      inline const std::string &name() const {
        return ( m_invoice.name() );
      }

      inline size_t size() const {
        return ( m_invoice.size() );
      }

      inline size_t error_count() const {
        return ( m_invoice.error_count() );
      }

      inline size_t has_errors() const {
        return ( m_invoice.error_count() > 0 );
      }


      inline std::string error_string( ) const {
        return ( m_invoice.errors_to_string() );
      }

      inline void throw_errors( ) const {
        return ( m_invoice.throw_errors() );
      }

      inline void clear_errors( ) {
        return ( m_invoice.clear_errors() );
      }

      /**
       * @brief Add a product, tracer or shape, to the invoicing system
       * 
       * This method configures and adds a PSMRTS product to the trace invoicing
       * system. it can be either a tracer and/or a shape product. Note that
       * some tracers require shapes and others do not. This configuration
       * supports both.
       * 
       * This interface lacks additional specialization/parameterization options
       * that can be applied to add/enhance a particular product. 
       * 
       * This call parses the resulting configuration for validity and adds it
       * to the list for inclusion or us when the priority parser is
       * generated. See create_tracer(). 
       * 
       * @code {.c++}
       *   add_product( "obj_file", "bennu_20facets.obj", "bullet", "" ); // tracer
       *   add_product( "obj_file", "bennu_20facets.obj", "", "obj" );    // shape
       * @endcode
       * 
       * 
       * @param var_name    Name of the variable that is defined in the
       *                      specifications for the tracer or shape
       * @param filename    Name of the file to load for the tracer/shape
       * @param tracer_type Type of tracer to use (excludes use of shape_type)
       * @param shape_type  Type of shape to use (excludes use of tracer_type)
       * @return true       If successfully instantiates the product
       * @return false      if product creation fails
       */
      inline bool add_product( const std::string &var_name,
                               const std::string &filename,
                               const std::string &tracer_type = "",
                               const std::string &shape_type = "" ) {

        ProductConfiguration product_t( filename );
        product_t.add( ProductOption( var_name, filename) );

        if ( tracer_type.length() > 0 ) {
          product_t.add( ProductOption( "tracer", tracer_type) );
        }

        if ( shape_type.length() > 0 ) {
          product_t.add( ProductOption( "shape", shape_type ) );
        }

        return ( this->make_product( product_t ) );
      }


      /**
       * @brief Add shape product to the invoice inventory
       * 
       * This method adds the shape to the product inventory for use in config
       * processing operations. It is used in inventory searches to resolve
       * configuration matches.
       * 
       * @param shape  Shape product to add to inventory
       * @return true  If the shape was successfully added
       * @return false If the shape could not be added
       */
      inline bool add_shape( const PsmrtsShape &shape ) {
        return ( m_invoice.add_shape( shape ) );
      }

      /**
       * @brief Add tracer product to the invlice inventory
       * 
       * This method adds the tracer to the product inventory for use in config
       * processing operations. It does not add it to the priority tracer but is
       * used in inventory searches to resolve configuration matches.
       * 
       * @param tracer  Tracer product to add to inventory
       * @return true   If the tracer is valid its added
       * @return false  If its not a valid tracer
       */
      inline bool add_tracer( const PsmrtsTracer &tracer ) {
        return ( m_invoice.add_tracer( tracer ) );
      }

      /**
       * @brief Create a tracer product from a configuration
       * 
       * This method can be use to create a fully spec'ed PSMRTS product. It
       * will be processe and added to the tracer so it is expected to contain a
       * configuration that results in a valid tracer. An error is generated if
       * it does not result in a tracer.
       * 
       * @param config PSMRTS prouduct configuration
       * @return true  If product is successfully created
       * @return false If product creation fails
       */
      inline bool make_product( const ProductConfiguration &config ) {

        ProductSet product_s = m_invoice.process_product( config );
        if ( !m_invoice.processor().is_valid_product( product_s ) ) {
          std::string mess = "PsmrtsTracerSystem::make_product(" + config.name() +
                              ") errors occured during validation: \n" +
                              m_invoice.processor().product_error_string( product_s );
          m_invoice.add_error( mess );
          return ( false );
        }

        if ( !m_invoice.processor().has_valid_tracer( product_s ) ) {
          std::string mess = "PsmrtsTracerSystem::make_product(" + config.name() +
                              ") does not contain a valid tracer";
          m_invoice.add_error( mess );
          return ( false );          
        }

        // Add the product to the tracer
        m_invoice.add_product( product_s );
        return ( true );
      }


      /**
       * @brief Expand the list of a shape files/parmeters
       * 
       * @param shapes List of psmrts shape file specifications
       * @return size_t Number of shapes added to the system
       */
      inline size_t process_shape_list( const std::vector<std::string> &shapes,
                                        const std::string &name = "psmrtstracersystem" ) {

        // Lock creation of tracers for this process
        std::scoped_lock mylocker( m_mutex );

        std::vector<std::string> shape_file_list;
        for ( const std::string &file_s : shapes ) {

          std::string file_t = m_invoice.translations().translate_path( file_s );
          std::string suffix_t = psmrts_tolower( psmrts_file_extension( file_t ) );

          if ( ( "txt" == suffix_t ) || ( "lis" == suffix_t )  ) {
            read_list_file( file_t, shape_file_list );
          }
          else {
            shape_file_list.push_back( psmrts_trim( file_s ) );
          }
        }

        //***** Lambda to add expanded file name to config ***** 
        auto add_expanded_path = [&] ( const std::string &file_t, 
                                       const std::string &path_t ) -> ProductOption {
          return ( ProductOption( file_t+"_expanded", 
                                  m_invoice.translations().translate_path( path_t ) ) );          
        };

        // Now process each file in the list. Files can have a preferred tracer
        // preceedig each file delineated with "::". The form of this file is
        // "bullet::$osirisrex/kernels/dsk/bennu.bds". To select a NAIF DSK,
        // this is required, otherwise the DSK is loaded as a "naifdsk" tracer. 
        // The possibilities are "bullet" and "naifdsk". Essentially, if the
        // shape file does not have an extension of "bds", "bullet" is the only
        // tracer that supports tracing these file formats and it is
        // automatically selected. 
        size_t nerrs = 0;
        size_t n_shapes_added = 0;
        for ( const std::string &shape_t : shape_file_list ) {
                    
          // Create the product and add it to the invoice system
          ProductConfiguration tracer_c( shape_t );

          std::string name_t;
          auto parts_t = psmrts::string_tokenizer( shape_t, "::" );
          if ( parts_t.size() > 1 ) {
            std::string tracer_t = psmrts_tolower( psmrts_trim( parts_t[0] ) );
            tracer_c.add_option( ProductOption( "tracer", tracer_t ) );

            // For ellipsoids, comparablity is dependent upon exact names and
            // comparisons of radii. For radii to be comparable, the radii needs
            // to be double values rather than strings. And we must standardize
            // the name of the ellipsoid so reset the name of the config - radii
            // provides uniqueness. Also suppports spheroids and spheres.
            static std::vector<std::string> ellipsoid_types = { "ellipsoid", "spheroid", "sphere" };
            if ( psmrts_contains_string( tracer_t, ellipsoid_types ) == true ) {
              name_t = "ellipsoid";
              tracer_c = ProductConfiguration( tracer_t, tracer_c );
              ProductOption radii_s( "radii_string", string_tokenizer( parts_t[1], "," ) );
              tracer_c.add_option( ProductOption( "radii", ProductOption::DoublesExtractor( radii_s ).get_all() ) );
              tracer_c.add_metadata( ProductOption( "identifier", shape_t ) );
            }
            else {
              name_t = psmrts_trim( parts_t[1] );
              tracer_c.add_option( ProductOption( "file", psmrts_trim( parts_t[1] ) ) );
              tracer_c.add_metadata( add_expanded_path( "file", name_t ) );
            }
              
            // Check for formatting issues
            if ( parts_t.size() > 2 ) {
              std::string mess = "Invalid format for file string (" + shape_t + ")";
              m_invoice.add_error( mess );
            }
          }
          else {
            name_t = parts_t[0];
            tracer_c.add_option( ProductOption( "file", name_t ) );
            tracer_c.add_metadata( add_expanded_path( "file", name_t ) );
          }

          if ( !tracer_c.contains( "tracer" ) ) {
            tracer_c.add( ProductOption( "tracer", "bullet") );
          }

          if ( !this->make_product ( tracer_c ) ) {
            nerrs++;
          }
          else {
            n_shapes_added++;
          }
        }

        // Check for errors in tracer creation process and toss'em if they occur
        if ( nerrs > 0 ) m_invoice.throw_errors();

        // Now set the priority tracer up and check for addtional errors
        (void) create_priority_tracer( name );
        if ( nerrs > 0 ) m_invoice.throw_errors();

        return ( n_shapes_added );
      }

      /**
       * @brief Set the reference ellipsoid object with radii parameters
       * 
       * This method will create a reference ellipsoid for the target body
       * system. 
       * 
       * @param name    Name of the target body
       * @param radii   Radii used to initialize the target body
       * @return true   If the ellispoid was created successfully
       * @return false  If creation of the ellipsoid failed
       */
      inline bool set_reference_ellipsoid( const std::string &name,
                                           const std::vector<double> &radii ) {

        // Lock creation of ellipsoid
        std::scoped_lock mylocker( m_mutex );

        ProductOption tracer( "tracer", "ellipsoid" );
        ProductOption rads( "radii", radii );
        ProductConfiguration ellipsoid( "ellipsoid", { tracer, rads } );
        ellipsoid.add_metadata( ProductOption( "identifier", name ) );
        ProductSet product_s = m_invoice.process_product( ellipsoid );

        bool status = m_invoice.processor().has_valid_tracer( product_s );
        if ( true == product_s.has_tracer() ) {
          m_ellipsoid_r = product_s.tracer_p.value();
        }

        return ( status );
      }

      /**
       * @brief Set the reference ellipsoid object with a tracer
       * 
       * This method provides a PsmrtsTracer as the reference ellipsoid.
       * 
       * @param ellipsoid An ellipsoid tracer
       */
      inline bool set_reference_ellipsoid( const PsmrtsTracer &ellipsoid ) {
        m_ellipsoid_r = ellipsoid;
        return ( m_ellipsoid_r.isValid() );
      }

      /**
       * @brief Create a priority tracer object from the products in this system
       * 
       * This step is required to generate the initial prioirity trace from the
       * list of products added using add_product(). Every product in the list
       * is used to create the priority tracer in the order in which they are
       * added to the system.  
       * 
       * @param name Optional name of the priority tracer. If no name is
       *             provided, the naem provided at construction is used.
       * @return PsrmtsPriorityTracer& Reference to created/existing priority
       *                               tracer created by this method.
       */
      inline PsmrtsPriorityTracer create_priority_tracer( const std::string &name = "" ) {
        m_tracer_p =  m_invoice.make_priority_tracer( name );

        // Check to ensure there is a reference ellipsoid for the system on
        // first instance of priority tracer. Users can reset this if desired.
        if ( !m_ellipsoid_r.isValid() ) {
          EllipsoidTracer e_t( m_tracer_p.minimum_radius(), 
                               m_tracer_p.maximum_radius(),
                               m_tracer_p.name() );
          this->set_reference_ellipsoid( PsmrtsTracer( e_t ) );
        }

        return ( m_tracer_p );
      }

      /** Returns reference to the priority shape tracer  */
      inline const PsmrtsPriorityTracer &get_shape_tracer() const {
        return ( m_tracer_p );
      }

      /** Returns reference to the ellipsoid tracer model  */
      inline const PsmrtsTracer &get_ellipsoid_tracer() const {
        return ( m_ellipsoid_r );
      }

      /**
       * @brief Get the tracer from intercept object
       * 
       * This method returns the tracer identified in the ray that successfully
       * intercepted the body. This could be the ellipsoid tracer or one of the
       * tracers contained in the priority tracer.
       * 
       * An invalid tracer, represented as a nonfunctioning PsmrtsTracer, may be
       * returned if the ray trace operation did not intercept the surface.
       * Otherwise, the tracer that intercepted the surface is returned.
       * 
       * @param ray   Ray intercept object that is assumed to contain a valid
       *              surface itercept. An invalid tracer will be returned if it
       *              is was not successful.
       * @return psrts::PsmrtsTracer The tracer that intercepted the surface
       *                             from the ray contained the ray object. If
       *                             the trace did not intercept the any
       *                             surface, an invalid PsmrtsTracer is
       *                             returned. Use PsmrtsTracer::isValid() to
       *                             check status.
       *                            
       */
      inline PsmrtsTracer get_tracer_from_intercept( const PRQRayTrace &ray ) 
                                                     const {
        UIDType uid_t = ray.trace( ).get_tracer_id();
        if ( !PsmrtsUID::is_valid_uid( uid_t) ) {
          return ( PsmrtsTracer() );
        }

        if ( uid_t == m_ellipsoid_r.uid() ) {
          return ( m_ellipsoid_r );
        }

        return ( m_tracer_p.get_tracer( uid_t ) );
      }

      /**
       * @brief Executes a trace on the priority shape model
       * 
       * This method executes the trace using the ray object that contains valid
       * observer and look direction coordinates. 
       * 
       * @param ray    Object containing the observer and look direction of the
       *               requested trace of the shape.
       * @return true  If the trace intercepted the shape
       * @return false If the trace did not intercept the shape
       */      
      inline bool shape_trace( PRQRayTrace &ray ) const {      
        return ( m_tracer_p.process( ray ) );
      }

      /**
       * @brief Execute a ray trace using the shape tracer
       * 
       * This method is a generic interface to execute a trace from an observer
       * and look direction in bodyfixed coordinates. The coordinates of the
       * observer and look direction are in kilometers.
       * 
       * @param observer The body-fixed position of the observer is a vector
       *                 from the center of the body to the observer expressed
       *                 in kilometers.
       * @param lookdir  The look direction of the ray from the observer
       *                 presumuably toward the body surface in kilometers.
       * @return PRQRayTrace The result of the ray trace. The status of the
       *                     trace cna be obtained from PRQRayTrace::hasHit()
       *                     method.
       */
      inline PRQRayTrace shape_trace( const std::vector<double> &observer_km,
                                      const std::vector<double> &lookdir_km ) 
                                      const {

        PRQRayTrace ray_t( Eigen::Vector3d( observer_km.data() ), 
                           Eigen::Vector3d( lookdir_km.data() ) );
        (void) this->process( ray_t );
        return ( ray_t );
      }

      /** Returns a ray trace from observer and look direction vectors */
      inline PRQRayTrace shape_trace( const Eigen::Vector3d &observer_km,
                                      const Eigen::Vector3d &lookdir_km ) 
                                      const {

        PRQRayTrace ray_t( observer_km, lookdir_km );
        this->process( ray_t );
        return ( ray_t );
      }      


      /**
       * @brief Execute a ray trace using the ellipsoid tracer
       * 
       * This method is a generic interface to execute a trace from an observer
       * and look direction in bodyfixed coordinates. The coordinates of the
       * observer and look direction are in kilometers.
       * 
       * @param observer The body-fixed position of the observer is a vector
       *                 from the center of the body to the observer expressed
       *                 in kilometers.
       * @param lookdir  The look direction of the ray from the observer
       *                 presumuably toward the body surface in kilometers.
       * @return PRQRayTrace The result of the ray trace. The status of the
       *                     trace cna be obtained from PRQRayTrace::hasHit()
       *                     method.
       */      
      inline PRQRayTrace ellipsoid_trace( const std::vector<double> &observer_km,
                                          const std::vector<double> &lookdir_km ) 
                                          const {

        PRQRayTrace ray_t( Eigen::Vector3d( observer_km.data() ), 
                           Eigen::Vector3d( lookdir_km.data() ) );
        (void) this->ellipsoid_trace( ray_t );
        return ( ray_t );
      }


      /** Returns a ray trace from observer and look direction vectors */
      inline PRQRayTrace ellipsoid_trace( const Eigen::Vector3d &observer_km, 
                                          const Eigen::Vector3d &lookdir_km ) 
                                          const {

        PRQRayTrace ray_t( observer_km, lookdir_km );

        m_ellipsoid_r.process( ray_t );
        return ( ray_t );
      }  


      /**
       * @brief Run trace on a the shape model tracer with a ray trace object type
       * 
       * @tparam T      Type of "ray" to run on the shape tracer
       * @param ray     The ray trace object containing observer/lookdirs
       * @return true   If the trace was successful
       * @return false  If the trace was unsucessful
       */
      template <typename T>
       inline bool process( T &ray ) const {
        return ( m_tracer_p.process( ray ) ); 
      }  


      /**
       * @brief Executes a trace on the ellispoid model
       * 
       * This method executes the trace using the ray object that contains valid
       * observer and look direction coordinates. 
       * 
       * @param ray    Object containing the observer and look direction of the
       *               requested trace of the ellipsoid.
       * @return true  If the trace intercepted the ellipsoid model
       * @return false If the trace did not intercept the ellipsoid
       */
      inline bool ellipsoid_trace( PRQRayTrace &ray ) const {      
        return ( m_ellipsoid_r.process( ray ) );
      }


      /**
       * @brief Executes photometric ray trace operation on a configured object
       * 
       * The "ray_p" photometric trace object contains an observer postion a
       * look direction vector from the observer and the location of the sun,
       * all in body-fixed coordinates in units of kilometers. It executes at
       * most two traces  on the shape tracer, the first being the observer/look
       * direction and, if a successful surface intercept is made, it computes
       * the look direction vector from the sun position to the surface
       * intercept point and executes the second trace from the sun position to
       * the surface.
       * 
       * @param ray_p  A configured photometric trace object with observer
       *               position, look direction vector and the position of the
       *               sun. 
       * @return true  If both traces are successful. 
       * @return false If one or both traces fail.
       */
      inline bool shape_photometric_trace( PRQPhotometricTrace &ray_p ) 
                                           const {      
        return ( m_tracer_p.process( ray_p ) );
      }

      /**
       * @brief Executes a photometric ray trace on the shape
       * 
       * This method executes two traces: first a trace from the observer
       * position along the look direction vector, lookdir, and then if
       * successful, from the sun position location, sunpos_km, to the surface
       * intercept location of the observer_km/lookdir_km trace. 
       * 
       * If the observer_km/lookdir_km trace is successful, the look direction
       * from the sun position is computed by subtracting the surface intersect
       * point of the observer_km/lookdir_km trace from the sunpos_km position.
       * Then a second trace is executed along that look direction. 
       * 
       * Note that if the sunpos_km/sunlookdir_km does not intersect the
       * location with 1 millimeter tolerance it is likely that from the
       * position of the sun, the surface intercept point is occluded/obscured
       * by foreground terrain. This distance can be computed using the two
       * ray traces contained in the trace object and using the
       * PsmrtsRayTrace::distance() method.
       * 
       */
      inline PRQPhotometricTrace shape_photometric_trace( const std::vector<double> &observer_km,
                                                          const std::vector<double> &lookdir_km,
                                                          const std::vector<double> &sunpos_km ) 
                                                          const {

        PRQPhotometricTrace ray_p( Eigen::Vector3d( observer_km.data() ), 
                                   Eigen::Vector3d( lookdir_km.data() ),
                                   Eigen::Vector3d( sunpos_km.data() ) );
        (void) this->shape_photometric_trace( ray_p );
        return ( ray_p );
      }

      /** Use PSMRTS stardard API for tracing shape models */
      inline PRQPhotometricTrace shape_photometric_trace( const Eigen::Vector3d &observer_km,
                                                          const Eigen::Vector3d &lookdir_km,
                                                          const Eigen::Vector3d &sunpos_km )
                                                          const {

        PRQPhotometricTrace ray_p( observer_km, lookdir_km, sunpos_km );
        this->shape_photometric_trace( ray_p );
        return ( ray_p );
      }      

      /**
       * @brief Executes a photometric ray trace on the ellipsoid
       * 
       * This method executes two traces: first a trace from the observer
       * position along the look direction vector, lookdir, and then if
       * successful, from the sun position location, sunpos_km, to the surface
       * intercept location of the observer_km/lookdir_km trace. 
       * 
       * If the observer_km/lookdir_km trace is successful, the look direction
       * from the sun position is computed by subtracting the surface intersect
       * point of the observer_km/lookdir_km trace from the sunpos_km position.
       * Then a second trace is executed along that look direction. 
       * 
       * Note that if the sunpos_km/sunlookdir_km does not intersect the
       * location with 1 millimeter tolerance it is likely that from the
       * position of the sun, the surface intercept point is occluded/obscured
       * by foreground terrain. This distance between the intercept points can
       * be computed using the two ray traces contained in the trace object and
       * using the PsmrtsRayTrace::distance() method of one of the rays.
       * 
       */      
      inline PRQPhotometricTrace ellipsoid_photometric_trace( const std::vector<double> &observer_km,
                                                              const std::vector<double> &lookdir_km,
                                                              const std::vector<double> &sunpos_km ) 
                                                              const {

        PRQPhotometricTrace ray_p( Eigen::Vector3d( observer_km.data() ), 
                                   Eigen::Vector3d( lookdir_km.data() ),
                                   Eigen::Vector3d( sunpos_km.data() ) );
        (void) this->ellipsoid_photometric_trace( ray_p );
        return ( ray_p );
      }

      /** Use PSMRTS API for photometric trace on ellipsoid model */
      inline PRQPhotometricTrace ellipsoid_photometric_trace( const Eigen::Vector3d &observer_km,
                                                              const Eigen::Vector3d &lookdir_km,
                                                              const Eigen::Vector3d &sunpos_km ) 
                                                              const {

        PRQPhotometricTrace ray_p( observer_km, lookdir_km, sunpos_km );
        this->ellipsoid_photometric_trace( ray_p );
        return ( ray_p );
      }      

      /**
       * @brief Executes photometric ray trace operation on a configured object
       * 
       * The "ray_p" photometric trace object contains an observer postion a
       * look direction vector from the observer and the location of the sun,
       * all in body-fixed coordinates. It executes at most two traces on the
       * ellipsoid tracer, the first being the observer/look direction and, if a
       * succesful surface intercept is made, it computes the look direction
       * vector from the sun position to the surface intercept point and executes
       * the second trace.
       * 
       * @param ray_p  A configured photometric trace object with observer
       *               position, look direction vector and the position of the
       *               sun. 
       * @return true  If both traces are successful. 
       * @return false If one or both traces fail.
       */      
      inline bool ellipsoid_photometric_trace( PRQPhotometricTrace &ray_p ) 
                                               const {      
        return ( m_ellipsoid_r.process( ray_p ) );
      }

      inline const PsmrtsInvoice &invoice() const { 
        return ( m_invoice );
      }

      inline const PsmrtsTranslations &translations() const {
        return ( m_invoice.translations() );
      }

    private:
      PsmrtsInvoice            m_invoice;
      PsmrtsPriorityTracer     m_tracer_p;
      PsmrtsTracer             m_ellipsoid_r;
      static inline std::mutex m_mutex{};
  };
}

#endif
