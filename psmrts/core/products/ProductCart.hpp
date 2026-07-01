/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef ProductCart_hpp
#define ProductCart_hpp

#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/PsmrtsTranslations.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>

namespace psmrts {

  namespace json_p = psmrts::json_utils;
  
  /** 
   * @brief Product cart contains a specification and a configuration
   * 
   * This class contains a product configuration and an associated product
   * specification used to evalute the configuration. The configuration may not
   * satisfy the specification where this situation will be indicate by a
   * non-zero error count.
   * 
   * Residual config options will be retained in a list to indicate options that
   * do not apply/comply with the specifications. This typically will occur when
   * a specification parses a configuration and one or more product options are
   * not recognized or does not satisfy the specification. Residual options are
   * not necessarily an error. For example, a PsmrtsTracer spec may requires a
   * shape that contains its own individaul options passed on in an additional
   * configuration for parsing by a PsmrtsShape product.
   * 
   * This is merely a convenient container. Processing of its contents is
   * performed by other classes. It does standardize/expedite constructing and
   * comparing PSRMTS products.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2026-02-14 Kris J. Becker  Original Version
   */
  class ProductCart : public PsmrtsRequest {
    public:
      using ProductOptionList = ProductConfiguration::ProductOptionList;
      using ResidualOptions   = PsmrtsContainer<ProductOption>;
      using UIDType           = PsmrtsProduct::UIDType;


      ProductCart( ) : PsmrtsRequest( "ProductCart" ),
                       m_specs(  ),
                       m_config( ),
                       m_residual( "residualoptions" ),
                       m_tracer_uid( PsmrtsUID::null_uid() ),
                       m_shape_uid( PsmrtsUID::null_uid() ) { }
      ProductCart( const std::string &name ) : 
                   PsmrtsRequest( name ),
                   m_specs(  ),
                   m_config( ),
                   m_residual( "residualoptions" ),
                   m_tracer_uid( PsmrtsUID::null_uid() ),
                   m_shape_uid( PsmrtsUID::null_uid() ) { }
      ProductCart( const ProductSpecification &specs ) : 
                   PsmrtsRequest( specs.name() ),
                   m_specs( specs ),
                   m_config(  ),
                   m_residual( "residualoptions" ),
                   m_tracer_uid( PsmrtsUID::null_uid() ),
                   m_shape_uid( PsmrtsUID::null_uid() ) { }                    
      ProductCart( const ProductSpecification &specs,
                   const ProductConfiguration &config,
                   const ResidualOptions &residuals = ResidualOptions( "residualoptions" )) : 
                   PsmrtsRequest( config.name() ),
                   m_specs( specs ),
                   m_config( config ),
                   m_residual( residuals ),
                   m_tracer_uid( PsmrtsUID::null_uid() ),
                   m_shape_uid( PsmrtsUID::null_uid() ) { }                                     
      virtual ~ProductCart() = default; 
       
      /** Special constructor to extract a product from a combined config */
      static inline ProductCart extract_config(const ProductConfiguration &config,
                                               const ProductSpecification &specs ) {
        ProductCart cart_t( specs );
        ResidualOptions residuals;
        cart_t.m_config   = specs.extract( config, residuals, cart_t );
        cart_t.m_residual = residuals;
        return ( cart_t );
      }

      inline bool isvalid() const {
        return ( this->error_count() == 0 );
      }

      inline bool has_valid_content() const {
        return ( this->isvalid() && 
                ( m_config.size() >  0 ) && 
                ( m_specs.size() > 0 ) &&
                ( this->residual_size() == 0 ) );
      }      

      inline size_t size() const {
        return ( m_config.size() );
      }

     inline bool isempty() const {
        return ( ( m_config.size() ==   0 ) && 
                 ( m_specs.size() ==  0 ) &&
                 ( this->residual_size() == 0 ) );
      }

      /** The product name such as "obj", "bullet" as defined in the specs */
      inline const std::string &name () const {
        return ( this->specification().name() );
      }

      /** The product type, "tracer" or "shape", as defined in the specs */
      inline const std::string &product() const {
        return ( this->specification().product() );
      }

      inline size_t residual_size() const {
        return ( m_residual.size() );
      }

      inline ProductCart &set_configuration( const ProductConfiguration &config = ProductConfiguration( "none" ) ) {
        m_config = config;
        return ( *this );
      }

      inline const ProductConfiguration &configuration() const {
        return ( m_config );
      }

      inline const ProductOptionList &options() const {
        return ( m_config.options() );
      }      

      inline void add_option( const ProductOption &option ) {
        m_config.add_option( option );
      }

      inline void add_metadata( const ProductOption &option ) {
        m_config.add_metadata( option );
      }

      inline ProductCart &set_specification( const ProductSpecification &specs = ProductSpecification() ) {
        m_specs = specs;
        return ( *this );
      }

      inline const ProductSpecification &specification() const {
        return ( m_specs );
      }  

      inline const ResidualOptions &residual() const {
        return ( m_residual );
      }

      inline void add_residual( const ProductOption &option ) {
        m_residual.add( option );
      }

      inline void clear_residuals() {
        m_residual.clear();
      }

      inline ProductConfiguration residual_config( ) const {
        return ( ProductConfiguration( m_config.name(), m_residual ) );
      }

      inline ordered_json to_json() const {
        ordered_json order_j;
        order_j.update( json_p::insert_object( "specification",  m_specs.to_json() )  );
        order_j.update( json_p::insert_object( "configuration",  m_config.to_json() )  );
        order_j.update( json_p::insert_object( "residualoptions",  process_json( m_residual ) ) );
        return ( order_j );
      }

      inline ProductCart &set_tracer_uid( const UIDType uid_t ) {
        m_tracer_uid = uid_t;
        return ( *this );
      }

      inline UIDType get_tracer_uid( ) const {
        return ( m_tracer_uid );
      }
      
      inline ProductCart &set_shape_uid( const UIDType uid_t ) {
        m_shape_uid = uid_t;
        return ( *this );
      }
      
      inline UIDType get_shape_uid( ) const {
        return ( m_shape_uid );
      }      

    private:
      ProductSpecification m_specs;
      ProductConfiguration m_config;
      ResidualOptions      m_residual;
      UIDType              m_tracer_uid;
      UIDType              m_shape_uid;      

      /** Convert the residual objects to an json structure */
      inline ordered_json process_json( const ResidualOptions &c ) const {

        ordered_json j_opts;
        for ( const auto &opt_t : c ) {
           j_opts.update( opt_t.to_json() );
        }

        return ( j_opts );
      }      
  };

} // namespace psmrts

#endif
