/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef DskSegment_hpp
#define DskSegment_hpp

#include <string>

#include <Eigen/Geometry>
#include <psmrts/core/ProductOption.hpp>
#include <psmrts/core/ProductConfiguration.hpp>

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
      using ProductOption       = psmrts::ProductOption;
      using ProductConfiguration = psmrts::ProductConfiguration;
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

      /** Returns validity state of segment - valid so long as it contains vertices */
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
      
      /** Returns number of vertices in segment */
      inline SpiceInt n_vertices() const {
        return ( m_vertices );
      }
      
      /** Returns number of plates in segment */
      inline SpiceInt n_plates() const {
        return ( m_plates );
      }

      /** Returns number of vectors in segment (vertices) */
      inline SpiceInt n_vectors() const {
        return ( n_vertices() );
      }      
      
      /** Returns DLA Descriptor associated with segment */
      inline const SpiceDLADescr &dladsc() const {
        return ( m_dla_descr );
      }

      /** Returns pointer to DLA Descriptor associated with segment */
      inline SpiceDLADescr *dladsc_ptr()  {
        return ( &m_dla_descr );
      }

      /** Returns pointer to DLA Descriptor associated with segment */
      inline const SpiceDLADescr *dladsc_ptr() const {
        return ( &m_dla_descr );
      }

      /** Returns DSK Descriptor */
      inline const SpiceDSKDescr &dskdsc() const {
        return ( m_dsk_descr );
      }
      
      /** Returns pointer to DSK Descriptor */
      inline SpiceDSKDescr *dskdsc_ptr()  {
        return ( &m_dsk_descr );
      }

      /** Returns pointer to DSK Descriptor */
      inline const SpiceDSKDescr *dskdsc_ptr() const {
        return ( &m_dsk_descr );
      }
      
      /** Returns radii vector */
      inline Eigen::Vector3d radii( ) const {
        return ( m_radii );
      }

      /** Returns segment's minimum radius */
      inline double minimum_radius( ) const {
        return ( m_min_radius );
      }

      /** Returns segment's maximum radius */
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

      /** Returns frame id */
      inline SpiceInt frameid() const {
        return ( dskdsc().frmcde );
      }

      /** Returns the DSK segment data type */
      inline SpiceInt dtype() const {
        return ( dskdsc().dtype );
      }

      /** Returns the data class of the segment (aspects of topology) */
      inline SpiceInt dclass() const {
        return ( dskdsc().dclass );
      }

      inline ProductConfiguration config() const {
        ProductConfiguration config( "dsksegment" );

        config.add( ProductOption( "dsk_body_id", this->bodyid() ) );
        config.add( ProductOption( "dsk_segment_index", this->segment_number() ) );
        config.add_metadata( ProductOption( "dsk_frame_id", this->frameid() ) );
        config.add_metadata( ProductOption( "dsk_surface_id", this->surfaceid() ) );
        config.add_metadata( ProductOption( "dsk_body_id", this->bodyid() ) );
        config.add_metadata( ProductOption( "dsk_segment_type", this->dtype() ) );
        config.add_metadata( ProductOption( "dsk_class_type", this->dclass() ) );
        config.add_metadata( ProductOption( "n_vertices",     this->n_vectors() ) );
        config.add_metadata( ProductOption( "n_facets",       this->n_plates() ) );
        config.add_metadata( ProductOption( "maximum_radius", this->maximum_radius() ) );
        config.add_metadata( ProductOption( "maximum_radius", this->maximum_radius() ) );

        return ( config );

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
