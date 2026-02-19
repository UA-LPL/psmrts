#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include <psmrts/core/products/PRQProduct.hpp>

/**
 * @brief Tests the construction of a PRQProduct using available methods.
 * 
 * This tests the PRQProduct class object. The PRQProduct is designed mainly
 * for creating a PsrmtsPriorityTracer and all its required components. These
 * components consist of shape objects (DSks, OBJ and PLY vertices and indexes)
 * and tracers (ellipsoids, bullet and naifdsk tracer interfaces). The shapes
 * are respresented as PsmrtsShapes and tracers are PsmrtsTracers.
 * 
 * Tests made here construct PRQProducts from a ProductConfiguration and
 * manually by adding shapes and tracers directly. 
 * 
 * The components of a PRQProduct are a list of PsmrtsInventory names that
 * refer to inventories contained in the PsrmtsFactory (typically associated
 * with single file or scene), a local PsmrtsInventory (a private shape/tracer
 * resource excluded from PsmrtsFactory), and a product PsmrtsInventory that
 * is used to create a PsmrtsPriortyTracer.
 * 
 * Tests conducted here ensure internal representations reflect proper
 * construction and internal behavior and maintain processes.
 * 
 * @history 2025-11-10 Kris J Becker
 */
TEST_CASE( "PRQProduct Default", "[prq][product][default]") {
   
  // Default construction.
  psmrts::PRQProduct prod("");

  // Checks configuration for default state.
  psmrts::ProductConfiguration prod_config = prod.config();
  CHECK( prod_config.name() == "" );
  CHECK( prod_config.size() == 0 );

  // Create a product configuration that contains an OBJ shape using a Bullet tracer.
  psmrts::ProductConfiguration config( "multi", { psmrts::ProductOption( "tracer", "bullet"), 
                                                  psmrts::ProductOption("obj_file", "l_00050mm_alt_ptm_5595n04217_v020.obj") 
                                                } );

  // Create a PRQProduct with the above product configuration.                                             
  psmrts::PRQProduct prod2("test", config);

  // Confirm PRQ config matches constructed config.
  psmrts::ProductConfiguration prod2_config = prod2.config();
  CHECK( prod2_config.name() == config.name() );
  CHECK( prod2_config.size() == config.size() );

  // Create an inventory and add an ellipsoid tracer.
  psmrts::PsmrtsInventory inv;
  psmrts::PsmrtsTracer t_ellipsoid = psmrts::PsmrtsTracer::ellipsoid(1.0, 2.0, 3.0);
  inv.tracers().add_product( t_ellipsoid );

  // Construct a new PRQProduct with the tracer inventory created above.
  // NOTE this constructor adds this inventory to the local inventory and not
  // the product inventory. The local inventory will be searched first to
  // resolve config request before the system (factory) inventory is searched
  //  to build the product inventory. This restricts searches to a pool of
  // resources specifed by the user and does not allow/provide for further
  // updates to the local inventory.
  psmrts::PRQProduct prod3("test2", inv);

  // This directly adds a tracer to the product inventory which is
  // separate from the local inventory pool.
  CHECK( prod3.add_tracer( t_ellipsoid ) == true );
  
  // This check ensures the tracer was added to the product inventory
  // and not the local. There should be only 1.
  psmrts::PsmrtsInventory prod3_inv = prod3.local_inventory();
  CHECK( prod3_inv.tracers().size() == 1 ); 

  // Construct an PSMRTS OBJ shape.
  std::string objfile = psmrts_shapes_path( "obj/data/bennu_20facets.obj" );
  psmrts::ObjShape obj_m( objfile );
  psmrts::PsmrtsShape obj_shape( obj_m );

  // Check current state of shapes in local inventory (currently none).
  // Add the OBJ shape to the of the local inventory. NOTE this
  // does not affect the contents of the PRQ because its no longer part
  // of the PRQ object unless you re-add (merge) the copy back into the
  // PRQ via add_inventory() method.
  CHECK( prod3_inv.shapes().size()                   == 0 );
  auto sid = prod3_inv.shapes().add_product( obj_shape );
  CHECK( prod3_inv.shapes().size()                   == 1 );

  // Shows the current product and local inventory shape content which is not
  // affected by the activity above.
  CHECK( prod3.product_inventory().shapes().size()   == 0 );
  CHECK( prod3.local_inventory().shapes().size()     == 0 );

  // Add this copy inventory to the local inventory via a merge.
  prod3.add_inventory( prod3_inv );
  CHECK( prod3.local_inventory().shapes().size() == 1 );

  // Still no shapes in the product inventory.
  psmrts::PsmrtsInventory global_inv = prod3.product_inventory();
  CHECK( global_inv.shapes().size() == 0 );
}
