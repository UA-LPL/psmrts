#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsProduct_hpp
#define PsmrtsProduct_hpp

#include <string>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsUID.hpp>

namespace psmrts {

  /** 
   * @brief Base class of a PSMRTS product
   * 
   * This class is the base class for all (cacheable) PSMRTS products.
   * It should be used for all products to standardize management
   * of these class objects.
   * 
   * For this class, the "name" is typically going to be a unique identifier.
   * Its intended to be the actual expanded name of a file associated with the
   * product created. 
   * 
   * The "type" is one of "tracer", "shape" or some larger category of  a
   * product.
   * 
   * The "model" explicitly names the implemenation of the product such as the
   * shape reader (e.g., "obj", dsk", or "ply"), or tracer library tool (e.g.,
   * "bullet", "naifdsk", etc).
   * 
   * @author Kris J. Becker, University of Arizona
   * @history 2025-05-02 Kris J. Becker  Original Version
   */
  class PsmrtsProduct {
    public:
      using UIDType = PsmrtsUID::UIDType;

      PsmrtsProduct( ) : m_tracker(), 
                         m_name( "void" ),
                         m_type( "void" ), 
                         m_model( "void" ) ,
                         m_product_id( PsmrtsUID::null_uid() ) { }
      PsmrtsProduct( const std::string &pname ):
                     m_tracker(), 
                     m_name( pname ), 
                     m_type( "type" ), 
                     m_model( "model" ),     
                     m_product_id( PsmrtsUID::get_uid() )  { }
      PsmrtsProduct( const std::string &pname, 
                     const std::string &ptype,
                     const std::string &pmodel = "model" ):
                     m_tracker(), m_name( pname ), 
                     m_type( ptype ), m_model( pmodel ), 
                     m_product_id( PsmrtsUID::get_uid() )  { }
      virtual ~PsmrtsProduct() { }

      inline static PsmrtsProduct VoidProduct( const std::string &name = "void" ) {
        PsmrtsProduct model_v;
        model_v.m_name = name;
        return ( model_v );
      }

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

      /** Returns the model of the product */
      inline const std::string &model() const {
        return ( m_model );
      }

      /** Returns the unique ID of the product */
      inline const UIDType &uid() const {
        return ( m_product_id );
      }
      
      /** Returns a distinct timestamp since the product has been created */
      inline PsmrtsThreadSafeCounter timestamp() const {
        return ( m_tracker.clone() );
      }
    
    protected:
      inline void set_name( const std::string &name ) {
        m_name = name;
      }

      inline void set_type( const std::string &type_p ) {
        m_type = type_p;
      }

      inline void set_model( const std::string &model ) {
        m_model = model;
      }


    private:
      PsmrtsThreadSafeCounter m_tracker;
      std::string             m_name;
      std::string             m_type;
      std::string             m_model;
      UIDType                 m_product_id;
  };

} // namespace psmrts

#endif
