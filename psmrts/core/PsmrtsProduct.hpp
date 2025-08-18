#ifndef PsmrtsProduct_hpp
#define PsmrtsProduct_hpp
#pragma once

#include <exception>
#include <string>
#include <variant>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsParameters.hpp>

namespace psmrts {

  /** 
   * @brief A variant class to manage a product from a specific line
   * 
   * This template class accepts a std::variant type, which in
   * PSMRTS is a format (mesh), tracer, shape tracer or priority
   * tracer. Each variant type must use std::monostate as its
   * first type (for allocation efficiencies and indicate no product).
   * 
   * @code
   *   using V = std::variant<std::monstate, PsmrtsPlyFormat, PsmrtsOBJFormat, PsmrtsDSKFormat>;
   * @endcode
   * 
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-05-02 Kris J. Becker  Original Version
   */
  template <typename... Ts>
    class PsmrtsProduct {
      public:
        using ProductSet = std::variant<Ts...>;
        using Product    = std::variant<std::monostate, Ts...>;  // Generic use of product

        PsmrtsProduct( ) { 
          init();
        }
        
        PsmrtsProduct(  const std::string &pname,
                        const std::string &ptype,
                        const std::string &pconfig = ""  ) {
          
          init( pname, ptype, this->parse_config( pconfig ) );
        }

        PsmrtsProduct(  const std::string &pname,
                        const std::string &ptype,
                        const ordered_json &pconfig_j  ) {

          init( pname, ptype, pconfig_j );
        }

        PsmrtsProduct(  const std::string &pname,
                        const std::string &ptype,
                        const Product &product,
                        const ordered_json &pconfig_j = json_utils::json_null() ) {

          init( pname, ptype, pconfig_j );
          m_product = product;
        }

        virtual ~PsmrtsProduct() { }


        inline const std::string &name() const {
          return ( m_name );
        }

        inline const std::string &type() const {
          return ( m_type );
        }

        inline bool has_product() const {
          return ( m_product.index() != 0 );
        }

        template <typename T>
          inline bool contains() const {
            return ( nullptr != std::get_if<T>( &m_product ) );
          }        

        inline bool isValid() const {
          return ( this->has_product() );
        }

        inline void set_product( const Product &p ) {
          m_product  = p;
          // m_config_j = p.config();
        }

        inline void reset() {
          m_product = Product();
          m_config_j = psmrts::json_utils::null();
        }

        /**
         * @brief Call product-type operator defined in parameter request
         * 
         * The parameter "request" of type P is a functor object with a
         * operator method that accepts the product type T as a parameter. 
         * The functor object declartion would look like:cast_shared_ptr
         * 
         * @code
         *   [inline] bool P.operator()( const T &product_t );
         * @endcode
         * 
         * @tparam P      Functor object with the proper product type operator
         * @param request Functor to receive the product stored this product
         * @return true   Functor operator return status: if successful, true -
         * @return false    if the operation failed, false is returned.
         */
        template < typename P> 
         inline bool process( P &request ) const {

          // If it contains a product, return its info
         if ( this->has_product() ) {
          return ( request( m_product ) );
         }

         return ( false );
        }

        /**
         * @brief Returns a pointer to stored type T instance
         * 
         * If this object contains a valid type, it will be 
         * 
         * @return const T* 
         */
        template <typename T>
        inline const T *product( ) const {
          if ( !this->has_product() ) return ( nullptr );
          return ( std::get_if<T>( &m_product ) );
        }

        inline const ordered_json &config() const {
          return ( m_config_j );
        }

        inline ordered_json parse_config( const std::string &str_j ) {
          if ( !str_j.empty() ) {
            return ( json_utils::parse_json_string( str_j ) );
          }
          return ( json_utils::null() );
        }

      private:
        PsmrtsThreadSafeCounter m_tracker;
        std::string             m_name;
        std::string             m_type;
        Product                 m_product;
        ordered_json            m_config_j;

        inline void init( const std::string &name_p = "PsmrtsInventory",
                          const std::string &type_p = "none",
                          const ordered_json &config_j = json_utils::null() ) {
          m_tracker  = PsmrtsThreadSafeCounter();
          m_name     = name_p;
          m_type     = type_p;
          m_config_j = config_j;
        }

    };

} // namespace psmrts

#endif
