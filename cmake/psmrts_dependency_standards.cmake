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
    else()
      message(FATAL_ERROR "cspice library import not found - must be one of "
                         "unofficial::cspice::cspice, CSPICE::cspice or cspice ")
      set(cspice_FOUND FALSE)
    endif()

    add_library( cspice::cspice ALIAS "${_cspice_target_name}" )
    set(cspice_FOUND TRUE)
    message(STATUS "cspice Target Created/Confirmed: cspice::cspice")

    unset(_cspice_target_name )
  endif()

endmacro()


macro(psmrts_add_bullet_target)

  if ( NOT TARGET Bullet::Bullet_double )
    if (BULLET_FOUND OR Bullet_FOUND )
      if( NOT ${BULLET_DEFINITIONS}
        MATCHES
        ".*-DBT_USE_DOUBLE_PRECISION.*")
        message(
          FATAL_ERROR "Bullet does not appear to be built with double precision, current definitions: ${BULLETFLOAT64_DEFINITIONS}")
      endif()
      message(STATUS "Bullet Double Compile Definitions: ${BULLET_DEFINITIONS}")

      # This configuration ensures the Bullet variable definitions are also conformant.
      add_library( Bullet::Bullet_double INTERFACE IMPORTED )
      set_target_properties( Bullet::Bullet_double
        PROPERTIES
          INTERFACE_COMPILE_DEFINITIONS "${BULLET_DEFINITIONS}"
          INTERFACE_INCLUDE_DIRECTORIES "${BULLET_INCLUDE_DIRS}"
          INTERFACE_LINK_DIRECTORIES "${BULLET_ROOT_DIR}"
          INTERFACE_LINK_LIBRARIES "${BULLET_LIBRARIES}"
      )
      set(Bullet_double_FOUND TRUE)
      message(STATUS "Bullet Double Target Created/Confirmed: Bullet::Bullet_double")
    endif()
  endif()

endmacro()


macro(psmrts_add_tinyobjloader_target)
  
  # This works for both vcpkg and conan
  set(_tinytarget tinyobjloader::tinyobjloader_double )
  if ( NOT TARGET ${_tinytarget}  )
    message(FATAL_ERROR "tinyobjloader double precision library import target not found - must be "
                        "${_tinytarget} ")
    set(tinyobjloader_double_FOUND FALSE)

  else()

    get_target_property(_tiny_comp_prop ${_tinytarget} INTERFACE_COMPILE_DEFINITIONS)
    # Check for double precision target property
    message(STATUS "tinyobjloader Double Compile Definitions: ${_tiny_comp_prop}")

    if(NOT ${_tiny_comp_prop} MATCHES ".*TINYOBJLOADER_USE_DOUBLE.*")
      message(
        FATAL_ERROR "tinyobjloader does not appear to be built with double precision, current definitions: "
                    "${_tiny_comp_prop}")
    endif() 
    
    set(tinyobjloader_double_FOUND TRUE)
    message(STATUS "tinyobjloader Double Target Confirmed: ${_tinytarget}")

    unset(_tiny_comp_prop)   
    unset(_tinytarget)   
  endif()

endmacro()


macro(psmrts_add_miniply_target)

  # Only known package is vcpkg currently
  if ( NOT TARGET miniply::miniply )
    if ( TARGET unofficial::miniply::miniply )
      # VCPKG target name
      set(_miniply_target_name unofficial::miniply::miniply )
    else()
      message(FATAL_ERROR "miniply::miniply library import target not found - can be "
                         "unofficial::miniply::miniply or some other unknown target")
      set(miniply_FOUND FALSE)
    endif()

    add_library( miniply::miniply ALIAS "${_miniply_target_name}" )
    set(miniply_FOUND TRUE)
    message(STATUS "miniply Target Created/Confirmed: miniply::miniply")

    unset(_miniply_target_name )
  endif()

endmacro() 


macro(psmrts_create_dependency_targets)

  psmrts_add_cspice_target()
  psmrts_add_bullet_target()
  psmrts_add_tinyobjloader_target()
  psmrts_add_miniply_target()

endmacro()