#ifndef BulletSystemModel_h
#define BulletSystemModel_h

/**
 * @brief Bullet Physics ISIS-specific include file 
 *  
 * This file *must* be included before any other Bullet includes because PSMRTS
 * uses some very specific configuration/build settings in the version of Bullet 
 * we are using. 
 *  
 * First, we require Bullet Physics to be built with the BtScalar type set to
 * double precision. The Bullet build settings in the CMake system explicitly 
 * sets the BtScalar type to double via a command line MACRO definition. This 
 * file enforces the proper Bullet environment for use in the ISIS system. 
 *  
 * We also check for compatibility with the Bullet 3 (B3*) environment but do 
 * not include the environment. Only the BT* environment is invoked for 
 * consistency. Include the B3 includes anywhere after this file to use that 
 * API. 
 *  
 * @author 2017-03-16 Kris Becker - U.S. Geological Survey
 * @internal 
 * @history 2017-03-16 Kris Becker Original Version 
 * @history 2024-05-16 Kris J. Becker - University of Arizona, OSIRIS-APEX
 *                        Modified from UA/ISIS and used in PSRMTS
 */

#if defined(BT_SCALAR_H) || defined(B3_SCALAR_H)
#if !defined(BT_USE_DOUBLE_PRECISION) && !defined(B3_USE_DOUBLE_PRECISION) 
#error "*** You must include BulletSystemModel.hpp prior to any Bullet includes ***"
#endif
#endif

// PSMRTS using the Bullet library requires double precision!! This sets the 
// bullet::btScalar type to double precision throughout the Bullet API. 
#define   BT_USE_DOUBLE_PRECISION   1 
#define   B3_USE_DOUBLE_PRECISION   1 

// Go ahead and include all of Bullet here so the define will be applied 
// universally. 
#include <btBulletDynamicsCommon.h>

namespace psmrts::bullet {
  /** Namespace for Bullet specific routines   */

  /**
   * Maximum number of parts/object
   * 
   * @return @b int The maximum number of parts allowed in each collision objects.
   */
  inline size_t bt_MaxBodyParts() {
  return ( 1<<MAX_NUM_PARTS_IN_BITS );
  }


  /**
   * Maximum number of triangles/part
   * 
   * @return @b long The maximum number of triangles allowed in each collision objects.
   */
  inline size_t bt_MaxTrianglesPerPart() {
  return ( 1<<(31-MAX_NUM_PARTS_IN_BITS) );
  }


  /**
   * Maximum number of parts/object in the Bullet 3 API
   * 
   * @return @b int The maximum number of parts allowed in each collision objects.
   */
  inline size_t b3_MaxBodyParts() {
  return ( bt_MaxBodyParts() );
  }


  /**
   * Maximum number of triangles/part in the Bullet 3 API
   * 
   * @return @b long The maximum number of triangles allowed in each collision objects.
   */
  inline size_t b3_MaxTrianglesPerPart() {
  return ( bt_MaxTrianglesPerPart() );
  }

  inline PHY_ScalarType bt_type_code( const double *bt_t ) {
    return ( PHY_DOUBLE );
  }

  inline PHY_ScalarType bt_type_code( const float *bt_t ) {
    return ( PHY_FLOAT );
  }

  inline PHY_ScalarType bt_type_code( const int *bt_t ) {
    return ( PHY_INTEGER );
  }

} // psmrts::bullet
#endif
