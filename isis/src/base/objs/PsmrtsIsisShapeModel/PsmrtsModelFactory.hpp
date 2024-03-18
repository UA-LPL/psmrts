#ifndef PsmrtsModelFactory_h
#define PsmrtsModelFactory_h

#include <QMap>
#include <QMutex>
#include <QSharedPointer>

#include "Pvl.h"
#include "PvlKeyword.h"
#include "PvlGroup.h"
#include "PvlFlatMap.h"

#include <RayTrace.hpp>
#include <PsmrtsUtilities.hpp>
#include <PsmrtsTracerModel.hpp>
#include <PsmrtsShapeTracerAdapter.hpp>
#include <PsmrtsPriorityTracer.hpp>
#include <NaifEllipsoidShape.hpp>
#include <DskKernelModel.hpp>

namespace Isis {

class Target;

/**
 * @brief Class for managing the construction and destruction of 
 *        PSMRTS shape models
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
 *   @histort 2021-05-03 Kris Becker - Added priority parameter to
 *                          create_world() method (results in API change!).
 */
  class PsmrtsModelFactory {
    public:
      typedef psmrts::PsmrtsShapeTracerAdapter<naif::DskKernelModel> PsmrtsAdaptedDskShape;

      static PsmrtsModelFactory *getInstance();

      inline static psmrts::PsmrtsPriorityTracer make_tracer() {
        return ( psmrts::PsmrtsPriorityTracer() );
      }


      inline static PvlFlatMap extract_shapemodel_list( const QString &shapefile ) {

        // IF the shapefile is not valid, throw an execption right now!
        if ( shapefile.isEmpty() ) {
          throw IException(IException::User, "No valid shapefile provided!", _FILEINFO_);
        }
        
        // Check for config option. Otherwise the default condition is to
        // construct the mode with existing conditions, which may include loading
        // a new shape from a file
        FileName sfile(shapefile);
        PvlFlatMap shpkey;
        shpkey.add("ShapeModel", shapefile);
        if ("conf" == sfile.extension().toLower() ) {
          Pvl pConf(shapefile);
          PvlFlatMap pconfmap = PvlFlatMap(pConf);
          if ( pconfmap.exists("ShapeModel") ) shpkey.add(pconfmap.keyword("ShapeModel"));
        }

        return ( shpkey );
      }

      inline static psmrts::PsmrtsPriorityTracer create_priority_tracer( const QString &shapeFile, 
                                                                         const PvlFlatMap &params,
                                                                         Target *target, Pvl &pvl,
                                                                         PvlContainer &kernel_group ) {

        psmrts::PsmrtsPriorityTracer s_tracer = PsmrtsModelFactory::make_tracer();
        PvlFlatMap xfile_parm = PsmrtsModelFactory::extract_shapemodel_list( shapeFile );

        QStringList xfiles = xfile_parm.allValues( "ShapeModel" );
        for ( auto const &fname : xfiles ) {
          s_tracer.add_tracer( new PsmrtsAdaptedDskShape( FileName( fname ).expanded() )) );
        }

        // Do this here, otherwise default behavior will ensue from here on out
        kernel_group.addKeyword(PvlKeyword("RayTraceEngine", "PSMRTS"), PvlContainer::Replace);

        return ( s_tracer );        
      }

};

#endif