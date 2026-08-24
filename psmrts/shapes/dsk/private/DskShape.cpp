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
#include <psmrts/core/AllOptionConversions.hpp>
#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>


namespace psmrts {

  DskShape::DskShape( const std::string &dsk_file,
                      const int segnum) : PsmrtsProduct( dsk_file, "dsk") {

    this->set_name( dsk_file );
    this->set_type( "shape" );
    this->set_model( "dsk" );

    PsmrtsTranslations trans = PsmrtsTranslations::create();
    std::string dsk_expanded = trans.translate_path( dsk_file );

    naif::DskKernelModel  model_d( dsk_expanded );
    naif::DskSegment segment_d = model_d.segment( segnum );

    m_config = model_d.create_segment_config( segment_d, dsk_file );
    m_config.add( ProductOption( "shape", "dsk" ) );
    m_config.add_metadata( ProductOption( "dsk_file_expanded", dsk_expanded ) );

    m_mesh = make_shared_copy( PsmrtsMeshData( model_d.load_facet_indexes( &segment_d), 
                               model_d.load_facet_vectors( &segment_d) ) );
  }


  DskShape::DskShape( const ProductCart &processed_cart ) :
                      PsmrtsProduct( processed_cart.configuration().name(), "shape", "dsk" ) {

    naif::DskKernelModel model_d( processed_cart, m_config );
    m_config.add( ProductOption( "shape", "dsk" ) ); 
    this->set_name( model_d.name() );

    // Load the requested segment and generate the config
    naif::DskSegment segment_d = model_d.segment();
    m_mesh = make_shared_copy( PsmrtsMeshData( model_d.load_facet_indexes( &segment_d ), 
                               model_d.load_facet_vectors( &segment_d ) ) );    
  }


} // namespace psmrts
