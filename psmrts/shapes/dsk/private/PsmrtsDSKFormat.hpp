/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef PsmrtsDSKFormat_hpp
#define PsmrtsDSKFormat_hpp

#include <string>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsBufferData.hpp>
#include <psmrts/core/PsmrtsVector3.hpp>
#include <psmrts/core/PsmrtsMeshData.hpp>
#include <psmrts/core/ProductConfiguration.hpp>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/tracers/naifdsk/private/DskKernelModel.hpp>


namespace psmrts {

    /**
     * @brief PsmrtsDSKFormat contains tools for DSK (bds) file format I/O
     * 
     * @author Kris J. Becker and Kyle A. Becker, University of Arizona
     * @history 2025-2-12 Kyle A. Becker   Original Version
     */

    class PsmrtsDSKFormat {
      public:
        using DskSegmentConfigList = std::vector<ProductConfiguration>;

        /** Default Constructor */
        PsmrtsDSKFormat() {}

        /** Construct a DSK object */
        PsmrtsDSKFormat( const std::string &dskfile ) {
            (void) load_dsk_file( dskfile ); 
        }

        /** Destructor */
        virtual ~PsmrtsDSKFormat() {}

        /** Returns the data source of the format model - DSK filepath */
        virtual std::string format_model_source() const {
            return ( dsk_source() );
        }

        /** Returns validity state of the object's mesh */
        inline bool isValid() const {
            if ( !m_mesh.isValid() ) return ( false );
            return ( true );
        }

        /** The DSK data source */
        inline const std::string &dsk_source() const {
            return ( m_dsk_source );
        }

        /** Returns the number of vertexes */
        inline size_t nVertexes() const {
            return ( m_mesh.nvectors() ); 
        }

        /** Returns the number of indexes */
        inline size_t nIndexes() const {
            return ( m_mesh.nfacets() ); 
        }

        inline int nSegments() const {
            return ( m_segments );
        }

        inline std::string data_type() const {
            return ( m_type );
        }
        /** 
         * @brief Loads a DSK file mesh data
         * 
         * This is the main method that will load a mesh based on a DSK
         * file. It retrieves and allocates appropriate index and double
         * vector data. It is advisable to input the correct pathing
         * prefaced by the PsmrtsUtilities' psmrts_shapes_path function.
         * 
         * @param dskfile      Absolute path string of the target DSK file
         * @return true        If the load was successful
         * @return false       If the load failed
         */
        inline bool load_dsk_file( const std::string &dskfile ) {
            m_dsk_source = dskfile;
            m_mesh = PsmrtsMeshData();

            naif::DskKernelModel d_model( dskfile );

            PsmrtsVector3i dsk_indexes = d_model.load_facet_indexes();
            PsmrtsVector3d dsk_vectors = d_model.load_facet_vectors();

            m_mesh = PsmrtsMeshData( dsk_indexes, dsk_vectors );

            parse_config( d_model ); 

            return (m_mesh.isValid() );
        }
        
        /** Returns Psmrts Mesh of DSK Index / Vector data */
        inline PsmrtsMeshData get_mesh( ) const {
          return (m_mesh);
        }

        /** Get a double precision vector array */
        inline PsmrtsVector3d get_double_vectors() const {
          return ( m_mesh.vectors().double_vectors() );
        }

        /** Get a float precision vector array */
        inline PsmrtsVector3f get_float_vectors() const {
            PsmrtsVector3f v_vectors = m_mesh.vectors().float_vectors();
            if ( m_mesh.vectors().isDouble() ) {
                v_vectors = vector_to_type<PsmrtsVector3f::value_type>( m_mesh.vectors().double_vectors() );
            }
             
            return ( v_vectors );
        }

        /** Get index buffer from DSK file mesh */
        inline PsmrtsVector3i get_indexes() const {
            return ( m_mesh.indexes() );
        }

        inline void parse_config( naif::DskKernelModel &model ) {
            int seg_num = model.n_dsk_segments();
            m_segments = seg_num;
            if ( seg_num > 0 ) {
                m_type = model.segment(0).dtype();
            }
            
            m_config.clear();
            for (int i = 0; i < seg_num; i++ ) {
                m_config.push_back( get_segment_metadata( model.segment(i) ) );
            } 
            return;
        }

        inline ProductConfiguration get_segment_metadata( const naif::DskSegment &segment ) {
          // Create config for each segment, loop.
          ProductConfiguration meta( "dsk" );
          meta.add( ProductOption( "shape", "dsk" ) );
          meta.add( ProductOption( "dsk_file", this->dsk_source() ) );
          meta.add( ProductOption( "data_type", "double" ) );
          meta.add_metadata( ProductOption( "dsk_segments", this->nSegments() ) );
          meta.add_metadata( ProductOption( "dsk_segment_number", segment.segment_number() ) );
          meta.add_metadata( ProductOption( "dsk_surface_id", segment.id() ) );
          meta.add_metadata( ProductOption( "n_vertices", segment.n_vertices() ) );
          meta.add_metadata( ProductOption( "n_facets", segment.n_plates() ) );
          meta.add_metadata( ProductOption( "dsk_reference_id", segment.bodyid() ) );
          meta.add_metadata( ProductOption( "dsk_body_id", segment.bodyid() ) );
          meta.add_metadata( ProductOption( "dsk_surface_id", segment.surfaceid() ) );
          meta.add_metadata( ProductOption( "dsk_frame_id", segment.frameid() ) );
          meta.add_metadata( ProductOption( "dsk_type", segment.dtype() ) );
          meta.add_metadata( ProductOption( "dsk_class", segment.dclass() ) );
          meta.add_metadata( ProductOption( "minimum_radius", segment.minimum_radius() ) );
          meta.add_metadata( ProductOption( "maximum_radius", segment.maximum_radius() ) );

          return ( meta );
        }

        inline const DskSegmentConfigList &config() const {
          return ( m_config );
        }

        private:
          std::string           m_dsk_source;
          int                   m_segments;
          std::string           m_type;
          PsmrtsMeshData        m_mesh;
          DskSegmentConfigList  m_config;
    };
} // namespace psmrts

#endif // PsmrtsDSKFormat_hpp
