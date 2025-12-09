#[==[.md
### Add standardized targets that attempts to ensure consistent dependencies

This set of macros attemps to create a standardized set of library target
using interfaces of currently know targets from different package managers
or creating new conformant targets from identified sources.

The main motavtion for this is that when cmake exports the psmrts targets,
any list of librarys specified without targets has the full path exported
to the CMake interface targerts. This is a workaround that is applied
to targets require by PSMRTS and provides a consistent standard for 
targets that vary within well known package managers.

These methods require cmake 3.24 or higher.
#]==]

function(prepend_root_dir ifile rootpath outvar)
  string(FIND "${ifile}" "${rootpath}" _root_pos)
  set(_outpath "${ifile}")
  if(_root_pos EQUAL -1)
    string(PREPEND _outpath "${rootpath}/")
  endif()
  set(${outvar} "${_outpath}" PARENT_SCOPE)
endfunction(prepend_root_dir)


function(get_full_library_path inlibs libpath outlibs)
  # WARNING: This loop is sensitive in that you must use NO_CACHE
  # AND unset the _lib_t variable in order to find all the libraries.
  # Otherwise, it only finds the first library! Repeatedly...
  foreach(_lib IN LISTS inlibs)
    find_library(_lib_t "${_lib}" PATH "${libpath}" NO_CACHE)
    list(APPEND _alllibs ${_lib_t})
    unset(_lib_t) # Must be done or it only finds the first library.
  endforeach()
  set(${outlibs} "${_alllibs}" PARENT_SCOPE)
endfunction(get_full_library_path)

macro(psmrts_add_cspice_target)

  if ( NOT TARGET cspice::cspice )
    if ( TARGET unofficial::cspice::cspice )
      # VCPKG target name
      set(_cspice_target_name unofficial::cspice::cspice )
    elseif( TARGET CSPICE::cspice )
      # Conda target name
      set(_cspice_target_name CSPICE::cspice )
    elseif( TARGET cspice )
      # Conan target name
      set(_cspice_target_name cspice )
    elseif( CSPICE_LIBRARY )
      # Detects a very limited ISIS cspice setup, sets up the cspice target
      # if it doesn't exist.
      if( NOT TARGET cspice )
        add_library(cspice INTERFACE IMPORTED)
        get_filename_component(_libdir  ${CSPICE_LIBRARY} DIRECTORY)
        get_filename_component(_libname ${CSPICE_LIBRARY} NAME)
        set_target_properties(cspice 
          PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CSPICE_INCLUDE_DIR}"
            INTERFACE_LINK_DIRECTORIES    "${_libdir}"
            INTERFACE_LINK_LIBRARIES      "${_libname}"
        )
        unset(_libdir)
        unset(_libname)  
        set(cspice_FOUND TRUE)
        set(CSPICE_FOUND TRUE)
      endif()        
      
      set(_cspice_target_name cspice )
    else()
      message(FATAL_ERROR "cspice library import not found - must be one of "
                         "unofficial::cspice::cspice, CSPICE::cspice or cspice ")
      set(cspice_FOUND FALSE)
    endif()

    add_library( cspice::cspice INTERFACE IMPORTED)
    set_target_properties(cspice::cspice PROPERTIES INTERFACE_LINK_LIBRARIES "${_cspice_target_name}" )
    set(cspice_FOUND TRUE)
    message(STATUS "cspice Target Created/Confirmed: cspice::cspice")

    unset(_cspice_target_name )
  endif()

endmacro()


macro(psmrts_add_bullet_target)

  if ( NOT TARGET Bullet::Bullet_double )
    if (BULLET_FOUND OR Bullet_FOUND )
      if(NOT "${BULLET_DEFINITIONS}" MATCHES ".*-DBT_USE_DOUBLE_PRECISION.*")
        message(
          FATAL_ERROR "Bullet does not appear to be built with double precision, current definitions: ${BULLET_DEFINITIONS}")
      endif()
      message(STATUS "Bullet Double Compile Definitions: ${BULLET_DEFINITIONS}")

      # This configuration ensures the Bullet variable definitions are also conformant.
      # It also set to work with the ISIS system.
      add_library( Bullet::Bullet_double INTERFACE IMPORTED )
      set(BULLET_INCLUDE_DIR ${BULLET_INCLUDE_DIRS})
      if("${BULLET_LIBRARY_DIRS}" STREQUAL "") # For vcpkg dang it!
        set(BULLET_LIBRARY_DIRS "lib")
      endif()

      # Reverse list of Bullet libraries and add absolute paths
      list(REVERSE BULLET_LIBRARIES)
      prepend_root_dir("${BULLET_INCLUDE_DIR}"    "${BULLET_ROOT_DIR}"     BULLET_INCLUDE_DIR)
      prepend_root_dir("${BULLET_LIBRARY_DIRS}"   "${BULLET_ROOT_DIR}"     BULLET_LIBRARY_DIRS)
      get_full_library_path("${BULLET_LIBRARIES}" "${BULLET_LIBRARY_DIRS}" BULLET_LIBRARIES)
      
      set_target_properties( Bullet::Bullet_double
        PROPERTIES
          INTERFACE_COMPILE_DEFINITIONS "${BULLET_DEFINITIONS}"
          INTERFACE_INCLUDE_DIRECTORIES "${BULLET_INCLUDE_DIR}"
          INTERFACE_LINK_DIRECTORIES "${BULLET_LIBRARY_DIRS}"
          INTERFACE_LINK_LIBRARIES_DIRECT "${BULLET_LIBRARIES}"
      )

      set(Bullet_double_FOUND TRUE)
      message(STATUS "Bullet Double Target Created/Confirmed: Bullet::Bullet_double")
      message(STATUS "Bullet Includes:  ${BULLET_INCLUDE_DIR}")
      message(STATUS "Bullet Libdirs:  ${BULLET_LIBRARY_DIRS}")
      message(STATUS "Bullet Libraries: ${BULLET_LIBRARIES}")
    endif()
  endif()

endmacro()

macro(psmrts_create_dependency_targets)

  psmrts_add_cspice_target()
  psmrts_add_bullet_target()

endmacro()