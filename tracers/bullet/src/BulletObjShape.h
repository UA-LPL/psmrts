#ifndef BulletObjShape_h
#define BulletObjShape_h
/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/08/25 01:37:55 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QString>

#include "BulletDskShape.h"

// This simply defines the use of the OBJ loader
#define TINYOBJLOADER_USE_DOUBLE
#include "tiny_obj_loader.h"

namespace Isis {

/**
 * Bullet OBJ loader class
 *  
 * This class derives from BulletDskShape which is well suited to handle the 
 * structures needed by the OBJ loader. 
 *  
 * @author 2019-10-03 Kris Becker 
 * @internal 
 *  @history 2020-01-14 Kris Becker - Moved writeOBJ() to this class from
 *                          BulletDskShape
 *  @history 2020-01-21 Kris Becker - Added setting of name in constructors 
 */
  class BulletObjShape : public BulletDskShape {
    public:
      BulletObjShape();
      BulletObjShape(const QString &objfile);
      BulletObjShape(const Pvl *conf);
      BulletObjShape(const QString &objfile, const int nparts, const bool debug = false);
      explicit BulletObjShape(const QString &objfile, const Pvl *conf);
      virtual ~BulletObjShape();

      // Custom OBJ reader 
      virtual void loadMeshFile(const QString &objfile, const Pvl *conf);
      virtual void writeOBJ(const QString &objFile) const;

    private:
      typedef struct {
        std::vector<double> s_vertices;
        std::vector<int>    s_indices;
        double minx, maxx;
        double miny, maxy;
        double minz, maxz;
      } TinyMesh;

      static void vertex_cb(void *user_data, tinyobj::real_t x, tinyobj::real_t y, 
                            tinyobj::real_t z, tinyobj::real_t w);
      static void index_cb(void *user_data, tinyobj::index_t *indices, int num_indices);
  };

} // namespace Isis

#endif

