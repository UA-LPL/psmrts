#ifndef PsmrtsBulletFactory_h
#define PsmrtsBulletFactory_h

#include <string>
#include <memory>
#include <exception>
#include <mutex>

#include <Eigen/Geometry>

#include <PsmrtsUtilities.hpp>
#include <BulletSystemModel.hpp>
#include <PsmrtsBulletMeshMap.hpp>
#include <PsmrtsBulletWorldModel.hpp>
#include <PsmrtsBulletTracerModel.hpp>
#include <PsmrtsFactory.hpp>

namespace psmrts::bullet {

/**
 * @brief Class for managing the construction and destruction of 
 *        BulletWorldManagers
 * 
 * This class is a singleton that constructs and holds BulletWorldMangers. Its 
 * intent is to provide a single instance of a BulletShapeModel to many cubes 
 * that refer to the same model. These models typically are large and may 
 * require significant memory/resources. Unique instances of these models for
 * applications (e.g., qview, qmos, jigsaw, etc...) that open many cubes with 
 * Bullet shapes at a time will quickly consume resources and lead to failures. 
 * 
 * When an BulletWorldManager is requested the cache will first be checked if 
 * one for that file already exists. If it exists in the cache, a shared 
 * reference to the existing BulletWorldManger is returned. If one does not 
 * exist yet, a new BulletWorldManger is created on the heap. Reference counting 
 * is presumed to be managed in BulletWorldManager. 
 *  
 * It follows that when creating a BulletShapeModel, a BulletWorldManager is 
 * also created and added to the cache. The pointers returned by the create
 * methods are owned by the caller and can safely be deleted without affecting 
 * the cache. 
 *  
 * When creating BulletWordManagers, you can force the creation of a new one in 
 * the API. This may become necessary as geometry operations can slow down if 
 * all are using the same instance of the a Bullet shape. Creating a new world 
 * with the same shape file will replace the existing one with a new instance. 
 * All existing instances are unaffected. Subsequent requests for the same shape 
 * file will use the newly create one providing better control over balancing 
 * the load. 
 *  
 * One can explicitly remove a Bullet world object at any time wihthout 
 * affecting existing instances. However, subsequent requests will create new 
 * instance if they don't exist. Upon application exit, they are removed if not 
 * explicity removed. 
 * 
 * @author 2018-07-21 UA/OSIRIS-REx IPWG Team 
 * @internal 
 *   @history 2018-07-21 UA/OSIRIS-REx IPWG Team  - Original Version.
 *   @history 2018-09-14 UA/OSIRIS-REx IPWG Team - Improved implementation;
 *                          return a BulletShapeModel as well as a
 *                          BulletWorldManager which is the fundamental shared
 *                          component.
 *   @history 2019-10-05 Kris Becker - Added model parameter argument to create
 *                          methods.
 *   @history 2021-04-22 Kris Becker - Modified how conf files are
 *                          handled/loaded by creating a world for each tile and
 *                          use threading to speed things up.
 *   @history 2021-04-24 Kris Becker - Added threading of tile load operations;
 *                          changed the API to improve consistency.
 *   @history 2021-05-03 Kris Becker - Added priority parameter to
 *                          create_world() method (results in API change!).
 */
  class PsmrtsBulletFactory : public PsmrtsFactory {
    public:

      PsmrtsBulletFactory() { 
        init_bullet_factory( );
      }

      virtual ~PsmrtsBulletFactory() { }


      static bool safe_removeal_of( const std::string &tracer ) {

        // Lock up inventory access for thread safety ( >=c++17 )
        std::scoped_lock mylocker( s_mutex );           

        // Check to see if it exists and unload only if there are no references
        auto kern = s_kernel_inventory.find( kfile );
        if ( kern != s_kernel_inventory.end() ) {
          if ( kern->second.use_count() == 1 ) {
            s_kernel_inventory.erase( kern );
            PsmrtsBulletFactory::close_kernel( kfile );
            return ( true );
          }
        }
        else {
          auto file_info = naif::PsmrtsBulletFactory::kernel_info( kfile );
          if ( file_info.found() == true ) {
            PsmrtsBulletFactory::close_kernel( kfile );
            return ( true );
          }
        }
        // returns find status
        return ( false );
      }

    private:

      inline void init_bullet_factory( ) {

      }


      
      typedef std::map<std::string, PsmrtsBulletTracerModel> ModelInventory;
      inline static std::mutex      s_mutex = { };
      inline static ModelInventory  s_tracer_inventory =  { };
  };

};

#endif