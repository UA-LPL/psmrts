#ifndef PsmrtsTracer_hpp
#define PsmrtsTracer_hpp

#include <exception>
#include <string>
#include <variant>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsRayTrace.hpp>
#include <PsmrtsRequest.hpp>
#include <ProductProcessDispatch.hpp>

#include <PsmrtsPLYFormat.hpp>

#include <BulletTracer.hpp>
#include <EllipsoidTracer.hpp>
#include <NaifDskTracer.hpp>

namespace psmrts {

  class PsmrtsTracer : public ProductProcessDispatch< MissingProcessRequestHandler, EllipsoidTracer, BulletTracer, NaifDskTracer> {
    public:
      using Tracer = ProductProcessDispatch::ProductType;

      PsmrtsTracer( )  {  }
      PsmrtsTracer( const Tracer &tracer ) : ProductProcessDispatch( tracer ) {  }
      virtual ~PsmrtsTracer() { }

      inline static PsmrtsTracer sphere( const double radius_km, const std::string &name="sphere" ) {
        Eigen::Vector3d radii( { radius_km, radius_km, radius_km } );
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ) ) );
      }

      inline static PsmrtsTracer spheroid( const double a_km, const double c_km, 
                                                const std::string &name="spheroid" ) {
        Eigen::Vector3d radii( { a_km, a_km, c_km} ); // testing
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ) ) );
      }

      inline static PsmrtsTracer ellipsoid( const double a_km,  const double b_km, const double c_km, 
                                                const std::string &name="ellipsoid" ) {
        Eigen::Vector3d radii( { a_km, b_km, c_km } );
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ) ) ); 
      }

      inline static PsmrtsTracer ellipsoid( const Eigen::Vector3d radii, 
                                                const std::string &name="ellipsoid" ) {
        return ( PsmrtsTracer( EllipsoidTracer( radii, name ) ) ); 
      }

      inline static PsmrtsTracer bullet( const std::string &meshfile ) {

        std::string fext_t = psmrts_tolower( psmrts_file_extension( meshfile ) );
        if ( "obj" == fext_t ) {
          return ( PsmrtsTracer( BulletTracer( psmrts::bullet::PsmrtsBulletWorldModel( psmrts::bullet::PsmrtsBulletMeshMap ( psmrts::PsmrtsOBJFormat( meshfile ) ), meshfile ) ) ) );
        }
        else if ( "ply" == fext_t ) {
          PsmrtsMeshData mesh_t(  PsmrtsPLYFormat( meshfile ).get_mesh() );
          psmrts::bullet::PsmrtsBulletMeshMap mesh_b( mesh_t, meshfile, 0 );
          psmrts::bullet::PsmrtsBulletWorldModel bt_world( mesh_b, meshfile );
          return ( PsmrtsTracer( BulletTracer( bt_world ) ) );
        }
        else {  // ( "dsk" == fext_t )
          naif::DskKernelModel dsk( meshfile );
          PsmrtsMeshData mesh_t( dsk.load_facet_indexes(), dsk.load_facet_vectors() );
          psmrts::bullet::PsmrtsBulletMeshMap mesh_b( mesh_t, meshfile, 0 );
          psmrts::bullet::PsmrtsBulletWorldModel bt_world( mesh_b, meshfile );
          return ( PsmrtsTracer( BulletTracer( bt_world ) ) );           
        }

        // Likely won't reach here...
        return ( PsmrtsTracer( MissingProcessRequestHandler( "PsmrtsTracer" ) ) );
      }

      inline static PsmrtsTracer naifdsk( const std::string &dskfile ) {
        return ( PsmrtsTracer( NaifDskTracer( dskfile ) ) ); 
      }

    };

} // namespace psmrts

#endif
