#pragma once

#include <vector>
#include <string>
#include <exception>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsInventory.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/PRQProduct.hpp>

namespace psmrts { 

  /**
   * @brief PRQShape is a request to create products through configurations
   * 
   * This class accepts a PSRMTS product configuration request that is
   * assumed to contain a shape configuration.
   * 
   * Uses for this approach a will query each known shape to satisfy the
   * options contained in the configuration.
   * 
   * @author 2025-09-22 Kris J Becker, University of Arizona
   * @history 2025-09-22 Kris J Becker, Original Version
   * 
   */
  class PRQShape : public PsmrtsRequest {
    public:

    /** default constructable */
      PRQShape() : PsmrtsRequest( "PRQShape" ) { 
        init();
      }
      PRQShape( const std::string &name ) : PsmrtsRequest( name ) {
        init( name );
      }

      PRQShape( const std::string &name,
                const ProductConfiguration &product_c ) : 
                PsmrtsRequest( name ) { 
        init( name );
        m_config = product_c;
      }
      PRQShape(  const std::string &name,
                   const PsmrtsInventory &local ) : 
                   PsmrtsRequest( name ) { 
        init( name );
        m_local_inventory = local;
      }      
      virtual ~PRQShape() { }
 
      using PsmrtsRequest::name;
      using PsmrtsRequest::run_count;
      using PsmrtsRequest::was_invoked;
      using PsmrtsRequest::error_count;
      using PsmrtsRequest::errors;

      /** Set the product configuration */
      inline void set_config( const ProductConfiguration &config ) {
        m_config = config;
      }

      inline const ProductConfiguration &config() const {
        return ( m_config );
      }

      /** Add a shape to the inventory */
      inline bool add_shape( const PsmrtsShape &shape ) {
        return ( m_product.shapes().add_product( shape ) );
      }

      /**
       * @brief Process a product PRQ containing general configuration
       * 
       * @param inventory 
       * @return true 
       * @return false 
       */
      inline bool process( const PRQProduct &config_p ) {
        ProductConfiguration pc_t = config_p.config();
        ProductSpecification specs_t = EllipsoidTracer::product_specifications();

        // Check for explicit specs for a support tracer type
        ProductOption tracer_t( specs_t.product(), specs_t.name() );
        if ( pc_t.contains( "tracer" ) ) {
          ProductOption tracer_e = pc_t.find( "tracer" );
          if ( tracer_t.to_string() != tracer_e.to_string() ) {
            return ( false );
          }
        }

        // Check for all required options
        ProductConfiguration matched_t( pc_t.name() );
        for ( auto const &name_p : specs_t.required() ) {
          if ( !pc_t.contains( name_p ) ) {
            std::string mess = "*** EllipsoidTracer required key " + name_p + " not in configuration!";
            config_p.add_error( std::runtime_error( mess ) );
            return ( false );
          }
          matched_t.add( pc_t.find( name_p ) );
        }

        // Check if any optional ones are found
        for ( auto const &name_p : specs_t.optional() ) {
          if ( pc_t.contains( name_p ) ) {
            matched_t.add( pc_t.find( name_p ) );
          }
        }

        // If we reach here, instantiate and add to PRQ
        try {
          config_p.add_tracer( EllipsoidTracer( matched_t ) );
          
        }
        catch ( const std::exception &e ) {
           std::string mess = "*** EllipsoidTracer failed with configuration specs = " + matched_t.to_json().dump();
           config_p.add_error( std::runtime_error( mess ) );
           return ( false );
        }
      }        

      }

    public:
      ProductConfiguration     m_config;
      PsmrtsInventory          m_product;

  };

}  // namespace psmrts 
 