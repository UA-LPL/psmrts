/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include "../DskShape.hpp"
#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>
#include <psmrts/core/ProductOrder.hpp>
#include <psmrts/core/AllOptionConversions.hpp>


namespace psmrts {


  inline ProductConfiguration create_segment_config( const naif::DskSegment &segment,
                                                     const std::string &dskfile  ) {

    ProductConfiguration dsk_config ("dsk");
    dsk_config.add( ProductOption( "shape", "dsk" ) );
    dsk_config.add( ProductOption( "dsk_file", dskfile ) );
    dsk_config.add( ProductOption( "data_type", "double" ) );
    dsk_config.add_metadata( ProductOption( "dsk_segments", 1 ) );
    dsk_config.add_metadata( ProductOption( "dsk_segment_number", segment.segment_number() ) );
    dsk_config.add_metadata( ProductOption( "dsk_surface_id", segment.id() ) );
    dsk_config.add_metadata( ProductOption( "n_vertices", segment.n_vertices() ) );
    dsk_config.add_metadata( ProductOption( "n_facets", segment.n_plates() ) );
    dsk_config.add_metadata( ProductOption( "dsk_reference_id", segment.bodyid() ) );
    dsk_config.add_metadata( ProductOption( "dsk_body_id", segment.bodyid() ) );
    dsk_config.add_metadata( ProductOption( "dsk_surface_id", segment.surfaceid() ) );
    dsk_config.add_metadata( ProductOption( "dsk_frame_id", segment.frameid() ) );
    dsk_config.add_metadata( ProductOption( "dsk_type", segment.dtype() ) );
    dsk_config.add_metadata( ProductOption( "dsk_class", segment.dclass() ) );
    dsk_config.add_metadata( ProductOption( "minimum_radius", segment.minimum_radius() ) );
    dsk_config.add_metadata( ProductOption( "maximum_radius", segment.maximum_radius() ) );
    return ( dsk_config );
  }

  DskShape::DskShape( const std::string &dsk_file,
                      const int segnum) : PsmrtsProduct( dsk_file, "dsk") {

    this->set_name( dsk_file );
    this->set_type( "dsk" );

    PsmrtsTranslations trans = PsmrtsTranslations::create();
    std::string dsk_expanded = trans.translate_path( dsk_file );

    naif::DskKernelModel  model_d( dsk_expanded );
    naif::DskSegment segment_d = model_d.segment( segnum );

    m_config = create_segment_config( segment_d, dsk_file );
    m_config.add_metadata( ProductOption( "dsk_file_expanded", dsk_expanded ) );

    m_mesh   = PsmrtsMeshData( model_d.load_facet_indexes( &segment_d), 
                               model_d.load_facet_vectors( &segment_d) );
  }


  DskShape::DskShape( const ProductConfiguration &config,
                      const PsmrtsTranslations &trans ) {
    this->set_name( config.name() );
    this->set_type( "dsk" );
    this->create( config, trans );
  }

  void DskShape::create( const ProductConfiguration &config,
                         const PsmrtsTranslations &trans  ) {

    // Check for valid shape type
    ProductOrder order = this->product_specifications().process_order( config, trans );
    if (order.error_count() > 0 ) {
      std::string mess = "DskShape::create(" + config.name() + ") has errors: " +
                          order.errors_to_string();
      throw std::runtime_error( mess );          
    }

    ProductConfiguration v_conf = order.config();
    ProductConfiguration dsk_config( "dsk" );

    if ( v_conf.contains( "shape" ) ) {
      if ( v_conf.find( "shape" ).to_string() != "dsk" ) {
        std::string mess = "DskShape::create() - shape type must be \"dsk\""
                            " but found " + v_conf.find("shape").to_string();
        throw std::runtime_error( mess );
      }
      dsk_config.add( v_conf.find( "shape" ) );
    }

    std::string dskfile_source;
    std::string dskfile;
    if ( v_conf.contains( "dsk_file" ) ) {
      dskfile_source  = v_conf.find( "dsk_file" ).to_string();
      dsk_config.add( v_conf.find( "dsk_file" ) );

      if ( v_conf.metadata().contains( "dsk_file_expanded" ) ) {
        dskfile =  v_conf.metadata().find( "dsk_file_expanded" ).to_string();
        dsk_config.add( ProductOption( "dsk_file_expanded", dskfile ) );
      }
      else {
        dskfile = dskfile_source;
      }
    }
    else {
      std::string mess = "DskShape - dsk_file not found in config";
      throw std::runtime_error( mess );
    }

    // Open the DSK file
    naif::DskKernelModel  model_d( dskfile );
    int segnum = 0;
    int dskbodyid = 0;
    naif::DskSegment segment_d;

    if ( v_conf.contains( "dsk_segment_number" ) ) {
      ProductOption dsk_seg_num = v_conf.find( "dsk_segment_number" );
      std::vector<int> v_segnums = OptionIntegersExtractor( dsk_seg_num ).get_all();
      if ( v_segnums.size() != 1 ) {
        std::string mess = "DskShape::create() - dsk_segment_number must select "
                           "one segment number but got" +
                           std::to_string( v_segnums.size() );
        throw std::runtime_error( mess );
      }
      
      segnum = v_segnums[0];
      if ( segnum < 1 || segnum >= model_d.n_dsk_segments() ) {
        std::string mess = "DskShape::create() - dsk_segment_number (" +
                           std::to_string(segnum ) + ") is invalid, "
                           " must be 0 to " + std::to_string( v_segnums.size() - 1 );
        throw std::runtime_error( mess );        
      }
      segment_d = model_d.segment( segnum );
      dsk_config.add( ProductOption( "dsk_segment_number", segnum ) );

    }

    if ( v_conf.contains( "dsk_body_id" ) ) {
      ProductOption dsk_body_opt = v_conf.find( "dsk_body_id" );
      std::vector<int> v_bodids = OptionIntegersExtractor( dsk_body_opt ).get_all();
      if ( v_bodids.size() != 1 ) {
        std::string mess = "DskShape::create() - dsk_body_id must select "
                           "one id number but got" +
                           std::to_string( v_bodids.size() );
        throw std::runtime_error( mess );
      }
      
      dskbodyid = v_bodids[0];
      const naif::DskSegment *segment_p = model_d.get_segment_with_id( dskbodyid );
      if ( !segment_p ) {
        std::string mess = "DskShape::create() - specified dsk_body_id (" +
                           std::to_string( dskbodyid ) +") is invalid or does"
                           " not exist in " + dskfile;      
        throw std::runtime_error( mess );        
      }
      segment_d = *segment_p;
      dsk_config.add( ProductOption( "dsk_body_id", segnum ) );
    }

    m_mesh   =  PsmrtsMeshData( model_d.load_facet_indexes( &segment_d), 
                                model_d.load_facet_vectors( &segment_d) );
    m_config = create_segment_config( segment_d, dskfile_source );
    m_config.merge( dsk_config );

  }


} // namespace psmrts
