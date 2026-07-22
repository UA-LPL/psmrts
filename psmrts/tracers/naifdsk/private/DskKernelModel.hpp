#include <tracy/Tracy.hpp>
/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#ifndef DskKernelModel_hpp
#define DskKernelModel_hpp

#include <string>
#include <vector>
#include <map>
#include <mutex>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsProduct.hpp>
#include <psmrts/core/PsmrtsBufferData.hpp>
#include <psmrts/core/PsmrtsBuffer.hpp>
#include <psmrts/core/PsmrtsVector3.hpp>
#include <psmrts/core/PsmrtsRayTrace.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/AllOptionConversions.hpp>
#include <psmrts/core/PsmrtsUID.hpp>


#include "NaifUtilities.hpp"
#include "DskSegment.hpp"
#include "KernelFileSystem.hpp"

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
  class DskKernelModel : public psmrts::PsmrtsProduct {
    
    public:
      // SharedDskDescriptor is the unique thread safe latch on the NAIF DSK file
      typedef KernelFileSystem::SharedDskDescriptor   SharedDskDescriptor;

      // List of DSK segments in the file
      typedef std::vector<DskSegment>                 DskSegmentList;

      // Data types/representations for the indexes and facet vectors/DSK segment
      typedef psmrts::PsmrtsVector3i            DskIndexDataModel;
      typedef psmrts::PsmrtsVector3d            DskVectorDataModel;

      /** Returns model type, ie. naifdsk */
      inline std::string tracer_model_type() const {
        return ( std::string( "naifdsk" ) );
      }

      /** Returns model name, ie. DskKernelModel */
      inline std::string tracer_model_name() const {
        return ( std::string( "DskKernelModel" )) ;
      }

      /** Returns tracer identification as a combination of model type, model name, and shape name */
      inline std::string shape_tracer_id() const {
        std::string shapename = shapefile();
        if ( shapename.length() == 0 ) shapename = "none";
        return ( tracer_model_type() + "::" + tracer_model_name() + "::" + shapename );
      }

      /** Default constructor */
      DskKernelModel( ) : psmrts::PsmrtsProduct( "dskkernelmodel", "tracer" ) {
        reset();
      }

      /** Open new or use existing DSK file */
      DskKernelModel( const std::string  &dskfile ) :
                      psmrts::PsmrtsProduct( dskfile, "tracer" ) {
        ZoneScoped;
        init( dskfile );
      }

      DskKernelModel( const std::string  &dskfile, 
                      const Eigen::Vector3d &radii ) :
                      psmrts::PsmrtsProduct( dskfile, "tracer" ) {
        ZoneScoped;
        init( dskfile );
        m_radii = radii;
      }

      /** Initialize with a unique NAIF DSK file descriptor */
      DskKernelModel( const SharedDskDescriptor &k_descr ) :
                      psmrts::PsmrtsProduct( k_descr.datum().m_source_file, "tracer" ) {
       ZoneScoped;
       init( k_descr );
      }

      /** Recreate a model using a DSK segment from an existing segment */
      DskKernelModel( const DskKernelModel &model, const DskSegment &segment ) :
                      psmrts::PsmrtsProduct( model.name(), "tracer" ) {  
        ZoneScoped;
        reset( &model.dskdsc() );
        add_segment( segment );
        return;
      }

      /** Recreate a model using a DSK segment from an existing segment */
      DskKernelModel( const DskKernelModel &model, 
                      const std::vector<DskSegment> &segments ) :
                      psmrts::PsmrtsProduct( model.name(), "tracer" ) {  
        ZoneScoped;
        reset( &model.dskdsc() );
        for (const auto &segment : segments ) {
          add_segment( segment );
        }
        return;
      }
      
      /** Create from product cart and return configuration */
      DskKernelModel( const psmrts::ProductCart &processed_cart,
                      psmrts::ProductConfiguration &config ) :
                      psmrts::PsmrtsProduct( processed_cart.configuration().name(),
                                             "tracer" )  {
        ZoneScoped;
        config = this->create_from_cart( processed_cart );
      }        
      
      /** Destructor */
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

      /**
       * @brief Create a DskKernelModel from only one segment
       * 
       * This method constructor will search this DSK for a segment defined
       * for the given surface id. This is typically a NAIF code, but essentially
       * is a unique value contained in each segments DSK des
       * 
       * @param surfaceid 
       * @return DskKernelModel 
       */
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

      /**
       * @brief Complete initialziation from a product cart object
       * 
       * @param processed_cart A configured cart containing a DSK shape configuration.
       * @return psmrts::ProductConfiguration 
       */
      inline psmrts::ProductConfiguration create_from_cart( const psmrts::ProductCart &processed_cart ) {

        ZoneScoped;
        std::string name_t = processed_cart.configuration().name();

        // Check for valid shape type
        if (processed_cart.error_count() > 0 ) {
          std::string mess = "DskKernelModel::create_from_cart(" + name_t + 
                            ") has config/spec processing errors: \n" +
                              processed_cart.errors_to_string();
          throw std::runtime_error( mess );          
        }

        psmrts::ProductConfiguration v_conf = processed_cart.configuration();
        psmrts::ProductConfiguration dsk_config( name_t );

        if ( v_conf.contains( "shape" ) ) {
          if ( v_conf.find( "shape" ).to_string() != "dsk" ) {
            std::string mess = "DskKernelModel::create_from_cart() - shape type must be \"dsk\""
                                " but found " + v_conf.find("shape").to_string();
            throw std::runtime_error( mess );
          }
          dsk_config.add( v_conf.find( "shape" ) );
        }

        if ( v_conf.contains( "tracer" ) ) {
          if ( v_conf.find( "tracer" ).to_string() != "naifdsk" ) {
            std::string mess = "DskKernelModel::create_from_cart() - tracer type must be \"naifdsk\""
                                " but found " + v_conf.find("shape").to_string();
            throw std::runtime_error( mess );
          }
          dsk_config.add( v_conf.find( "tracer" ) );
        }        

        std::string dskfile          = name_t;
        std::string dskfile_expanded = name_t;
        if ( v_conf.contains( "dsk_file" ) ) {
          dskfile = v_conf.find( "dsk_file" ).to_string();
          name_t  = dskfile;
          dsk_config.add( psmrts::ProductOption( "dsk_file", dskfile ) );

          if ( v_conf.metadata().contains( "dsk_file_expanded" ) ) {
            dskfile_expanded =  v_conf.metadata().find( "dsk_file_expanded" ).to_string();
            dsk_config.add_metadata( psmrts::ProductOption( "dsk_file_expanded", dskfile_expanded ) );
          }
          else {
            dskfile_expanded = dskfile;
          }
        }
        else {
          std::string mess = "DskShape - dsk_file not found in config";
          throw std::runtime_error( mess );
        }

        // Open the DSK file
        this->set_name( name_t );
        this->init ( dskfile_expanded );

        DskSegment segment_d = this->segment( 0 );
        int segnum = 0;
        int dskbodyid = 0;

        // Add DSK file based info to metadata
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_segments", this->n_dsk_segments() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "total_verticies", this->n_total_vertices() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "total_facets", this->n_total_plates() ) );

        // Check for a specific segment request
        if ( v_conf.contains( "dsk_segment_number" ) ) {
          psmrts::ProductOption dsk_seg_num = v_conf.find( "dsk_segment_number" );
          std::vector<int> v_segnums = psmrts::OptionIntegersExtractor( dsk_seg_num ).get_all();
          if ( v_segnums.size() != 1 ) {
            std::string mess = "DskKernelModel::create_from_cart() - dsk_segment_number must select "
                              "one segment number but got" +
                              std::to_string( v_segnums.size() );
            throw std::runtime_error( mess );
          }
          
          segnum = v_segnums[0];
          if ( segnum < 1 || segnum >= this->n_dsk_segments() ) {
            std::string mess = "DskKernelModel::create_from_cart() - dsk_segment_number (" +
                              std::to_string(segnum ) + ") is invalid, "
                              " must be 0 to " + std::to_string( v_segnums.size() - 1 );
            throw std::runtime_error( mess );        
          }
          segment_d = this->segment( segnum );
          dsk_config.add( dsk_seg_num );
        }

        // Check for a body id request
        if ( v_conf.contains( "dsk_body_id" ) ) {
          psmrts::ProductOption dsk_body_opt = v_conf.find( "dsk_body_id" );
          std::vector<int> v_bodids = psmrts::OptionIntegersExtractor( dsk_body_opt ).get_all();
          if ( v_bodids.size() != 1 ) {
            std::string mess = "DskKernelModel::create_from_cart() - dsk_body_id must select "
                              "one id number but got" +
                              std::to_string( v_bodids.size() );
            throw std::runtime_error( mess );
          }
          
          dskbodyid = v_bodids[0];
          const DskSegment *segment_p = this->get_segment_with_id( dskbodyid );
          if ( !segment_p ) {
            std::string mess = "DskKernelModel::create_from_cart() - specified dsk_body_id (" +
                              std::to_string( dskbodyid ) +") is invalid or does"
                              " not exist in " + dskfile;      
            throw std::runtime_error( mess );        
          }
          segment_d = *segment_p;
          dsk_config.add( dsk_body_opt );
        }

        // Set the total segement to the specific one
        m_segments = { segment_d };

        dsk_config = this->create_segment_config( segment_d, dskfile ).merge( dsk_config );
        dsk_config.add_metadata( psmrts::ProductOption( "shape_uid", psmrts::PsmrtsUID::to_string( this->uid() ) ) );
        return ( dsk_config );
      }

      /** Return the name of the NAIF DSK kernel file */
      inline const std::string &shapefile() const {
        return ( kernel().m_kernel_file );
      }

      /** Return the DSK file handle for the kernel */
      SpiceInt handle() const {
        return ( kernel().handle() );
      }

      /** Return total vertices in all DSK segments */
      inline size_t n_total_vertices() const {
        return ( m_total_vertices );
      }

      /** Return number of vertices */
      inline size_t vertex_count() const {
        return ( this->n_total_vertices() );
      }

      /** Return total number of facets/plates in all DSK segments */
      inline size_t n_total_plates() const {
        return ( m_total_plates );
      }

      /** Return number of plates */
      inline size_t plate_count() const {
        return ( n_total_plates() );
      }

      /** Returns the number of DSKs contained in this object */
      inline size_t n_dsk_segments() const {
        return ( m_segments.size() );
      }

      /** Returns reference to list of DSK segments */
      inline const DskSegmentList &segments() const {
        return ( m_segments );
      }

      /** Returns a refernce to the nth DSK segment. Exceptions are thrown for bad index */
      inline const DskSegment &segment( const int segnum = 0 ) const {
        ZoneScoped;
        for ( const auto &segment : segments() ) {
          if ( segment.segment_number() == segnum ) {
            return ( segment );
          }
        }

        // Not found so throw an error
        std::string mess = "*** ERROR - [naif::DskKernelModel] - DSK segment " + std::to_string( segnum ) + " does not exist";
        throw std::runtime_error( mess );
      }

      /** Returns list of all DSK segment ids in this object */
      inline std::vector<SpiceInt> get_id_list() const {
        std::vector<SpiceInt> sid_list;

        for ( const auto &segment : segments() ) {
          sid_list.push_back( segment.id() );
        }
        
        return ( sid_list );
      }

      /** Returns pointer to segment with a specifed id, or nullptr if it doesn't exist */
      inline const DskSegment *get_segment_with_id( const int surfaceid ) const {
        for ( const auto &segment : segments() ) {
          if ( segment.id() == surfaceid ) {
            return ( &segment );
          }
        }
        
        // Segment does not exist with that surface id
        return ( nullptr );
      }

      /** Add a segment to the DSK model */
      inline void add_segment( const DskSegment &segment ) {
          m_total_vertices += segment.n_vertices();
          m_total_plates   += segment.n_plates();

          // Just use the first segment as the radii. Note this is likely an
          // issue for multiple segments with different bodies or a situation
          // where the segment does not have global coverage!
          if ( this->n_dsk_segments() == 0 ) { m_radii = segment.radii(); }

          // Done with this segment so save it!
          m_segments.push_back( segment );
          return;
      }      

      /** Returns minimum radius */
      inline double minimum_radius() const {
        ZoneScoped;
        return ( this->segment().minimum_radius() );
      }

      /** Returns maximum radius */
      inline double maximum_radius() const {
        ZoneScoped;
        return ( this->segment().maximum_radius() );
      }

      /** Returns radii vector */
      inline Eigen::Vector3d radii() const {
        return ( m_radii );
      }      

      /** Returns the number of shared instances exist for this DSK file */
      inline size_t use_count() const {
        return ( m_dsk_descriptor.use_count() );
      }

      //-----> Ray Tracing routines <------

     /**
      * @brief Trace a look vector from an observer in the DSK segment
      * 
      * This method will trace a ray defined as a look direction , from
      * an observer location within a given segment in the DSK file.
      * 
      * @param observer 
      * @param lookdir 
      * @param segment 
      * @param ray
      * @return true 
      * @return false 
      */
      inline bool ray_trace( const Eigen::Vector3d &observer, 
                             const Eigen::Vector3d &lookdir,
                             const DskSegment &segment, 
                             psmrts::PsmrtsRayTrace &ray ) const {
        return ( this->ray_trace( segment, ray.reset( observer, lookdir) ) );
      }

      inline bool ray_trace( const DskSegment &segment, 
                             psmrts::PsmrtsRayTrace &ray ) const {                              
        ZoneScoped;

        // Lock up NAIF file I/O for thread safety ( >=c++17 )
        std::scoped_lock mylocker( this->mutex() );

        psmrts::PsmrtsRayTrace::RayTraceDatum &datum_r = ray.datum();
        datum_r.m_segment  = segment.id();

        SpiceBoolean found;
        SpiceInt plateid; //reinterpret_cast<SpiceInt*>(&datum_r.m_plateid)
        (void) dskx02_c( kernel().handle(), segment.dladsc_ptr(), 
                         datum_r.m_observer.data(), datum_r.m_lookdir.data(),
                         &plateid, datum_r.m_xyz.data(), &found);
        datum_r.m_plateid = plateid;
        check_naif_errors();
        
        datum_r.m_hit = ( SPICETRUE == found );

        // Only get the normal if has intercept
        if ( datum_r.hasHit() ) {
           (void) dskn02_c( kernel().handle(), segment.dladsc_ptr(), 
                            datum_r.m_plateid, datum_r.m_normal.data() );
            check_naif_errors();
        }
        // Return to 0-base index.
        datum_r.m_plateid -= 1;

        return ( ray.hasHit() );
      }

      /**
       * @brief Trace a look vector from an observer location in all DSK segments
       * 
       * 
       * @param observer 
       * @param lookdir 
       * @param ray 
       * @return true 
       * @return false 
       */
      inline bool ray_trace( const Eigen::Vector3d &observer, 
                              const Eigen::Vector3d &lookdir,
                              psmrts::PsmrtsRayTrace &ray ) const {
        return( this->ray_trace( ray.reset( observer, lookdir) ) );
      }

      inline bool ray_trace( psmrts::PsmrtsRayTrace &ray ) const {
        ZoneScoped;
        m_tracker++;
        for ( const auto &segment : segments() ) {
          bool has_hit = this->ray_trace( segment, ray );
          if ( true == has_hit ) {
            return ( has_hit );
          }
        }

        // No intercept found in any segment
        return ( false );
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
      inline bool get_facet( const psmrts::PsmrtsRayTrace &ray,
                             psmrts::PsmrtsRayTrace::FacetDatum &facet ) const {
                
        ZoneScoped;
        // Sanity check validity of raytrace
        facet.m_has_facet = false;
        if ( ray.hasHit() ) {

          const DskSegment *segment = get_segment_with_id( ray.segment_number() );
          if ( nullptr != segment ) {
            // Lock up NAIF file I/O for thread safety ( >=c++17 )
            std::scoped_lock mylocker( this->mutex() );

            // Fetch the indexes of the facet vectors
            SpiceInt n;
            SpiceInt indexes[3];

            // Adding 1 to m_plateid to return to 1-based index, consistent with dsk.
            (void) dskp02_c( kernel().handle(), segment->dladsc_ptr(),
                             ray.plateid()+1, 1, &n, ( SpiceInt (*)[3] ) ( indexes ) );
            check_naif_errors();

            // set plateid and segment in facet
            facet.m_plateid = ray.plateid();
            facet.m_segment = ray.segment_number();

            // Converting back to 0-based for return
            facet.m_indexes = { indexes[0]-1, indexes[1]-1, indexes[2]-1 };

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

            facet.m_normal = psmrts::compute_normal( facet.m_vector1,
                                                     facet.m_vector2,
                                                     facet.m_vector3 );

            facet.m_has_facet = true;
          }
        }

        return ( facet.m_has_facet );
      }

      /**
       * @brief Read all facet plate indexes from a DSK segment
       * 
       * This loads all the vector indexes that make a 3-vector facet corner.
       * 
       * In the NAIF system, these are 1-based array indexes. Since this is an
       * export operation, the indexes are convert to 0-based indexes
       * prior to returning the array. This conforms with most other readers.
       * 
       * To get the actual facet vectors, use load_facet_vectors( dsksegment ).
       * 
       * To loop through all facets:
       * @code
       *  auto indexes = load_facet_indexes();
       *  auto vectors = load_facet_vectors();
       * 
       * for ( size_t facet = 0 ; facet < indices.size() ; facet++ ) {
       *    Eigen::Vector3d v0 = vectors( indexes(facet)[0] );
       *    Eigen::Vector3d v1 = vectors( indexes(facet)[1] );
       *    Eigen::Vector3d v2 = vectors( indexes(facet)[2] );
       * }
       * @endcode
       * 
       * @param dsksegment          Optional or first DSK segment to read the data
       * @return DskIndexDataModel  The index data array providing a 3 element
       *                              index into the vector array.
       */
      inline DskIndexDataModel load_facet_indexes( const DskSegment *dsksegment = nullptr ) const {
        ZoneScoped;

        const DskSegment &segref = ( nullptr != dsksegment ) ? *dsksegment : this->segment();
        DskIndexDataModel dskndx( segref.n_plates() );

        // Lock up NAIF file I/O for thread safety ( >=c++17 )
        std::scoped_lock mylocker( this->mutex() );        

        SpiceInt n;
        SpiceInt start = 1;
        (void) dskp02_c( kernel().handle(), segref.dladsc_ptr(),
                         start, segref.n_plates(), &n, 
                         ( SpiceInt (*)[3] ) ( dskndx(0).data() ) );
        check_naif_errors();

        // Sanity check on the return count
        if ( segref.n_plates() != n ) {
          std::string mess = "*** ERROR - [naif::DskKernelModel::load_facet_indexes] Expected " + 
                             std::to_string( segref.n_plates() ) + " but read " + std::to_string( n );
          throw std::runtime_error( mess );
        }

        // The facet indexes in NAIF are 1-based. Convert them to 0-based here!
        DskIndexDataModel::vector_type ones = DskIndexDataModel::vector_type::Ones();
        for ( int i = 0 ; i < dskndx.size() ; i++ ) {
          dskndx( i ) = dskndx( i ) - ones;
        }

        return ( dskndx );
      }

      /**
       * @brief Read all facet vectors from a DSK segment
       * 
       * This method reads the entire set of shape vectors into a data buffer.
       * 
       * @param dsksegment          Optional or first segment to get the
       *                              vectors from
       * @return DskVectorDataModel Returns the data as a 3-vector buffer
       */
      inline DskVectorDataModel load_facet_vectors( const DskSegment *dsksegment = nullptr ) const {

        ZoneScoped;
        const DskSegment &segref = ( nullptr != dsksegment ) ? *dsksegment : this->segment();

        // For 1-baaed indexing into the vectors
        DskVectorDataModel dskvec( segref.n_vectors() );

        // Lock up NAIF file I/O for thread safety ( >=c++17 )
        std::scoped_lock mylocker( this->mutex() );        

        SpiceInt n;
        SpiceInt start = 1;
        (void) dskv02_c( kernel().handle(), segref.dladsc_ptr(),
                         start, segref.n_vectors(), &n, 
                         ( SpiceDouble (*)[3] ) ( dskvec(0).data() ) );
        check_naif_errors();

        // Sanity check on the return count
        if ( segref.n_vectors() != n ) {
          std::string mess = "*** ERROR - [naif::DskKernelModel::load_facet_vectors] Expected " + 
                             std::to_string( segref.n_vectors() ) + " but read " + std::to_string( n );
          throw std::runtime_error( mess );
        }
        
        return ( dskvec );
      }

      /** Returns a clone of the model */
      inline DskKernelModel clone() const {
        return ( *this );
      }

      inline double elapsed_life_time_s() const {
        return ( m_tracker.runtime_s() );
      }

      inline size_t track_count() const {
        return ( m_tracker.count() );
      }
      
      inline psmrts::PsmrtsThreadSafeCounter performance_snapshot() const {
        return ( m_tracker.clone() );
      }

      inline psmrts::ProductConfiguration create_segment_config( const DskSegment &segment,
                                                                 const std::string &dskfile  ) const {
        ZoneScoped;
        psmrts::ProductConfiguration dsk_config ( dskfile );
        // dsk_config.add( psmrts::ProductOption( "shape", "dsk" ) );
        dsk_config.add( psmrts::ProductOption( "dsk_file", dskfile ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_data_type", "double" ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_segments", 1 ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_segment_number", segment.segment_number() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_segment_id", segment.id() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "n_vertices", segment.n_vertices() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "n_facets", segment.n_plates() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_reference_id", segment.bodyid() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_body_id", segment.bodyid() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_surface_id", segment.surfaceid() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_frame_id", segment.frameid() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_type", segment.dtype() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "dsk_class", segment.dclass() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "minimum_radius", segment.minimum_radius() ) );
        dsk_config.add_metadata( psmrts::ProductOption( "maximum_radius", segment.maximum_radius() ) );
        return ( dsk_config );
      }      


      inline psmrts::ProductConfiguration config( const std::vector<DskSegment> &segments ) const {
        ZoneScoped;
        psmrts::ProductConfiguration config( "dsksegment" );

        std::vector<int> bodyid, segnum, surfid, frameid, segtype, classtype, nvertices, nfacets;
        std::vector<double> maxradius, minradius;
        for ( const auto &segment : segments ) {
          bodyid.push_back( segment.bodyid() );
          segnum.push_back( segment.segment_number() );
          surfid.push_back( segment.surfaceid() );
          frameid.push_back( segment.frameid() );
          segtype.push_back( segment.dtype() );
          classtype.push_back( segment.dclass() );
          nvertices.push_back( segment.n_vectors() );
          nfacets.push_back( segment.n_plates() );
          minradius.push_back( segment.minimum_radius() );
          maxradius.push_back( segment.maximum_radius() );
        }

        config.add_metadata( psmrts::ProductOption( "dsk_data_type",     "double" ) );
        config.add_metadata( psmrts::ProductOption( "dsk_segment_index", segnum ) );
        config.add_metadata( psmrts::ProductOption( "dsk_surface_id",    surfid ) );
        config.add_metadata( psmrts::ProductOption( "dsk_frame_id",      frameid ) );
        config.add_metadata( psmrts::ProductOption( "dsk_body_id",       bodyid) );
        config.add_metadata( psmrts::ProductOption( "dsk_segment_type",  segtype ) );
        config.add_metadata( psmrts::ProductOption( "dsk_class_type",    classtype ) );
        config.add_metadata( psmrts::ProductOption( "n_vertices",        nvertices) );
        config.add_metadata( psmrts::ProductOption( "n_facets",          nfacets ) );
        config.add_metadata( psmrts::ProductOption( "maximum_radius",    maxradius ) );
        config.add_metadata( psmrts::ProductOption( "minimum_radius",    minradius ) );

        return ( config );

      }


    protected:


      /** Return a reference to the NAIF SpiceDSKDescr for the NAIF DSK file */
      inline const SharedDskDescriptor &dskdsc() const {
        return ( m_dsk_descriptor );
      }

      /** Return reference to shared mutex for strategic NAIF file I/O locking. See DskSegment.hpp */
      inline std::mutex &mutex() const {
        return ( dskdsc().mutex() );
      }      

      /** Return reference to NAIF DSK kernel descriptor. See DskSegment.hpp */
      inline const KernelDescriptor &kernel() const {
        return ( dskdsc().datum() );
      }

    private:
      SharedDskDescriptor             m_dsk_descriptor;
      DskSegmentList                  m_segments;
      size_t                          m_total_plates;
      size_t                          m_total_vertices;
      Eigen::Vector3d                 m_radii;
      psmrts::PsmrtsThreadSafeCounter m_tracker;     // Tracks times and copy counts


      /** Reset DSK model to default state */
      inline void reset( const SharedDskDescriptor *dskdsc = nullptr ) {
        if ( dskdsc != nullptr ) {
          m_dsk_descriptor = *dskdsc;
        }
        else {
          m_dsk_descriptor = SharedDskDescriptor();
        }

        m_segments.clear();
        m_total_plates   = 0;
        m_total_vertices = 0;
        m_radii          = Eigen::Vector3d::Zero();
        m_tracker        = psmrts::PsmrtsThreadSafeCounter();

        return;
      }




      /**
       * @brief Initialize the DSK using the kernel descriptpr
       * 
       * See https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/dskgd_c.html.
       * 
       * @param k_descr Unique NAIF kernel descriptor to initialize object instance
       */
      inline void init( const SharedDskDescriptor &k_descr ) {
        ZoneScoped;
       
        // Completely reset with this descriptor
        reset( &k_descr );

        // Check to ensure the file is open
        if ( !k_descr.datum().found() ) {
          std::string mess = "*** ERROR - [naif::DskKernelModel] DSK file " + k_descr.datum().filename() + " is not valid or open";
          throw std::runtime_error( mess );
        }

        // Lock up NAIF file I/O for thread safety ( >=c++17 )
        std::scoped_lock mylocker( this->mutex() );

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

      /** Intialize with an opened DSK file and single segment from that DSK */
      inline void init( const DskKernelModel &model ) {
        m_dsk_descriptor = model.m_dsk_descriptor;
        m_segments       = model.m_segments;
        m_total_plates   = model.m_total_plates;
        m_total_vertices = model.m_total_vertices;
        m_radii          = model.m_radii;
        m_tracker        = psmrts::PsmrtsThreadSafeCounter();
        return;
      }

      /** Intialize with an opened DSK file and single segment from that DSK */
      inline void init( const SharedDskDescriptor &kdescr, 
                        const DskSegment &segment ) {
        reset( &kdescr );
        add_segment( segment );
      }

      /** Open or share a DSK file using a thread-safe secure procedure */
      inline void init( const std::string &dskfile ) {
        init( DskKernelModel::get_dsk_shape( dskfile ) );
        return;
      }

    // This class maintains its own inventory. Here is the implementation of
    // that API
    private:
      typedef std::map<std::string, DskKernelModel>   DskShapeInventory;
      inline static std::mutex        s_mutex = {};
      inline static DskShapeInventory s_dsk_shape_inventory = {};

    public:

      inline static bool has_dsk_shape( const std::string &dskfile ) {
        ZoneScoped;
        std::scoped_lock mylocker( s_mutex );  
        auto dsk = s_dsk_shape_inventory.find( dskfile );
        return ( dsk != s_dsk_shape_inventory.end() );        
      }

      inline static DskKernelModel get_dsk_shape( const std::string &dskfile ) {
        ZoneScoped;
        std::scoped_lock mylocker( s_mutex );  
        auto dsk = s_dsk_shape_inventory.find( dskfile );
        if ( s_dsk_shape_inventory.end() == dsk ) {
          DskKernelModel dskmodel( KernelFileSystem::get_shared_descriptor( dskfile ) );
          auto dsk_result = s_dsk_shape_inventory.insert_or_assign( dskfile, dskmodel );
          dsk = dsk_result.first;
        }

        return ( dsk->second );        
      }      

      inline static DskKernelModel get_dsk_shape_with_id( const std::string &dskfile, const int id ) {
        return ( get_dsk_shape( dskfile ).create_from_id( id ) );
      }      


      inline static bool remove_dsk_shape( const std::string &dskfile ) {
        std::scoped_lock mylocker( s_mutex );  
        auto dsk = s_dsk_shape_inventory.find( dskfile );
        if  ( s_dsk_shape_inventory.end() != dsk ) {
          s_dsk_shape_inventory.erase( dskfile );
          KernelFileSystem::safe_disposal_of( dskfile );
          return ( true );
        }
        return ( false );
      }

      inline static std::vector<std::string> get_dsk_shape_inventory_list() {
        std::vector<std::string> v_dsk_files;
        std::scoped_lock mylocker( s_mutex ); 
        for ( const auto &dsk : s_dsk_shape_inventory ) {
          v_dsk_files.push_back( dsk.first );
        } 
        return ( v_dsk_files );
      }

      /**
       * @brief Resets the DSK file inventory
       * 
       * This method empties the maintained DSK shared inventory. You may
       * want to do this periodically but certainly must be done when
       * a kclear_c() is called.
       * 
       * Note that the entire NAIF SPICE Kernel pool is *NOT* reset by this
       * function. That will result in Very Bad Things and break existing
       * instances of DSK kernel models.
       * 
       * Essentially, calling this and then reopening a new DSK without
       * clearing the kernel pool (via naif::initKernelSystem( true) ) will
       * retrieve a KernelDescriptor for the file, open it if needed,
       * reread the DSK segments in the file, and then add that DskKernelModel
       * instance to the DSK shape inventory, thus starting over.
       * 
       * This management is critical in testing as each Catch2 TEST_CASE must
       * reset both the DSK kernel and NAIF SPICE kernel pool systems.
       * 
       * @history 2024-03-04 Kris J. Becker Original Version
       */
      inline static void reset_dsk_system( ) {
        std::scoped_lock mylocker( s_mutex );  
        s_dsk_shape_inventory.clear();
        return;
      }

  };

} // namespace naif

#endif
