#ifndef PsmrtsDSKFormat_hpp
#define PsmrtsDSKFormat_hpp

#include <string>
#include <memory>
#include <exception>
#include <iostream>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsBufferData.hpp>
#include <PsmrtsVector3.hpp>
#include <PsmrtsMeshData.hpp>
#include <DskKernelModel.hpp>


namespace psmrts {

    /**
     * @brief PsmrtsDSKFormat contains tools for DSK (bds) file format I/O
     * 
     * @author Kris J. Becker and Kyle A. Becker, University of Arizona
     * @history 2025-2-12 Kyle A. Becker   Original Version
     */

    class PsmrtsDSKFormat {
        public:

        /** Default Constructor */
        PsmrtsDSKFormat() {}

        /** Construct a DSK object */
        PsmrtsDSKFormat( const std::string &dskfile ) {
            (void) load_dsk_file( dskfile ); 
        }

        /** Destructor */
        virtual ~PsmrtsDSKFormat() {}

        /** Returns the data source of the format model - DSK filepath */
        virtual std::string format_model_source() {
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

        inline bool load_dsk_file( const std::string &dskfile ) {
            m_dsk_source = dskfile;
            m_mesh = PsmrtsMeshData();

            naif::DskKernelModel d_model( dskfile );

            PsmrtsVector3i dsk_indexes = d_model.load_facet_indexes();
            PsmrtsVector3d dsk_vectors = d_model.load_facet_vectors();

            m_mesh = PsmrtsMeshData( dsk_indexes, dsk_vectors );

            return (m_mesh.isValid() );
        }
        
        // Possible to access dskdsc? Protected via DskKernelModel? For config?

        // WIP - Need index/double vector data 
        // Do we need to worry about encapsulation here? Should we return a mesh replica?
        inline PsmrtsMeshData get_mesh( const bool make_it_a_double = false ) const {
            if ( true == make_it_a_double ) {
                if ( !m_mesh.isVectorDouble() ) {
                    // need index and vector functions
                    return ( PsmrtsMeshData( this->get_indexes(), this->get_double_vectors() ) );
                }
            }
            return (m_mesh);
        }

        private:
            std::string         m_dsk_source;
            PsmrtsMeshData      m_mesh;
    };
} // namespace psmrts

#endif // PsmrtsDSKFormat_hpp