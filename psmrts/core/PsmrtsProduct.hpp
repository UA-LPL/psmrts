#ifndef PsmrtsProduct_hpp
#define PsmrtsProduct_hpp
#pragma once

#include <exception>
#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>

namespace psmrts {

  /** 
   * @brief Base class of a PSMRTS product
   * 
   * This class is the base class for all (cacheable) PSMRTS products.
   * It should be used for all products to standardize management
   * of these class objects.
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-05-02 Kris J. Becker  Original Version
   */
  class PsmrtsProduct {
    public:
      using UIDType = PsmrtsUID::UIDType;

      PsmrtsProduct( ) : m_tracker(), m_name("product"), m_type("type"), 
                         m_product_id( PsmrtsUID::get_uid() ) { }
      PsmrtsProduct( const std::string &pname ):
                     m_tracker(), m_name( pname ), m_type( "product" ),      
                     m_product_id( PsmrtsUID::get_uid() )  { }
      PsmrtsProduct( const std::string &pname, 
                     const std::string &ptype ):
                     m_tracker(), m_name( pname ), m_type( ptype ),
                     m_product_id( PsmrtsUID::get_uid() )  { }
      virtual ~PsmrtsProduct() { }


      inline const PsmrtsProduct &product() const {
        return ( *this );
      }

      /** Returns the name of the product */
      inline const std::string &name() const {
        return ( m_name );
      }
      
      /** Returns the type of the product */
      inline const std::string &type() const {
        return ( m_type );
      }

      /** Returns the unique ID of the product */
      inline const UIDType &uid() const {
        return ( m_product_id );
      }
      
      /** Returns a distinct timestamp since the product has been created */
      inline PsmrtsThreadSafeCounter timestamp() const {
        return ( m_tracker.clone() );
      }

    private:
      PsmrtsThreadSafeCounter m_tracker;
      std::string             m_name;
      std::string             m_type;
      UIDType                 m_product_id;
  };

} // namespace psmrts

#endif
