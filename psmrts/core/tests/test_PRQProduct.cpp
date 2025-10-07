#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/PRQProduct.hpp>


TEST_CASE( "PRQProduct Default", "[prq][product][default]") {
    // Error - Says 2 default constructs, need to fix default parameter
    // or accept nuance.
    psmrts::PRQProduct prod("");

    psmrts::ProductConfiguration prod_config = prod.config();
    CHECK( prod_config.name() == "" );
    CHECK( prod_config.size() == 0 );

    psmrts::ProductConfiguration config( "multi", { psmrts::ProductOption( "tracer", "bullet"), 
                                                    psmrts::ProductOption("obj_file", "l_00050mm_alt_ptm_5595n04217_v020.obj") 
                                                  } );
    psmrts::PRQProduct prod2("test", config);
    psmrts::ProductConfiguration prod2_config = prod2.config();
    CHECK( prod2_config.name() == config.name() );
    CHECK( prod2_config.size() == config.size() );

    psmrts::PsmrtsInventory inv;
    psmrts::PsmrtsTracer t_sphere;
    psmrts::PsmrtsTracer t_ellipse = psmrts::PsmrtsTracer::ellipsoid(1.0, 2.0, 3.0);
    inv.tracers().add_product(t_sphere);

    psmrts::PRQProduct prod3("test2", inv);
    CHECK( prod3.add_tracer( t_ellipse ) == true );
    
    psmrts::PsmrtsInventory prod3_inv = prod3.local_inventory();
    CHECK( prod3_inv.tracers().size() == 1 ); // why not 2? 

    std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
    psmrts::ObjShape obj_m( objfile );
    psmrts::PsmrtsShape obj_shape( obj_m );
    CHECK( prod3_inv.shapes().size() == 0 );
    CHECK( prod3.add_shape( obj_shape ) == true );
    prod3_inv = prod3.local_inventory();
    CHECK( prod3_inv.shapes().size() == 0 ); // 1?

    psmrts::PsmrtsInventory global_inv = prod3.product_inventory();
    CHECK( global_inv.shapes().size() == 1 ); // why is this not in local, but in global?
                                              // I added it to the prod3 env specifically?
}
