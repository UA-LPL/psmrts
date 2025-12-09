
#include <psmrts/core/tests/psmrts_catch2_environment.hpp>

#include "../private/BulletSystemModel.hpp"

TEST_CASE ( "Bullet System Info", "[bullet][system]" ) {

  CHECK( MAX_NUM_PARTS_IN_BITS       == 10 );

  CHECK( psmrts::bullet::bt_MaxBodyParts()        == 1024 );
  CHECK( psmrts::bullet::bt_MaxTrianglesPerPart() == 2097152 );

  CHECK( psmrts::bullet::b3_MaxBodyParts()        == 1024 );
  CHECK( psmrts::bullet::b3_MaxTrianglesPerPart() == 2097152 );

  double *d_none = nullptr;
  CHECK( psmrts::bullet::bt_type_code ( d_none ) == PHY_DOUBLE );

  float  *f_none = nullptr;
  CHECK( psmrts::bullet::bt_type_code ( f_none ) == PHY_FLOAT );  

  int  *i_none = nullptr;
  CHECK( psmrts::bullet::bt_type_code ( i_none ) == PHY_INTEGER);  

  // This must be double!
  btScalar *bt_scaler = nullptr;  
  CHECK( psmrts::bullet::bt_type_code ( bt_scaler ) == PHY_DOUBLE );

  CHECK (sizeof( btScalar ) == 8 );
}