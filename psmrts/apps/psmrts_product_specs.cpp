/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <iostream>
#include <string>
#include <vector>

#include <Eigen/Geometry>

#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/PsmrtsRequest.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/algorithms/VariantTraits.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>


inline void usage(int argc, char *argv[] ) {
  std::cout << "\nUsage: " << argv[0] << " - display PMRST product specifications interface" << std::endl;
  std::cout << "\n  Paramaters: -h/--usage      Display this help/usage text and exit" << std::endl;
  std::cout << "              --index <n>     Number of spaces to indec JSON text [default:1]" << std::endl;
  std::cout << "                                -1 is no spaces (compact), 0 is left aligned, 1 is one space, etc..." << std::endl;
  std::cout << "              --tracers       Lists all tracers only" << std::endl;
  std::cout << "              --shapes        Lists all shapes only" << std::endl;
  std::cout << "              name [name...]] Additional arguments list specific products." << std::endl;
  return;
}


/**
 * @brief Main function of the psmrts_capi_features application.
 *
 * This application serves as a demonstration of the Bullet shape tracing sytem
 * in the PSMRTS library.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of strings representing command line arguments.
 * @return Integer indicating exit status of the application. Returns 0 upon
 *         successful execution, non-zero otherwise.
 */
int main( int argc, char *argv[] ) {


// Parse the arguments. This app can be run with no arguments!
  std::vector<std::string> products;
  int j_indent   = 1;
  bool do_all    = true;
  bool do_tracers = false;
  bool do_shapes = false;

  size_t nth = 1;
  for ( ; nth < argc ; nth++ ) {
    std::string arg_s( argv[nth] );
    if ( '-' == arg_s[0] ) {

      if ( ("-h" == arg_s ) || ( "--usage" == arg_s ) ) {
        usage( argc, argv );
        return ( 1 );
      }
      else if ( "--index" == arg_s ) {
        if ( ++nth >= argc ) {
          std::cerr << "--index must provide an integer argument!" << std::endl;
          return ( 2 );
        }
        size_t pos{};
        std::string indent_s = argv[nth];
        j_indent = std::stoi( indent_s, &pos );
        if ( indent_s.length() != pos ) {
          std::cerr << "--indent \"" << indent_s <<"\" is invalid, must be an integer!" << std::endl;
          return ( 3 );
        }
      }
      else if ( "--tracers" == arg_s ) {
        do_tracers = true;
        do_all = false;
      }
      else if ( "--shapes" == arg_s ) {
        do_shapes = true;
        do_all = false;
      }
      else {
        std::cerr << "***Error - invalid/unrecognized program option: " << arg_s << std::endl;
        return ( 4 );
      }
    }
    else {
      products.push_back( arg_s );
    }
  }

  psmrts::PsmrtsContainer<psmrts::ProductSpecification> products_v("products");
  
  psmrts::ProductMaker<psmrts::PsmrtsShape> shape_m;
  ordered_json shapes_j = ordered_json::array();
  for ( const auto &s : shape_m.get_product_specs() ) {
    ordered_json s_j;
    s_j = psmrts::json_utils::insert_object( s.name(), s.to_json() );
   shapes_j.push_back(  s_j );
  }

  psmrts::ProductMaker<psmrts::PsmrtsTracer> tracer_m;
  ordered_json tracers_j = ordered_json::array();
  for ( const auto &t : tracer_m.get_product_specs() ) {
    ordered_json t_j;
    t_j = psmrts::json_utils::insert_object( t.name(), t.to_json() );
     tracers_j.push_back( t_j );
  }

  ordered_json specs_j = ordered_json::object();
  specs_j["shapes"]  = shapes_j;
  specs_j["tracers"] = tracers_j;

  std::cout << specs_j.dump(1) << std::endl;
  
  return ( 0 );
}
