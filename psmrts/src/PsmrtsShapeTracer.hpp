#ifndef PsmrtsShapeTracer_hpp
#define PsmrtsShapeTracer_hpp

#include <exception>
#include <string>
#include <variant>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <PsmrtsRayTrace.hpp>
#include <PsmrtsRequest.hpp>
#include <ProductProcessDispatch.hpp>

#include <PsmrtsPLYFormat.hpp>

#include <BulletShapeTracer.hpp>
#include <EllipsoidShapeTracer.hpp>
#include <NaifDskShapeTracer.hpp>

namespace psmrts {

  class PsmrtsShapeTracer : public ProductProcessDispatch< NoProcessRequestHandler, EllipsoidShapeTracer, BulletShapeTracer, NaifDskShapeTracer> {
    public:
      using PsmrtsTracer = ProductProcessDispatch::ProductType;

      PsmrtsShapeTracer( )  {  }
      PsmrtsShapeTracer( const PsmrtsTracer &tracer ) : ProductProcessDispatch( tracer ) {  }
      virtual ~PsmrtsShapeTracer() { }

      inline static PsmrtsShapeTracer sphere( const double radius_km, const std::string &name="sphere" ) {
        Eigen::Vector3d radii( { radius_km, radius_km, radius_km } );
        return ( PsmrtsShapeTracer( EllipsoidShapeTracer( radii, name ) ) );
      }

      inline static PsmrtsShapeTracer spheroid( const double a_km, const double c_km, 
                                                const std::string &name="spheroid" ) {
        Eigen::Vector3d radii( { a_km, c_km, c_km} );
        return ( PsmrtsShapeTracer( EllipsoidShapeTracer( radii, name ) ) ); 
      }

      inline static PsmrtsShapeTracer ellipsoid( const double a_km,  const double b_km, const double c_km, 
                                                const std::string &name="ellipsoid" ) {
        Eigen::Vector3d radii( { a_km, b_km, c_km } );
        return ( PsmrtsShapeTracer( EllipsoidShapeTracer( radii, name ) ) ); 
      }

      inline static PsmrtsShapeTracer ellipsoid( const Eigen::Vector3d radii, 
                                                const std::string &name="ellipsoid" ) {
        return ( PsmrtsShapeTracer( EllipsoidShapeTracer( radii, name ) ) ); 
      }

      inline static PsmrtsShapeTracer bullet( const std::string &meshfile ) {

        std::string fext_t = psmrts_tolower( psmrts_file_extension( meshfile ) );
        if ( "obj" == fext_t ) {
          return ( PsmrtsShapeTracer( BulletShapeTracer( psmrts::bullet::PsmrtsBulletWorldModel( psmrts::bullet::PsmrtsBulletMeshMap ( psmrts::PsmrtsOBJFormat( meshfile ) ), meshfile ) ) ) );
        }
        else if ( "ply" == fext_t ) {
          PsmrtsMeshData mesh_t(  PsmrtsPLYFormat( meshfile ).get_mesh() );
          psmrts::bullet::PsmrtsBulletMeshMap mesh_b( mesh_t, meshfile, 0 );
          psmrts::bullet::PsmrtsBulletWorldModel bt_world( mesh_b, meshfile );
          return ( PsmrtsShapeTracer( BulletShapeTracer( bt_world ) ) );
        }
        else {  // ( "dsk" == fext_t )
          naif::DskKernelModel dsk( meshfile );
          PsmrtsMeshData mesh_t( dsk.load_facet_indexes(), dsk.load_facet_vectors() );
          psmrts::bullet::PsmrtsBulletMeshMap mesh_b( mesh_t, meshfile, 0 );
          psmrts::bullet::PsmrtsBulletWorldModel bt_world( mesh_b, meshfile );
          return ( PsmrtsShapeTracer( BulletShapeTracer( bt_world ) ) );           
        }

        // Likely won't reach here...
        return ( PsmrtsShapeTracer( NoProcessRequestHandler( "PsmrtsShapeTracer" ) ) );
      }

      inline static PsmrtsShapeTracer naifdsk( const std::string &dskfile ) {
        return ( PsmrtsShapeTracer( NaifDskShapeTracer( dskfile ) ) ); 
      }

    };

} // namespace psmrts

#endif