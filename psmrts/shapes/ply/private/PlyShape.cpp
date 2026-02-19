/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include "../PlyShape.hpp"
#include "PsmrtsPLYFormat.hpp"

namespace psmrts {
  PlyShape::PlyShape( const std::string &ply_file ) : PsmrtsProduct( ply_file, "ply"){
    PsmrtsPLYFormat m_model( ply_file );
    m_config = m_model.get_metadata(); // check if can rename to config
    m_mesh = m_model.get_mesh();
  }

  PlyShape::PlyShape( const ProductCart &processed_cart ) {
    this->set_name( processed_cart.name() );
    this->set_type( "ply" );
    this->create( processed_cart );
  }     
  
  void PlyShape::create( const ProductCart &cart ) {

    // Check for valid shape type
    if (cart.error_count() > 0 ) {
      std::string mess = "PlyShape::create(" + cart.name() + 
                         ") has config/spec processing errors: \n" +
                          cart.errors_to_string();
      throw std::runtime_error( mess );          
    }
    if (cart.error_count() > 0 ) {
      std::string mess = "PlyShape::create(" + cart.name() + ") has errors: " +
                          cart.errors_to_string();
      throw std::runtime_error( mess );          
    }

    m_config = cart.configuration();
    if ( m_config.contains( "shape" ) ) {
      if ( m_config.find( "shape" ).to_string() != "ply" ) {
        std::string mess = "PlyShape::create() - shape type must be \"ply\""
                            " but found " + m_config.find("shape").to_string();
        throw std::runtime_error( mess );
      }
    }

    std::string plyfile;
    // Check for obj_file
    if ( m_config.contains( "ply_file" ) ) {
      plyfile  = m_config.find( "ply_file" ).to_string();
      if ( m_config.metadata().contains( "ply_file_expanded" ) ) {
        plyfile =  m_config.metadata().find( "ply_file_expanded" ).to_string();
      }
    }
    else {
      std::string mess = "PlyShape - ply_file not found in config";
    }

    // Load the PLY file
    PsmrtsPLYFormat model_p( plyfile );

    if ( m_config.contains( "ply_data_type") && 
          ( m_config.find( "ply_data_type" ).to_string() == "float" ) ) {
      m_mesh = PsmrtsMeshData( model_p.get_indexes(), model_p.get_float_vectors() );
    }
    else {
      m_mesh = PsmrtsMeshData( model_p.get_indexes(), model_p.get_double_vectors() );
    }

    m_config = model_p.get_metadata();
  }


} // namespace psmrts
