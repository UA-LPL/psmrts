#[==[.md
### Add a test application and any exclusions to participate in code coverage

__NOTE__ this function does not call add_test() or any testing framework
test discovery functions. That is the responsibility of the developer.
This function manages the parameters that are provided to the
setup_target_for_coverage_gcovr_html() function.


```
psmrts_register_code_coverage(
      DEPENDENCY testapp
      EXCLUDES   "paths/*;files"
)
```

There are two variables that contain lists of code coverage applications names
(PSMRTS_COVERAGE_DEPENDENCIES) and a list of paths and files to exclude from
testing (PSMRTS_COVERAGE_EXCLUDES). Either parameter can be a list of applications
and directory glob patterns or file names.

The `PSMRTS_COVERAGE_DEPENDENCIES` parameter is accumulates the `DEPENDENCIES`
parameter. This will typically be your Catch2/GTest application name provided
in add_test(). This is an optional parameter.

The `PSMRTS_COVERAGE_EXCLUDES` parameter accumulates a list of files given to
the EXCLUDE parameter from code coverage. File paths are typically relative to
the root dir. For PSMRTS the gcovr root directory is
"${PSMRTS_SOURCE_DIR}/psmrts".

The PSMRTS top level determines the BASE_DIRECTORY.

Due to the nature of adding a test at any build stage, particularly private
implementations, adding code coverage dependencies and exclusion configurations
is necessary. This function provides the ability to add the name of
applications to include in coverage testing (PSMRTS_COVERAGE_DEPENDENCIES) and
files to exclude (PSMRTS_COVERAGE_EXCLUDES) during the code coverage build.

Here are some examples of calls for this functions.

```
psmrts_register_code_coverage(DEPENDENCY "psmrts_test_suite" EXCLUDES "psmrts_test_suite;paths/*")
psmrts_register_code_coverage(DEPENDENCY "psmrts_test_suite2")
psmrts_register_code_coverage(EXCLUDES   "all_the_bad_ones.*")
psmrts_register_code_coverage(EXCLUDES   "core/tests/psmrts_catch2_environment.hpp")  
```
#]==]
function(psmrts_register_code_coverage)

    include(CMakeParseArguments)
    cmake_parse_arguments(
        psmrts_register
        ""
        ""
        "DEPENDENCY;EXCLUDES"
        ${ARGN}
    )

    # Check on the call status
    if (psmrts_register_UNPARSED_ARGUMENTS)
      message(FATAL_ERROR
        "There are unparsed arguments to psmrts_register_code_coverage(): "
        "${psmrts_register_UNPARSED_ARGUMENTS}"
      )
    endif (psmrts_register_UNPARSED_ARGUMENTS)

    if(NOT "${psmrts_register_DEPENDENCY}" STREQUAL "")
      set_property(GLOBAL APPEND PROPERTY PSMRTS_COVERAGE_DEPENDENCIES "${psmrts_register_DEPENDENCY}" )
    endif()

    if(NOT "${psmrts_register_EXCLUDES}" STREQUAL "")
      set_property(GLOBAL APPEND PROPERTY PSMRTS_COVERAGE_EXCLUDES "${psmrts_register_EXCLUDES}" )
    endif()

endfunction(psmrts_register_code_coverage)