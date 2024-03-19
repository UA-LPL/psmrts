#ifndef DskSegment_hpp
#define DskSegment_hpp

#include <exception>
#include <string>
#include <vector>
#include <mutex>

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
   * See also DskKernelModel.hpp.
   * 
   */
  class DskSegment {
    public:
      DskSegment( ) { init(); }

      /** DSK segment constructor w/all data */
      DskSegment( const SpiceDLADescr &dladsc, 
                  const SpiceDSKDescr &dskdsc,
                  const SpiceInt nvertices,
                  const SpiceInt nplates,
                  const int segnum ) {
        init();

        m_segnum    = segnum;
        m_dla_descr = dladsc;
        m_dsk_descr = dskdsc;
        m_vertices  = nvertices;
        m_plates    = nplates;

        // Compute radii/extremes
        m_radii = compute_min_max_radii( m_min_radius, m_max_radius );
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

      /** Unique segment identifier within this DSK/body */
      inline SpiceInt id() const {
        return ( surfaceid() );
      }
      
      inline SpiceInt n_vertices() const {
        return ( m_vertices );
      }
           
      inline SpiceInt n_plates() const {
        return ( m_plates );
      }

      inline SpiceInt n_vectors() const {
        return ( n_vertices() );
      }      
           
      inline const SpiceDLADescr &dladsc() const {
        return ( m_dla_descr );
      }

      inline SpiceDLADescr *dladsc_ptr()  {
        return ( &m_dla_descr );
      }

      inline const SpiceDLADescr *dladsc_ptr() const {
        return ( &m_dla_descr );
      }

      inline const SpiceDSKDescr &dskdsc() const {
        return ( m_dsk_descr );
      }
      

      inline SpiceDSKDescr *dskdsc_ptr()  {
        return ( &m_dsk_descr );
      }

      inline const SpiceDSKDescr *dskdsc_ptr() const {
        return ( &m_dsk_descr );
      }
      
      inline Eigen::Vector3d radii( ) const {
        return ( m_radii );
      }

      inline double minimum_radius( ) const {
        return ( m_min_radius );
      }

      inline double maximum_radius( ) const {
        return ( m_max_radius );
      }

      /** Body ID, typically a NAIF FRAME ID */
      inline SpiceInt bodyid() const {
        return ( dskdsc().center );
      }

      /** Unique surface identifier, typically a NAIF FRAME ID */
      inline SpiceInt surfaceid() const {
        return ( dskdsc().surfce );
      }

      inline SpiceInt frameid() const {
        return ( dskdsc().frmcde );
      }

      inline SpiceInt dtype() const {
        return ( dskdsc().dtype );
      }

      inline SpiceInt dclass() const {
        return ( dskdsc().dclass );
      }

    private:
      // DSK Parameters
      int             m_segnum;
      SpiceDLADescr   m_dla_descr;
      SpiceDSKDescr   m_dsk_descr;
      SpiceInt        m_plates;
      SpiceInt        m_vertices;
      Eigen::Vector3d m_radii;
      double          m_min_radius;
      double          m_max_radius;

      /** Initialize the DSK structure */
      inline void init( ) {
        m_segnum     = -1;
        m_dla_descr  = { 0 };
        m_dsk_descr  = { 0 };
        m_plates     = 0;
        m_vertices   = 0;
        m_radii      = Eigen::Vector3d::Zero();
        m_min_radius = 0.0;
        m_max_radius = 0.0;
      }

      inline Eigen::Vector3d compute_min_max_radii(double &minrad, double &maxrad ) const {
        minrad = maxrad = 0.0;
        Eigen::Vector3d v_radii = Eigen::Vector3d::Zero();

        if ( dskdsc().corsys  == SPICE_DSK_PDTSYS ) {
          double re = dskdsc().corpar[0];  // Equitorial radius
          double f  = dskdsc().corpar[1];  
          double rp = re * ( 1.0 - f );    // Polar radius 
          minrad = std::min( re, rp );
          maxrad = std::max( re, rp );
          v_radii = { re, re, rp };
        }
        else if ( dskdsc().corsys  == SPICE_DSK_LATSYS ) {
          minrad = dskdsc().co3min;
          maxrad = dskdsc().co3max;
          v_radii = { maxrad, maxrad, maxrad };
        }
        else if (dskdsc().corsys  == SPICE_DSK_RECSYS ) { 
          Eigen::Vector3d vradmin( { dskdsc().co1min, dskdsc().co2min, dskdsc().co3min } );
          Eigen::Vector3d vradmax( { dskdsc().co1max, dskdsc().co2max, dskdsc().co3max } );
          minrad = vradmin.norm();
          maxrad = vradmax.norm();
          v_radii = vradmax;
        }
        else {
          std::string mess = "DSK Segment type " + std::to_string( dskdsc().corsys ) +
                            " coordinate system not supported";
          throw std::runtime_error( mess );
        }

        return ( v_radii );
      }
  };

} // namespace naif

#endif
