/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef NaifDskTracer_hpp
#define NaifDskTracer_hpp

#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductFeature.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>

#include <psmrts/algorithms/TracingBasics.hpp>
#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>

namespace psmrts  {
  /**
   * @brief NAIF DSK ShapeModel
   * 
   * 
   */
  class NaifDskTracer : public PsmrtsProduct {
    public:
      using ProductInfo     = ProductSpecification::ProductInfo;
      using ProductFeatures = ProductSpecification::ProductFeatures;

      NaifDskTracer( ) : PsmrtsProduct( "naifdsktracer", "tracer" ), 
                         m_model() {
        m_config = init_naifdsk( "naifdsk" );
      }
      NaifDskTracer( const naif::DskKernelModel &dsktracer ) : 
                     PsmrtsProduct( dsktracer.shapefile(), "tracer" ),
                     m_model( dsktracer ) { 
        m_config = init_naifdsk( dsktracer, dsktracer.shapefile() );
      }
      NaifDskTracer( const std::string &dsk ) : 
                     PsmrtsProduct( dsk, "tracer" ),
                     m_model( dsk ) {
        m_config = init_naifdsk( m_model, dsk );
      }
      NaifDskTracer( const ProductCart &processed_cart ) {
        this->set_name( processed_cart.name() );
        this->set_type( "naifdsk" );           
        this->create( processed_cart );
      }        
      virtual ~NaifDskTracer() { }

      /**
       * @brief NAIF Dsk Ray Trace Processor
       * 
       * This method accepts a PRQRayTrace, which is instantiated with 
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
      inline bool process ( PRQRayTrace &trace ) const {
        return ( algorithms::process_basic_trace( *this, trace ) );
      }

      /**
       * @brief NAIF Dsk Ray Trace Array Processor
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
      inline bool process ( PRQRayTraceArray &tracelist ) const {
        return ( algorithms::process_basic_trace_array( *this, tracelist ) );
      }

      /**
       * @brief NAIF Dsk Facet Processor
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
      inline bool process( PRQFacet &facet ) const {
        return ( algorithms::process_basic_facet( *this, facet ) );
      }

      /**
       * @brief NAIF Dsk Photometric Trace Processor
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
      inline bool process( PRQPhotometricTrace &trace_p ) const {
        return ( algorithms::process_basic_photometric_trace( *this, trace_p ) );
      }

       /**
       * @brief NAIF Dsk Photometric Trace Array Processor
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
       inline bool process( PRQPhotometricTraceArray &tracelist ) const {
        return ( algorithms::process_basic_photometric_trace_array( *this, tracelist ) );
      }

       /**
       * @brief NAIF Dsk Features Processor
       * 
       * This method accepts a PRQFeatures, and stores into it all the 
       * relevant DSK information using JSON.
       * 
       * @param features PRQFeatures that holds tracer-relevant information
       *                  in a JSON format
       * @return true    If features were added successfully
       * @return false   If any issues during processing
       */
      inline bool process( PRQFeatures &features ) const {
        psmrts_json f_e;
        f_e += { "name" , "naifdsk" };
        f_e += { "product" , "shapetracer" };
        f_e += { "mesh" , true };
        features.add_feature( f_e );
        return ( true );
      }

      /**
       * @brief NAIF NaifDsk Ray Trace Method
       * 
       * Deriving classes must implement this method as is specified for 
       * shape models.
       * 
       * It must include designated values necessary for a Ray Trace (observer, look direction),
       * return true if trace results in an intercept, and store the relevant result data into 
       * a PsmrtsRayTrace object.
       * 
       * @param observer Location of the observer (s/c) relative to the target body
       * @param lookdir  Look direction of the ray from the observer to
       *                   trace for intersections
       * @param ray      PsmrtsRayTrace returns the results of the trace
       * @return true    If trace intercepts shape
       * @return false   If trace fails to intercept
       */
      inline bool ray_trace( const Eigen::Vector3d &observer,
                              const Eigen::Vector3d &lookdir,
                              PsmrtsRayTrace &ray ) const {
        // this->local_tracker()++;
        return ( this->ray_trace( ray.reset( observer, lookdir ) ) );
      }
      
      inline bool ray_trace( PsmrtsRayTrace &ray ) const {
        bool status = m_model.ray_trace( ray );
        ray.set_tracer_id( this->uid() );
        return ( status );
      } 
      
      inline bool get_facet(  const PsmrtsRayTrace &ray, 
                              PsmrtsRayTrace::FacetDatum &facet) const {
        return ( m_model.get_facet( ray, facet ) );                                 
      }

      /** Report all remaining features not available - e.g., PRQFacet not relevant to Ellipsoid format */
      PSMRTS_PROCESS_CATCHALL( "NaifDskTracer" )

      static inline ProductSpecification product_specifications() {
        ProductInfo  info( "naifdsk", { 
                                 ProductOption( "name", "naifdsk"),
                                 ProductOption( "product", "tracer"),
                                 ProductOption( "description", "NAIF DSK ray tracing system specifications") } );
        ProductFeature product( "tracer", {
                                 ProductOption( "name", "tracer" ),
                                 ProductOption( "type", "string" ),
                                 ProductOption( "description", "Describe the product type" ),
                                 ProductOption( "status", "optional" ),
                                 ProductOption( "default", "naifdsk" ),
                                 ProductOption( "valid", "naifdsk" ) } );
        ProductFeature dfile( "dsk_file", {
                                 ProductOption( "name", "dsk_file" ),
                                 ProductOption( "type", "file" ),
                                 ProductOption( "description", "Name of DSK kernel" ),
                                 ProductOption( "status", "required" ),
                                 ProductOption( "aliases", { "file", "filename" } ),
                                 ProductOption( "file_suffixes", { "bds", "BDS" } ) } );
        ProductFeature bodyid( "dsk_body_id", {
                                 ProductOption( "name", "dsk_surface_id" ),
                                 ProductOption( "type", "int" ),
                                 ProductOption( "description", "NAIF ID of the target body whose surface is described" ),
                                 ProductOption( "status", "optional" ),
                                 ProductOption( "conflicts_with", "dsk_segment_index" ),
                                 ProductOption( "aliases", { "target_id", "naif_id" } ) } );
        ProductFeature segid( "dsk_segment_index", {
                                 ProductOption( "name", "dsk_segment_index" ),
                                 ProductOption( "type", "int" ),
                                 ProductOption( "description", "Segement index of desired body in the DSK file" ),
                                 ProductOption( "status", "optional" ),
                                 ProductOption( "conflicts_with", "dsk_surface_id" ),
                                 ProductOption( "default", 0 ),
                                 ProductOption( "aliases", { "segment", "dsk_segment" } ) } );
#if 0
        // Not implemented.  
        ProductFeature kernels( "kernels", {
                                 ProductOption( "name", "kernels" ),
                                 ProductOption( "type", "string" ),
                                 ProductOption( "description", "Additional kernels required to load for target" ),
                                 ProductOption( "status", "optional" ),
                                 ProductOption( "aliases", "required_kernels" ) } );
#endif
        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( info, { product, dfile, bodyid, segid } ) );
      }
      
      
      inline const ProductConfiguration &config() const {
        return ( m_config );
      }
      
      inline bool matches( const ProductConfiguration &conf ) const {
        return ( this->config().matches( conf ) );
      }

      inline void create( const ProductCart &cart ) {

        // Check for valid shape type
        if ( cart.error_count() > 0 ) {
          std::string mess = "EllipsoidTracer::create(" + cart.name() + 
                            ") has config/spec processing errors: \n" +
                              cart.errors_to_string();
          throw std::runtime_error( mess );          
        }

        if ( !cart.isvalid() ) {
          std::string mess = "EllipsoidTracer::create(" + cart.name() + 
                            ") is invalid with " + 
                            std::to_string( cart.configuration().size() ) +
                            " config options and " +
                            std::to_string( cart.residual().size() ) +
                            " residual options";
          throw std::runtime_error( mess );          
        }

        ProductConfiguration v_conf = cart.configuration();
        if ( v_conf.contains( "dsk_segment_index" ) && ( v_conf.contains( "dsk_body_id" ) ) ) {
          std::string mess = "NaifDskTracer::create(" + cart.name() + 
                            ") cannot have both dsk_segment_index and dsk_surface_id";
          throw std::runtime_error( mess );    
        }

        // Get the sk file and open it
        std::string dskfile;
        if ( v_conf.metadata().contains( "dsk_file_expanded" ) ) {
          dskfile = v_conf.metadata().find("dsk_file_expanded").to_string();
        }
        else {
          dskfile = v_conf.find("dsk_file").to_string();
        }
        m_model = naif::DskKernelModel( dskfile );

        // Check for segment indexes
        if ( v_conf.contains( "dsk_segment_index" ) ) {
          std::vector<int> segnums = OptionIntegersExtractor( v_conf.find( "dsk_segment_index" ) ).get_all();
          std::vector<naif::DskSegment> segments;
          for ( const auto &segnum : segnums ) {
            segments.push_back( m_model.segment( segnum ) );
          }

          m_model = naif::DskKernelModel( m_model, segments );
        }

        // Check for surface id requests
        if ( v_conf.contains( "dsk_surface_id" ) ) {
          std::vector<int> surfids = OptionIntegersExtractor( v_conf.find( "dsk_surface_id" ) ).get_all();
          std::vector<naif::DskSegment> segments;
          for ( const auto &sid : surfids ) {
            const naif::DskSegment *segment = m_model.get_segment_with_id( sid );
            if ( nullptr == segment ) {
              std::string mess = "Cannot find segment with (surface) id " + 
                                  std::to_string( sid ) + " to create new model";
              throw std::runtime_error( mess );
            }
            segments.push_back( *segment );
          }
          m_model = naif::DskKernelModel( m_model, segments );
        }

          m_config = ProductConfiguration( "naifdsk" );
          m_config.add( ProductOption( "tracer", "naifdsk" ) );
          m_config.merge( m_model.config( m_model.segments() ) );
          return;     
      }

      inline double maximum_radius() const {
        return ( m_model.maximum_radius() );
      }

      inline double minimum_radius() const {
        return ( m_model.minimum_radius() );
      }

    private:
      naif::DskKernelModel m_model;
      ProductConfiguration m_config;

      inline ProductConfiguration init_naifdsk( const std::string &source ) {
        auto config = ProductConfiguration( source, { ProductOption( "tracer", "naifdsk" ),
                                                      ProductOption( "file", source ) } );
        return ( config );
      }

      inline ProductConfiguration init_naifdsk( const naif::DskKernelModel &model, const std::string &source ) {
        auto config = ProductConfiguration( source, { ProductOption( "tracer", "naifdsk" ),
                                                      ProductOption( "file", source ),
                                                      ProductOption( "plates", std::to_string(model.plate_count())),
                                                      ProductOption( "segments", std::to_string(model.n_dsk_segments())) } );
        return ( config );
      }
  };

} // namespace psmrts

#endif
