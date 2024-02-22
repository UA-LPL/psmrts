#ifndef DskKernelModel_hpp
#define DskKernelModel_hpp

#include <exception>
#include <string>
#include <vector>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsDataModel.hpp>
#include <RayTrace.hpp>
#include <NaifUtilities.hpp>
#include <DskSegment.hpp>
#include <KernelFileSystem.hpp>

namespace naif {


  /**
   * @brief Implementation of DSK model support and security
   * 
   * This class manages access to a DSK. Since the NAIF system does not 
   * allow a file to opened more than once, access must be managed
   * per DSK in this class. 
   * 
   * @author 2024-02-19 Kris J. Becker
   */
  class DskKernelModel {
    
    public:
      typedef KernelFileSystem::SharedDskDescriptor  SharedDskDescriptor;
      typedef std::vector<DskSegment>                DskSegmentList;

      typedef psmrts::PsmrtsDataModel<Eigen::Vector3i> DskIndexDataModel;
      typedef psmrts::PsmrtsDataModel<Eigen::Vector3d> DskVectorDataModel;

      DskKernelModel( ) {
        reset();
      }

      DskKernelModel( const std::string  &k_dskfile ) {
        init( k_dskfile );
      }

      DskKernelModel( const SharedDskDescriptor &k_descr ) {
        init( k_descr );
      }

      virtual ~DskKernelModel() { }

      /**
       * @brief Check validity of DSK
       * 
       * In order to be a valid DSK, there must be a valid kernel descriptor,
       * at least one segment and must contain plates and vertices.
       * 
       * @return true   If valid
       * @return false  if invalid
       */
      inline bool isValid( ) const {
        if ( !kernel().found() )       return ( false );
        if ( n_dsk_segments()   <= 0 ) return ( false );
        if ( n_total_plates()   <= 0 ) return ( false );
        if ( n_total_vertices() <= 0 ) return ( false );
        return ( true );
      }

      inline DskKernelModel create_from_id( const int surfaceid ) const {

        const DskSegment *segment = get_segment_with_id( surfaceid );
        if ( nullptr == segment ) {
          std::string mess = "Cannot find segment with (surface) id " + 
                              std::to_string( surfaceid ) + " to create new model";
          throw std::runtime_error( mess );
        }

        // We have a valid segment
        return ( DskKernelModel( *this, *segment ) );
      }

      inline const std::string &dskfile() const {
        return ( kernel().m_kernel_file );
      }

      /** Return the DSK file handle for the kernel */
      SpiceInt handle() const {
        return ( kernel().handle() );
      }

      inline size_t n_total_vertices() const {
        return ( m_total_vertices );
      }
      inline size_t n_total_plates() const {
        return ( m_total_plates );
      }

      inline size_t n_dsk_segments() const {
        return ( m_segments.size() );
      }

      inline const DskSegmentList &segments() const {
        return ( m_segments );
      }

      inline const DskSegment &segment( const int segnum = 0 ) const {
        for ( auto const &segment : segments() ) {
          if ( segment.segment_number() == segnum ) {
            return ( segment );
          }
        }

        // Not found so throw an error
        std::string mess = "*** ERROR - [naif::DskKernelModel] - DSK segment " + std::to_string( segnum ) + " does not exist";
        throw std::runtime_error( mess );
      }

      inline std::vector<SpiceInt> get_id_list() const {
        std::vector<SpiceInt> sid_list;

        for ( auto const &segment : segments() ) {
          sid_list.push_back( segment.id() );
        }
        
        return ( sid_list );
      }

      inline const DskSegment *get_segment_with_id( const int surfaceid ) const {
        for ( auto const &segment : segments() ) {
          if ( segment.id() == surfaceid ) {
            return ( &segment );
          }
        }
        
        return ( nullptr );
      }

      inline size_t use_count() const {
        return ( m_dsk_descriptor.use_count() );
      }

      //**** Ray Tracing routines
      inline bool intercept( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir,
                             const DskSegment &segment, 
                             psmrts::RayTrace::RayTraceDatum &raytrace ) const {

        // Lock up NAIF file I/O for thread safety ( >=c++17 )
        std::scoped_lock mylocker( m_dsk_descriptor.mutex() );

        raytrace.reset();
        raytrace.m_observer = observer;
        raytrace.m_lookdir  = lookdir;
        raytrace.m_segment  = segment.id();

        SpiceBoolean found;
        (void) dskx02_c( kernel().handle(), segment.dladsc_ptr(), 
                         raytrace.m_observer.data(), raytrace.m_lookdir.data(),
                         &raytrace.m_plateid, raytrace.m_xyz.data(), &found);
        check_naif_errors();
        
        raytrace.m_hit = ( SPICETRUE == found );

        // Only get the normal if has intercept
        if ( raytrace.hasHit() ) {
           (void) dskn02_c( kernel().handle(), segment.dladsc_ptr(), 
                            raytrace.m_plateid, raytrace.m_normal.data() );
            check_naif_errors();
        }

        return ( raytrace.hasHit() );
      }

      /**
       * @brief Get the facet for the specified plateid and segment
       * 
       * This method can be used to read a facet from the 
       * 
       * @param raytrace 
       * @param facet 
       * @return true 
       * @return false 
       */
      inline bool get_facet( const psmrts::RayTrace::RayTraceDatum &raytrace,
                             psmrts::RayTrace::FacetDatum &facet ) {
                
        // Sanity check validity of raytrace
        facet.m_has_facet = false;

        if ( raytrace.hasHit() ) {

          const DskSegment *segment = get_segment_with_id( raytrace.m_segment );
          if ( nullptr != segment ) {
            // Lock up NAIF file I/O for thread safety ( >=c++17 )
            std::scoped_lock mylocker( m_dsk_descriptor.mutex() );

            // Fetch the indexes of the facet vectors
            SpiceInt n;
            SpiceInt indexes[3];
            (void) dskp02_c( kernel().handle(), segment->dladsc_ptr(),
                             raytrace.m_plateid, 1, &n, ( SpiceInt (*)[3] ) ( indexes ) );
            check_naif_errors();
            facet.m_indexes = { indexes[0], indexes[1], indexes[2] };

            // Fetch each vector in the facet
            SpiceDouble vector[3];
            (void) dskv02_c( kernel().handle(), segment->dladsc_ptr(),
                             indexes[0], 1, &n, (SpiceDouble (*)[3]) ( vector ) ); 
            check_naif_errors();
            facet.m_vector1 = { vector[0], vector[1], vector[2] };

            (void) dskv02_c( kernel().handle(), segment->dladsc_ptr(),
                             indexes[1], 1, &n, (SpiceDouble (*)[3]) ( vector ) ); 
            check_naif_errors();
            facet.m_vector2 = { vector[0], vector[1], vector[2] };

            (void) dskv02_c( kernel().handle(), segment->dladsc_ptr(),
                             indexes[2], 1, &n, (SpiceDouble (*)[3]) ( vector ) ); 
            check_naif_errors();
            facet.m_vector3 = { vector[0], vector[1], vector[2] };

            facet.m_has_facet = true;
          }
        }

        return ( facet.m_has_facet );
      }


      inline bool intercept(const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir,
                             psmrts::RayTrace::RayTraceDatum &raytrace ) const {

        for ( auto const &segment : segments() ) {
          bool hashit = intercept( observer, lookdir, segment, raytrace );
          if ( true == hashit ) {
            return ( hashit );
          }
        }

        // No intercept found in any segment
        return ( false );
      }

      inline DskIndexDataModel load_facet_indexes( const DskSegment *dsksegment = nullptr ) const {

        const DskSegment &segref = ( nullptr != dsksegment ) ? *dsksegment : this->segment();
        DskIndexDataModel dskndx( segref.n_vertices() );

        // Lock up NAIF file I/O for thread safety ( >=c++17 )
        std::scoped_lock mylocker( m_dsk_descriptor.mutex() );        

        SpiceInt n;
        SpiceInt start = 1;
        (void) dskp02_c( kernel().handle(), segref.dladsc_ptr(),
                         start, segref.n_vertices(), &n, 
                         ( SpiceInt (*)[3] ) ( dskndx(0).data() ) );
        check_naif_errors();

        // Sanity check on the return count
        if ( segref.n_vertices() != n ) {
          std::string mess = "*** ERROR - [naif::DskKernelModel::load_facet_indexes] Expected " + 
                             std::to_string( segref.n_vertices() ) + " but read " + std::to_string( n );
          throw std::runtime_error( mess );
        }

        return ( dskndx );
      }

  inline DskVectorDataModel load_facet_vectors( const DskSegment *dsksegment = nullptr ) const {

        const DskSegment &segref = ( nullptr != dsksegment ) ? *dsksegment : this->segment();

        // For 1-baaed indexing into the vectors
        DskVectorDataModel dskvec( segref.n_vectors() + 1 );

        // Lock up NAIF file I/O for thread safety ( >=c++17 )
        std::scoped_lock mylocker( m_dsk_descriptor.mutex() );        

        SpiceInt n;
        SpiceInt start = 1;
        (void) dskv02_c( kernel().handle(), segref.dladsc_ptr(),
                         start, segref.n_vectors(), &n, 
                         ( SpiceDouble (*)[3] ) ( dskvec(start).data() ) );
        check_naif_errors();

        // Sanity check on the return count
        if ( segref.n_vectors() != n ) {
          std::string mess = "*** ERROR - [naif::DskKernelModel::load_facet_vectors] Expected " + 
                             std::to_string( segref.n_vectors() ) + " but read " + std::to_string( n );
          throw std::runtime_error( mess );
        }
        
        return ( dskvec );
      }


      
    protected:

    /** This is a protected constructor as it requires the segment to be in the DSK */
      DskKernelModel( const DskKernelModel &model, const DskSegment &segment ) {  
        reset( &model.m_dsk_descriptor );
        add_segment( segment );
        return;
      }

      inline const KernelDescriptor &kernel() const {
        return ( m_dsk_descriptor.datum() );
      }

    private:
      SharedDskDescriptor m_dsk_descriptor;
      DskSegmentList      m_segments;
      size_t              m_total_plates;
      size_t              m_total_vertices;


      /* Reset DSK model to default state */
      inline void reset( const SharedDskDescriptor *dskdsc = nullptr ) {
        if ( dskdsc != nullptr ) {
          m_dsk_descriptor = *dskdsc;
        }
        else {
          m_dsk_descriptor = SharedDskDescriptor();
        }

        m_segments.clear();
        m_total_plates = 0;
        m_total_vertices = 0;

        return;
      }


      /** Add a segment to the DSK model */
      inline void add_segment( const DskSegment &segment ) {
          m_total_vertices += segment.n_vertices();
          m_total_plates   += segment.n_plates();

          // Done with this segment so save it!
          m_segments.push_back( segment );
          return;
      }

      /**
       * @brief Initialize the DSK using the kernel descriptpr
       * 
       * See https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/dskgd_c.html.
       * 
       * @param k_descr NAIF kernel descriptor
       */
      inline void init( const SharedDskDescriptor &k_descr ) {
       
        // Completely reset with this descriptor
        reset( &k_descr );

        // Check to ensure the file is open
        if ( !k_descr.datum().found() ) {
          std::string mess = "*** ERROR - [naif::DskKernelModel] DSK file " + k_descr.datum().filename() + " is not valid or open";
          throw std::runtime_error( mess );
        }

        // Lock up NAIF file I/O for thread safety ( >=c++17 )
        std::scoped_lock mylocker( m_dsk_descriptor.mutex() );

        // Get the first segment
        SpiceDLADescr v_dladsc;
        SpiceBoolean  v_found;
        dlabfs_c( kernel().handle(), &v_dladsc, &v_found );
        check_naif_errors();

        // Check to ensure the file is open
        if ( !v_found ) {
          std::string mess = "*** ERROR - [naif::DskKernelModel] DSK file " + kernel().filename() + " is not valid/open";
          throw std::runtime_error( mess );
        }

        // Read/process each DSK segment
        SpiceDSKDescr  v_dskdsc;
        SpiceInt       v_plates;
        SpiceInt       v_vertices;      
        int segnum = 0;
        while ( v_found ) {

          // Get the DSK descriptor
          (void) dskgd_c( kernel().handle(), &v_dladsc, &v_dskdsc);
          check_naif_errors();

          // Get the number of verticies and plates
          (void) dskz02_c( kernel().handle(), &v_dladsc, &v_vertices, &v_plates );
          check_naif_errors();

          // Construct/add the segment
          DskSegment v_segment( v_dladsc, v_dskdsc, v_vertices, v_plates, segnum );
          add_segment( v_segment );

          // Get next DSK segment
          (void) dlafns_c( kernel().handle(),
                           v_segment.dladsc_ptr(),
                           &v_dladsc, &v_found );
          check_naif_errors();

          segnum++;
        }

        return;

      }

      inline void init( const SharedDskDescriptor &kdescr, const DskSegment &segment ) {
        reset( &kdescr );
        add_segment( segment );
      }

      inline void init( const std::string &dskfile ) {
        init( KernelFileSystem::get_shared_descriptor( dskfile ) );
        return;
      }

  };

} // namespace naif

#endif
