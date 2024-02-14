#ifndef DskKernelModel_hpp
#define DskKernelModel_hpp

#include <exception>
#include <string>
#include <vector>

#include <Eigen/Geometry>
#include <NaifUtilities.hpp>
#include <KernelFileSystem.hpp>

namespace naif {

  /**
   * @brief NAIF DSK intrinsic infrastructure support
   * 
   * This struct contains components for quantifying and providing
   * data to digital shape kernel (DSK) files.
   * 
   */
  class DskSegment {
    public:
      DskSegment( ) { init(); }

      /** Main DSK segment constructor */
      DskSegment( const SpiceInt &handle, 
                  const SpiceDLADescr &dladsc, 
                  const int segnum ) {
        init();

        m_segnum    = segnum;
        m_dla_descr = dladsc;

        // Get the DSK descriptor
        (void) dskgd_c( handle, dladsc_ptr(), dskdsc_ptr() );
        check_naif_errors();

        // Get the number of verticies and plates
        (void) dskz02_c( handle, dladsc_ptr(), &m_vertices, &m_plates );
        check_naif_errors();

        // Compute radii extremes
        compute_min_max_radii( m_min_radius, m_max_radius );
        return;
      }

      /** DSK segment constructor w/all data */
      DskSegment( const SpiceDLADescr &dladsc, 
                  const SpiceDSKDescr &dskdsc,
                  const SpiceInt &nvertices,
                  const SpiceInt &nplates,
                  const int segnum ) {
        init();

        m_segnum    = segnum;
        m_dla_descr = dladsc;
        m_dsk_descr = dskdsc;
        m_vertices  = nvertices;
        m_plates    = nplates;

        // Compute radii extremes
        compute_min_max_radii( m_min_radius, m_max_radius );
        return;
      }

      virtual ~DskSegment() { }


      inline bool isValid() const {
        return ( m_vertices > 0 );
      }

      /** Return the segment number */
      inline SpiceInt segment_number() const {
        return ( m_segnum );
      }
      
      inline SpiceInt n_vertices() const {
        return ( m_vertices );
      }
           
      inline SpiceInt n_plates() const {
        return ( m_plates );
      }
           
      inline const SpiceDLADescr &dladsc() const {
        return ( m_dla_descr );
      }

      inline SpiceDLADescr *dladsc_ptr()  {
        return ( &m_dla_descr );
      }

      inline const SpiceDSKDescr &dskdsc() const {
        return ( m_dsk_descr );
      }

      inline SpiceDSKDescr *dskdsc_ptr()  {
        return ( &m_dsk_descr );
      }

      inline double minimum_radius( ) const {
        return ( m_min_radius );
      }

      inline double maximum_radius( ) const {
        return (m_max_radius );
      }

      inline SpiceInt bodyid() const {
        return ( m_dsk_descr.center );
      }

      inline SpiceInt surfaceid() const {
        return ( m_dsk_descr.surfce );
      }

      inline SpiceInt frameid() const {
        return ( m_dsk_descr.frmcde );
      }

      inline SpiceInt dtype() const {
        return ( m_dsk_descr.dtype );
      }

      inline SpiceInt dclass() const {
        return ( m_dsk_descr.dclass );
      }

    private:
      // DSK Parameters
      int            m_segnum;
      SpiceDLADescr  m_dla_descr;
      SpiceDSKDescr  m_dsk_descr;
      SpiceInt       m_plates;
      SpiceInt       m_vertices;
      double         m_min_radius;
      double         m_max_radius;

      /** Initialize the DSK structure */
      inline void init( ) {
        m_segnum    = -1;
        m_dla_descr = { 0 };
        m_dsk_descr = { 0 };
        m_plates    = 0;
        m_vertices  = 0;
        m_min_radius = 0.0;
        m_max_radius = 0.0;
      }

      inline void compute_min_max_radii( double &minrad, double &maxrad ) 
                                         const {
        minrad = maxrad = 0.0;

        if ( dskdsc().corsys  == SPICE_DSK_PDTSYS ) {
          double re = dskdsc().corpar[0];  // Equitorial radius
          double f  = dskdsc().corpar[1];  
          double rp = re * ( 1.0 - f );    // Polar radius 
          minrad = std::min( re, rp );
          maxrad = std::max( re, rp );
        }
        else if ( dskdsc().corsys  == SPICE_DSK_LATSYS ) {
          minrad = dskdsc().co3min;
          maxrad = dskdsc().co3max;
        }
        else if (dskdsc().corsys  == SPICE_DSK_RECSYS ) { 
          Eigen::Vector3d vradmin( { dskdsc().co1min, dskdsc().co2min, dskdsc().co3min } );
          Eigen::Vector3d vradmax( { dskdsc().co1max, dskdsc().co2max, dskdsc().co3max } );
          minrad = vradmin.norm();
          maxrad = vradmax.norm();
        }
        else {
          std::string mess = "DSK Segment type " + std::to_string( dskdsc().corsys ) +
                            " coordinate system not supported";
          throw std::runtime_error( mess );
        }

        return;
      }
  };


  /**
   * @brief Implementation of DSK model
   * 
   * 
   */
  class DskKernelModel {
    public:
      typedef std::vector<DskSegment>  DskSegmentList;

      DskKernelModel( ) {
        reset();
      }

      DskKernelModel( const KernelDescriptor &k_descr ) {
        reset();
        init( k_descr );
      }
      DskKernelModel( const std::string  &k_dskfile ) {
        reset();
        init( k_dskfile );
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
        if ( !m_kernel_descriptor.isValid() ) return ( false );
        if ( n_dsk_segments() <= 0 ) return ( false );
        if ( m_total_plates   <= 0 ) return ( false );
        if ( m_total_vertices <= 0 ) return ( false );
        return ( true );
      }

      inline DskKernelModel create_from_bodyid( const int bodyid ) const {

        const DskSegment *segment = get_by_bodyid( bodyid );
        if ( nullptr == segment ) {
          std::string mess = "Cannot find segment with body id " + 
                              std::to_string( bodyid ) + " to create new model";
          throw std::runtime_error( mess );
        }

        // We have a valid segment
        return ( DskKernelModel( m_kernel_descriptor, *segment  ) );

      }

      inline const std::string &dskfile() const {
        return ( m_kernel_descriptor.m_kernel_file );
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

      inline std::vector<SpiceInt> get_bodyid_list() const {
        std::vector<SpiceInt> bodyid_list;

        for ( auto const &segment : segments() ) {
          bodyid_list.push_back( segment.bodyid() );
        }
        
        return ( bodyid_list );
      }

      inline const DskSegment *get_by_bodyid( const int bodyid ) const {
        for ( auto const &segment : segments() ) {

          if ( segment.bodyid() == bodyid ) {
            return ( &segment );
          }

        }
        
        return ( nullptr );
      }
      
      /** Return the DSK file handle for the kernel */
      SpiceInt handle() const {
        return ( m_kernel_descriptor.handle() );
      }

    protected:
    /** This is a protected constructor as it requires segement to be in the DSK */
      DskKernelModel( const KernelDescriptor &k_descr, const DskSegment &segment ) {  
        reset();
        init( k_descr, segment );
        return;
      }

    private:
      KernelDescriptor    m_kernel_descriptor;
      DskSegmentList      m_segments;
      size_t              m_total_plates;
      size_t              m_total_vertices;
      double              m_min_radius;
      double              m_max_radius;



      /* Reset DSK model to default state */
      inline void reset( ) {
        m_kernel_descriptor = KernelDescriptor();
        m_segments.clear();
        m_total_plates = 0;
        m_total_vertices = 0;
        m_min_radius = 0.0;
        m_max_radius = 0.0;
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
      inline void init( const KernelDescriptor &k_descr ) {
        m_segments.clear();
        m_kernel_descriptor = k_descr;

        // Check to ensure the file is open
        if ( !k_descr.isValid() ) {
          std::string mess = "*** ERROR - [naif::DskKernelModel] DSK file " + k_descr.m_kernel_file + " is not valid or open";
          throw std::runtime_error( mess );
        }

        // Get the first segment
        SpiceDLADescr v_dladsc;
        SpiceBoolean  v_found;
        dlabfs_c( m_kernel_descriptor.handle(), &v_dladsc, &v_found );
        check_naif_errors();

        // Check to ensure the file is open
        if ( !v_found ) {
          std::string mess = "*** ERROR - [naif::DskKernelModel] DSK file " + k_descr.m_kernel_file + " is not valid/open";
          throw std::runtime_error( mess );
        }

        // Read/process each DSK segment
        int segnum = 0;
        while ( v_found ) {

          DskSegment v_segment( m_kernel_descriptor.handle(), v_dladsc, segnum );
          add_segment( v_segment );

          (void) dlafns_c( m_kernel_descriptor.handle(),
                           v_segment.dladsc_ptr(),
                           &v_dladsc, &v_found );
          check_naif_errors();

          segnum++;
        }

        return;

      }

      inline void init( const KernelDescriptor &kdescr, const DskSegment &segment ) {
        m_kernel_descriptor = kdescr;
        add_segment( segment );
      }

      inline void init( const std::string &dskfile ) {

        load_kernel( dskfile );
        check_naif_errors();

        KernelDescriptor v_kernel = KernelFileSystem::kernel_info( dskfile );
        check_naif_errors();

        init( v_kernel );

        return;
      }

  };

} // namespace naif

#endif
