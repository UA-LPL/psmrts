/** This is free and unencumbered software released into the public domain.
 
      OSIRIS-APEX (OSIRIS-APophis EXplorer) by University of Arizona 
                        is marked with CC0 1.0
      (https://creativecommons.org/publicdomain/zero/1.0/legalcode.en)
 
The authors of PSMRTS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/
 
/* SPDX-License-Identifier: CC0-1.0 */

#include <string>

#include "../EllipsoidTracer.hpp"
#include <psmrts/core/products/ProductCart.hpp>

#include <cspice/SpiceUsr.h>


namespace psmrts  {
 

  bool EllipsoidTracer::ray_trace( PsmrtsRayTrace &ray ) const {

    // Let the model do it!
    ray.datum().m_hit = this->ray_trace( ray.observer().data(), 
                                            ray.lookdir().data(), 
                                            ray.datum().m_xyz.data(), 
                                            ray.datum().m_normal.data() );
    return ( ray.hasHit() );
  }

  bool EllipsoidTracer::ray_trace( const double *observer, 
                                 const double *lookdir,
                                 double *xyz, 
                                 double *normal ) const {

      SpiceBoolean found;
      (void) surfpt_c( observer, lookdir,
                        m_radii[0], m_radii[1], m_radii[2],
                        xyz, &found );

      bool hasHit = ( SPICETRUE == found );
      if (hasHit ) {
        this->compute_normal( xyz, normal );
      }

      // Returns intercept state
      return ( hasHit );
    }


  /** Retuns the vector normal of the input point */
  void EllipsoidTracer::compute_normal( const double *point, double *normal ) const {  
    (void) surfnm_c( m_radii[0], m_radii[1], m_radii[2], point, normal );
    return;
  }

  void EllipsoidTracer::create( const ProductCart &cart ) {

      std::string name_t = cart.configuration().name();


      // Check for valid shape type
      if ( cart.error_count() > 0 ) {
        std::string mess = "EllipsoidTracer::create(" + name_t + 
                          ") has config/spec processing errors: \n" +
                            cart.errors_to_string();
        throw std::runtime_error( mess );          
      }

      if ( !cart.isvalid() ) {
        std::string mess = "EllipsoidTracer::create(" + name_t + 
                          ") is invalid with " + 
                          std::to_string( cart.configuration().size() ) +
                          " config options and " +
                          std::to_string( cart.residual().size() ) +
                          " residual options";
        throw std::runtime_error( mess );          
      }

      ProductConfiguration v_conf = cart.configuration();
      std::string model = "ellipsoid";

      if ( v_conf.contains( "tracer" ) ) {
        model = v_conf.find( "tracer" ).to_string();
        std::vector<std::string> valid_s = { "ellipsoid", "spheroid", "sphere" };
        if ( std::find( valid_s.begin(), valid_s.end(), model) == valid_s.end() ) {
          std::string mess = "EllipsoidTracer::create() - tracer must be "
                             "\"ellipsoid\", \"spheroid\" or \"sphere\" "
                              " but found " + model;
          throw std::runtime_error( mess );
        }
      }

      std::vector<double> radii = OptionDoublesExtractor( v_conf.find( "radii" ) ).get_all();
      if ( ( radii.size() < 1 ) || (radii.size() > 3 ) ) {
        std::string mess = "EllipsoidTracer::create() - radii must have 1, 2 or 3 values"
                           " but got " + std::to_string( radii.size() );
        throw std::runtime_error( mess );
      }

      std::string name = name_t;
      if ( v_conf.contains( "name" ) ) {
        name = v_conf.find( "name" ).to_string();
      }

      if ( radii.size() == 1 ) {
        m_radii[0]  = radii[0];
        m_radii[1]  = radii[0];
        m_radii[2]  = radii[0];
        m_config = init_config( name, { radii[0] }, model );
      }
      else if ( radii.size() == 2 ) {
        m_radii[0]  = radii[0];
        m_radii[1]  = radii[0];
        m_radii[2]  = radii[1];
        m_config = init_config( name, { radii[0], radii[1] }, model);
      }
      else {
        m_radii[0]  = radii[0];
        m_radii[1]  = radii[1];
        m_radii[2]  = radii[2];
        m_config = init_config( name, { radii[0], radii[1], radii[2] }, model);
      }        

    }

} // namespace psmrts

