/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef BulletTracer_hpp
#define BulletTracer_hpp

#include <string>
#include <memory>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductFeature.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/algorithms/TracingBasics.hpp>

namespace psmrts  {

  /**
   * @brief Bullet ShapeModel
   * 
   * 
   */
  class BulletTracer : public PsmrtsProduct {
    public:
      using ProductInfo   = ProductSpecification::ProductInfo;
      using FeatureOption = ProductFeature::FeatureOption;
      using FeatureList   = ProductFeature::FeatureOptionList;

      BulletTracer( );
      BulletTracer( const SharedShape &shape );
      BulletTracer( const ProductCart &processed_cart  );      
      BulletTracer( const ProductCart &processed_cart,
                    const SharedShape &shape  );      
      virtual ~BulletTracer();

      double maximum_radius() const;

      double minimum_radius() const;

      /**
       * @brief Bullet Ray Trace Processor
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
       * @brief Bullet Ray Trace Array Processor
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
       * @brief Bullet Facet Processor
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
       * @brief Bullet Photometric Trace Processor
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
       * @brief Bullet Photometric Trace Array Processor
       * 
       * This method accepts a PRQPhotometricTraceArray, which represents
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
      inline bool process ( PRQPhotometricTraceArray &tracelist ) const {
        return ( algorithms::process_basic_photometric_trace_array( *this, tracelist ) );
      }

      /**
       * @brief Bullet Features Processor
       * 
       * This method accepts a PRQFeatures, and stores into it all the 
       * relevant Bullet information using JSON.
       * 
       * @param features PRQFeatures that holds tracer-relevant information
       *                  in a JSON format
       * @return true    If features were added successfully
       * @return false   If any issues during processing
       */
      inline bool process( PRQFeatures &features ) const {
        psmrts_json f_e;
        f_e["name"]        = "bullet";
        f_e["product"]     = "tracer";
        f_e["mesh"]        = true;
        f_e["optimizebvh"] = false;
        f_e["vectortype"]  = { "double", "float" };
        features.add_feature( f_e );
        return ( true );
      }

      inline bool process( PRQShape &shaper ) const {
        shaper.set_shape( this->shape() );
        return ( true );
      }

      /**
       * @brief Bullet Ray Trace Method
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
      
      // These are the tracing and facet implementations in the source class
      bool ray_trace( PsmrtsRayTrace &ray ) const;
      bool get_facet(  const PsmrtsRayTrace &ray, 
                       PsmrtsRayTrace::FacetDatum &facet) const;
            
      static inline ProductSpecification product_specifications() {
        ProductInfo  info( "bullet", { 
                                 FeatureOption( "name", "bullet" ),
                                 FeatureOption( "product", "tracer" ),
                                 FeatureOption( "description", "The Bullet Physics ray tracing system specification") } );
        ProductFeature product( "tracer", {
                                 ProductOption( "name", "tracer" ),
                                 ProductOption( "type", "string" ),
                                 ProductOption( "description", "Describe the product type" ),
                                 ProductOption( "status", "required" ),
                                 ProductOption( "default", "bullet" ),
                                 ProductOption( "valid", "bullet" ) } );
        // Not the most ideal way to require a supported shape file format.
        ProductFeature shapefile( "shape", {
                                 ProductOption( "name", "shape" ),
                                 ProductOption( "type", "string" ),
                                 ProductOption( "status", "dependency" ),
                                 ProductOption( "description", "Bullet requires a file/mesh shape" ),
                                 ProductOption( "aliases", { "file", "filename", 
                                                             "obj_file", "obj_mesh", "obj_string",
                                                             "ply_file", "ply_mesh", 
                                                             "dsk_file", "dsk_mesh", 
                                                             "mesh_file", "source",
                                                            "shape_uid", "mesh_uid", "uid" } ) } );
        ProductFeature shapeuid( "shape_uid", {
                                 ProductOption( "name", "shape_uid" ),
                                 ProductOption( "type", "string" ),
                                 ProductOption( "status", "optional" ),
                                 ProductOption( "description", "Specifies a shape PSMRTS UID" ),
                                 ProductOption( "aliases", { "mesh_uid", "uid" } ) } );                                                             
        ProductFeature bvh( "bullet_optimize_bvh", {
                                 FeatureOption( "name", "bullet_optimize_bvh" ),
                                 FeatureOption( "type", "bool" ),
                                 FeatureOption( "description", "Use optimized bounding volume hierachy (BVH) when created" ),
                                 FeatureOption( "status", "optional" ),
                                 FeatureOption( "default", "true" ),
                                 FeatureOption( "valid", { "true", "1", "yes", "false", "0", "no" } ) } );
        ProductFeature cmp( "bullet_compression", {
                                 FeatureOption( "name", "bullet_compression" ),
                                 FeatureOption( "type", "bool" ),
                                 FeatureOption( "description", "Compress Bullet data during construction" ),
                                 FeatureOption( "status", "optional" ),
                                 FeatureOption( "default", "true" ),
                                 FeatureOption( "valid", { "true", "1", "yes", "false", "0", "no" } ) } );

        // This validates the JSON structure and provides product info to callers
        return ( ProductSpecification( info, { product, shapefile, shapeuid, bvh, cmp } ) );
      }

      /** Return reference to PsmrtsShape used in this instance */
      const SharedShape &shape() const;

      /** Return the current product configuration */
      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      inline bool matches( const ProductConfiguration &conf ) const {
        if ( this->shape()->matches( conf ) && this->config().matches( conf ) ) {
          return ( true );
        }

        return ( false );
      }
                      
      /** Catcha nd report errors on all remaining processes not available */
      PSMRTS_PROCESS_CATCHALL( "BulletTracer" )


    private:
      class BulletTracerImpl;
      std::shared_ptr<BulletTracerImpl> m_model;
      ProductConfiguration              m_config;

      void create( const ProductCart &cart, 
                   const SharedShape &shape = SharedShape() );

  };

} // namespace psmrts

#endif
