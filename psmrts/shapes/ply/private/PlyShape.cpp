/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <memory>

#include "../PlyShape.hpp"
#include "PsmrtsPLYFormat.hpp"

namespace psmrts {

  using UniquePLYFormat = std::unique_ptr<PsmrtsPLYFormat>;


  PlyShape::PlyShape( const std::string &ply_file ) : 
                      PsmrtsProduct( ply_file, "shape", "ply") {
    UniquePLYFormat ply_t = std::make_unique<PsmrtsPLYFormat>( PsmrtsPLYFormat( ply_file ) );
    m_config = ply_t->get_metadata(); // check if can rename to config
    m_mesh = make_shared_copy( ply_t->get_mesh() );
  }

  PlyShape::PlyShape( const ProductCart &processed_cart ) :
                      PsmrtsProduct( processed_cart.configuration().name(), "shape", "ply" ) {
    this->create( processed_cart );
  }     
  
  void PlyShape::create( const ProductCart &cart ) {

    const ProductConfiguration &v_conf = cart.configuration();
    std::string name_t = v_conf.name();

    // Check for valid shape type
    if (cart.error_count() > 0 ) {
      std::string mess = "PlyShape::create(" + name_t + 
                         ") has config/spec processing errors: \n" +
                          cart.errors_to_string();
      throw std::runtime_error( mess );          
    }
    if (cart.error_count() > 0 ) {
      std::string mess = "PlyShape::create(" + name_t + ") has errors: " +
                          cart.errors_to_string();
      throw std::runtime_error( mess );          
    }

    m_config = ProductConfiguration( v_conf.name() );
    if ( v_conf.contains( "shape" ) ) {
      if ( v_conf.find( "shape" ).to_string() != "ply" ) {
        std::string mess = "PlyShape::create() - shape type must be \"ply\""
                            " but found " + v_conf.find("shape").to_string();
        throw std::runtime_error( mess );
      }
    }
    m_config.add( ProductOption( "shape", "ply" ) );

    std::string plyfile          = name_t;
    // Check for obj_file
    if ( v_conf.contains( "ply_file" ) ) {
      plyfile  = v_conf.find( "ply_file" ).to_string();
      m_config.add( ProductOption( "ply_file", plyfile ) );
      name_t = plyfile;
      if ( v_conf.metadata().contains( "ply_file_expanded" ) ) {
        plyfile =  v_conf.metadata().find( "ply_file_expanded" ).to_string();
        m_config.add_metadata( ProductOption( "ply_file_expanded", plyfile) );
      }
    }
    else {
      std::string mess = "PlyShape - ply_file not found in config";
      throw std::runtime_error( mess );
    }

    this->set_name( name_t );

    // Load the PLY file
    UniquePLYFormat ply_t = std::make_unique<PsmrtsPLYFormat>( PsmrtsPLYFormat( plyfile ) );

    if ( v_conf.contains( "ply_data_type") && 
          ( v_conf.find( "ply_data_type" ).to_string() == "float" ) ) {
      m_config.add( ProductOption( "ply_data_type", "float") );
      m_mesh =  make_shared_copy( PsmrtsMeshData( ply_t->get_indexes(), ply_t->get_float_vectors() ) );
    }
    else {
      m_mesh =  make_shared_copy( PsmrtsMeshData( ply_t->get_indexes(), ply_t->get_double_vectors() ) );
    }

    m_config.add_metadata( ply_t->get_metadata().metadata() );
    m_config.add_metadata( ProductOption( "shape_uid", PsmrtsUID::to_string( this->uid() ) ) );

  }


} // namespace psmrts
