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
#include <psmrts/core/PsmrtsJson.hpp>
#include <psmrts/core/PsmrtsContainer.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>

/** Usage function for the application */
inline void usage(int argc, char *argv[] ) {
  std::cout << "\nUsage: " << argv[0] << " - display PMRST product specifications interface" << std::endl;
  std::cout << "\n  Paramaters: -h/--usage      Display this help/usage text and exit" << std::endl;
  std::cout << "              --indent <n>    Number of spaces to indent JSON text [default:1]" << std::endl;
  std::cout << "                                -1 is no spaces (compact), 0 is left aligned, 1 is one space, etc..." << std::endl;
  std::cout << "              --tracers       Lists all tracers only" << std::endl;
  std::cout << "              --shapes        Lists all shapes only" << std::endl;
  std::cout << "              name [name...]] Additional arguments list specific products." << std::endl;
  return;
}


/**
 * @brief Main function of the psmrts_product_specs application.
 *
 * This application provides a list of all available shape and tracer products
 * and the user configurable options and driver configurations. The "features"
 * sections contains the list of available options that can be supplied when
 * creating a particular product. 
 * 
 * Output is generated in JSON format where the "--indent" option controls the
 * JSON spacing on output. A value of -1 produces a compact JSON string with no
 * indention. Only shapes (--shapes) or tracers (--tracers) can be listed. Users
 * can also select specific products by name as optional arguments to the app.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of strings representing command line arguments.
 * @return Integer indicating exit status of the application. Returns 0 upon
 *         successful execution, non-zero otherwise.
 * @author 2026-02-20 Kris J. Becker - Univerity of Arizona
 * @history2026-02-20 Kris J. Becker Original verison
 */
int main( int argc, char *argv[] ) {
  using SpecsList = psmrts::PsmrtsContainer<psmrts::ProductSpecification>;

// Parse the arguments. This app can be run with no arguments!
  std::vector<std::string> products;
  int j_indent   = 1;
  bool do_all     = true;
  bool do_tracers = false;
  bool do_shapes  = false;
  bool got_list   = false;

  // Parse the input parameters from the user
  size_t nth = 1;
  for ( ; nth < argc ; nth++ ) {
    std::string arg_s( argv[nth] );
    if ( '-' == arg_s[0] ) {

      if ( ("-h" == arg_s ) || ( "--usage" == arg_s ) ) {
        usage( argc, argv );
        return ( 1 );
      }
      else if ( "--indent" == arg_s ) {
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
      products.push_back( psmrts::psmrts_tolower( arg_s ) );
      got_list = true;
    }
  }


  // Keep track of found products
  std::vector<std::string> found_products;

  // List of found shapes
  SpecsList shape_v;
  ordered_json shapes_j = ordered_json::array();

  if ( do_all || do_shapes ) {
    shape_v = psmrts::ProductMaker<psmrts::PsmrtsShape>().get_product_specs(); 
    for ( const auto &s : shape_v ) {
      ordered_json s_j;
      if ( got_list ) {
        if ( psmrts::psmrts_contains_string( s.name(), products ) ) {
          found_products.push_back( s.name() );
          s_j = psmrts::json_utils::insert_object( s.name(), s.to_json() );
          shapes_j.push_back(  s_j );
        }
      }
      else {
        s_j = psmrts::json_utils::insert_object( s.name(), s.to_json() );
        shapes_j.push_back(  s_j );
      }
    }
  }
  
  // List of found tracers
  SpecsList tracer_v;
  ordered_json tracers_j = ordered_json::array();

  if ( do_all || do_tracers ) {
    tracer_v = psmrts::ProductMaker<psmrts::PsmrtsTracer>().get_product_specs();
    for ( const auto &t : tracer_v ) {
      ordered_json t_j;
      if ( got_list ) {
        if ( psmrts::psmrts_contains_string( t.name(), products ) ) {
          found_products.push_back( t.name() );
          t_j = psmrts::json_utils::insert_object( t.name(), t.to_json() );
          tracers_j.push_back( t_j );
        }
      }
      else {
        t_j = psmrts::json_utils::insert_object( t.name(), t.to_json() );
        tracers_j.push_back( t_j );
      }
    }    
  }

  // Check if list is satisfied list
  if ( got_list ) {
    size_t nerrs = 0;
    for ( const auto &s : products ) {
      if ( !psmrts::psmrts_contains_string( s, found_products ) ) {
        std::cerr << "*** Error - product " << s << " in requestd list not found!" << std::endl;
        nerrs++;
      }
    }
    if ( nerrs > 0 ) return ( 5 );
  }

  ordered_json specs_j = ordered_json::object();
  if ( shapes_j.size() > 0)  specs_j["shapes"]   = shapes_j;
  if ( tracers_j.size() > 0) specs_j["tracers"] = tracers_j;

  if (specs_j.size() == 0 ) {
    std::cerr << "*** Error No products found!" << std::endl;
    return ( 6 );
  }

  // Report the results!
  std::cout << specs_j.dump( j_indent ) << std::endl;
  
  return ( 0 );
}
