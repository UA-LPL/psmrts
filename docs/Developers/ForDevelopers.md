  @page developerPage For Developers
  @tableofcontents

  @section developerConfiguration Configuring the PSMRTS Development Environment

  Building PSMRTS requires the pkg-config build utility which may not be available on your system. The MacOS does not ship with pkg-config and it must be installed. There are several ways to install pkg-config such as using HomeBrew, Anaconda/Miniconda (conda install pkg-config) or from source. The Conda solution will always require an active Python environment. Linux and Windows systems may provide this as a package install or other alternatives.

  For the Mac, it is recommended to install from source as it is a better long term solution. There is a universal binary install package for the Mac located here. Simply download the PKG file, click on the package and follow the install instructions.

  @section devBuildInstall Building and Installing PSMRTS

  Building `PSMRTS` requires the `pkg-config` build utility which may not be available on your system. The MacOS does not ship with `pkg-config` and it must be installed. There are several ways to install `pkg-config` such as using [HomeBrew](https://formulae.brew.sh/formula/pkg-config), Anaconda/Miniconda (`conda install pkg-config`) or from [source](https://gitlab.freedesktop.org/pkg-config/pkg-config). The Conda solution will always require an active Python environment. Linux and Windows systems may provide this as a package install or other alternatives.
The `PSMRTS` system upon cloning from the repo is contained in the `./psmrts` directory. One command is used to completely build `PSMRTS`. The `make_psmrts.sh` script is provided to ease the `PSMRTS` build process. After cloning `PSMRTS`, here are the basic command options of `make_psmrts.sh` to build the C++ and C API libraries:

1. `cd psmrts`
2. `./make_psmrts.sh`
   - Add `-s` to build shared libraries (required for conda builds)
   - Add `-t` to enable testing
   - Add `-c` to enable code coverage
   - Add `-x` to build extras
   - Add `-d` to build Debug
   - Add `-D` to build Doxgen documentation
   - Add `-V` to use the vcpkg package manager to provide PSMRTS dependencies
   - Add `-T` alternative vcpkg triplet
   - Add `-C` to use the conda package manager to provide PSMRTS dependencies
   - Add `-j x` will use x threads to build PSMRTS (recommended)

   You must use one of -V (vcpkg) or -C (conda) to provide the required PSMRTS dependencies or provide them by some other means. In some cases it may be useful to specify an alternative vcpkg triplet for certain platforms.
   
   When the build completes, the system can be installed with the following command:
 ```
 cmake  --install build --prefix install
 ``` 
Here `install` is the directory where `PSMRTS` header files, libraries and CMAKE configuration files will installed.

Below are details describing `PSMRTS` build environments in `conda` and `vcpkg`. Since the full `vcpkg` development environment is not easily provided directly, we used `conda` to provide additional functionality to provide required documentation and code coverage packages. 

  @subsection devCondaBuild Building PSMRTS with Conda

To build using the `conda` environment, you must first install [Anancond](https://www.anaconda.com) or [Miniconda](https://www.anaconda.com/docs/getting-started/miniconda/main) (recommended) and install the minimum build packages into a chosen named conda environment (e.g.,`PSMRTS`). It must be activated before building `PSMRTS`. To install Miniconda, follow the basic [instructions](https://www.anaconda.com/docs/getting-started/miniconda/install). There are several YAML files provided in PSMRTS that can be used to create a development environment for `PSMRTS` depending upon what you want to do.

To just build the `PSMRTS` system for installation, use the conda environment file `psmrts_conda_deps.yml`. This configuration installs the minimum `PSMRTS` The following instructions can be used to create the conda environment, build and install `PSMRTS`.
```
1. git clone https://github.com/UA-LPL/psmrts.git
2. cd psmrts
3. conda env create -n psmrts -f psmrts_conda_deps.yml
4. conda activate psmrts
5. ./make_psmrts.sh -s -x -C -j4          # cmake configuration step
6. cmake --install build --prefix install # Install in desired location
```
Note that you could install `PSMRTS` directly in the conda environment by setting `--prefix $CONDA_PREFIX`. This directly integrates `PSMRTS` into the conda environment and provides a consistent development environment that includes `PSMRTS` seamlessly. `PSMRTS` can then be incorporated into your application with the CMake command:
```
find_package(PSMRTS REQUIRED CONFIG)
```

To build `PSMRTS` tests, code coverage and documentation, use the `psmrts_conda_deps_all.yml` conda configuration file in step 1. that adds additional packages that `PSMRTS` uses for these features. Once this environment is install and activated the following commands can be used to build and run the other targets.
```
1.  git clone https://github.com/UA-LPL/psmrts.git
2.  cd psmrts
3.  conda env create -n psmrts_dev -f psmrts_conda_deps_all.yml
4.  conda activate psmrts_dev
5.  ./make_psmrts.sh -s -x -t -d -c -D -C -j4
6.  cmake --build build --target docs     # Build doxygen documentation
7.  open docs/html/index.html                  # On Mac, open the PSMRT documentation
8.  cmake --build build --target coverage      # Build code coverage
9.  open build/coverage/index.html             # On Mac, open the PSMRT code coverage report
10. cd build
11. ctest --output-on-failure -j4              # Run the PSMRT tests
```
`PSMRTS` documentation is built in the `./docs/html` directory. Code coverage is build in `./build/coverage`. The results of both are contained in a file named `index.html` and can be viewed with any web browser.

  @subsection devVcpkgBuild Building PSMRTS with vcpkg

As mentioned, `vcpkg` was used to develop `PSMRTS` due to its ease of setup/installation and the default state of dependency libraries being static archive (.a) libraries. In addition, instead of delivering prebuilt binaries, `vcpkg` builds all dependencies from source and caches them locally for efficient builds. `vcpkg` can also build `PSMRTS` with `classic` or `manifest` mode. The major difference between these modes are `classic` maintains a system wide installation of a `vcpkg` installation whereas `manifest` mode installs dependencies in a local directory, typcially in `./build/vcpkg_installed` - and at times dependencies can be installed from a system or user cache and not rebuilt from source. `PSMRTS` detects `classic` mode by checking for the existance of the environment variable called `VCPKG_ROOT`. If it is not set, it sets VCPKG_ROOT=$PWD/vcpkg, clones `vcpkg` in `./psmrts` and runs cmake. This is `manifest` mode. If `VCPKG_ROOT` is set prior to running `make_psmrts.sh` `PSMRTS` does not install `vcpkg` or install any of is dependencies and assumes they are installed in a system-wide `vcpkg` installation. This is `classic` mode.

When building with `vcpkg`, you can build outside a `conda` environment with testing turned on (-t -d) and without code coverage and documentation (exlude -s -c -D). This also requires not preinstall as does when builing with a full `conda` environment. The instructions to build in `manifest` mode:

```
1. git clone https://github.com/UA-LPL/psmrts.git
2. cd psmrts
3. ./make_psmrts.sh -t -d -V -j4          # cmake configuration step
4. cmake --install build --prefix install # Install in desired location
```

Building `PSMRTS` tests with `vcpkg` requires additional packages and programs
that are not directly available in `vcpkg` so they must come from somewhere
else. You could use Homebrew or conda, however, Homebrew will install them in a
system-wide location which may impact how `PSMRTS` builds using different build
environments. Using conda to provide the additional packages needed to build
documentation and code coverage installs them in an isolated environment to
minimize impact on other build situations. `PSMRTS` provides a YAML file
`./tools/build_addons.yml` that is intended to provide the necessary tools to
create a `conda` environment containing the applications required to produce
documentation and code coverage. To use `conda` for these requirements, you must
first install Miniconda as described in the `conda` section, Once Miniconda is
available, use the following instructions for a full `vcpkg` development
experience:

```
1.  git clone https://github.com/UA-LPL/psmrts.git
2.  cd psmrts
3.  conda env create -n psmrts_vcpkg -f tools/build_addons.yml
4.  conda activate psmrts_vcpkg
5.  ./make_psmrts.sh -x -t -s -d -c -D -V -j4
6.  cmake --build build --target docs     # Build doxygen documentation
7.  open docs/html/index.html                  # On Mac, open the PSMRTS documentation
8.  cmake --build build --target coverage      # Build code coverage
9.  open build/coverage/index.html             # On Mac, open the PSMRTS code coverage report
10. cd build
11. ctest --output-on-failure -j4              # Run the PSMRTS tests
```

In some cases you may need to explicitly specify a `vcpkg` triplet. You may provide a specific triplet to build for other platforms should the `make_psmrts.sh` script fail to determine the proper triplet. See the `vcpkg` documentation describing [triplets](https://learn.microsoft.com/en-us/vcpkg/concepts/triplets) for additional details.

  @section devTesting Testing PSMRTS Code

The C++ testing framework Catch2 is in `PSMRTS` for the C++ API. `PSMRTS` tests are organized by features in  `./tests` subdirectories. The `PSMRTS` C API is tested with the `cmocka` C testing framework. Each `./tests` directory configures its own testing environment including code coverage. Developers may use other testing frameworks by adding the package dependency in the `vcpkg.json` or `conda` YAML file and configure appropriately. Each `PSMRTS` feature should build its own test application and add it to the `ctest` system. See the cmake configuration the  [psmrts/core/tests](./psmrts/core/tests/CMakeLists.txt) directory for an example.

  @section devDocumentation Creating PSMRTS Documentation

`PSMRTS` documentation system is based upon the [Doxygen](https://www.doxygen.nl) generator. The `docs` directory contains the Doxygen file that contains the configuration to create the `PSMRTS` documentation. The CMAKE target `docs` creates the HTML documentation in the `docs/html` directory. The `conda` configuration provides the necessary tools to create the documentation but other means can provide the required apps, namely `doxygen`, `graphviz`, `gcovr` and `lcov`. 

The following commands can be used to create the necessary `conda` environment and build the documentation (and code coverage):

```
1.  git clone https://github.com/UA-LPL/psmrts.git
2.  cd psmrts
3.  conda env create -n psmrts_docs_cov -f psmrts_conda_deps_all.yml
4.  conda activate psmrts_docs_cov
5.  ./make_psmrts.sh -t -x -s -d -c -D -C -j4
6.  cmake --build build --target docs     # Build doxygen documentation
7.  open docs/html/index.html                  # On Mac, open the PSMRTS documentation

  @section devCodeCoverage Code Coverage in PSMRTS

Code coverage can be ran on `PSMRTS` code by providing the `-c` flag to the PSMRTS build scripts. `PSMRTS` uses a custom CMake code coverage script called [CodeCoverage.cmake](https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake). This file is included in the code repository in the `./cmake` directory.

Code coverage prerequisites are provided in the `PSMRTS` package manager configurations. The cmake configurations in each `./tests` subdirectories can customize what files are added in code coverage reports. The `PSMRTS` cmake configuration provides methods to include and exclude source files in the report. See the cmake file [psmrts_register_code_coverage.cmake](.cmake/psmrts_register_code_coverage.cmake) for details and refer to the tests subdirectories CMakeLists.txt files for examples.

Using the configuration described in the `PSMRTS Documentation` section above, you can create a code coverage HTML report using the following additional commands: 

```
1.  cmake --build build --target coverage      # Build code coverage
2.  open build/coverage/index.html             # On Mac, open the PSMRTS code coverage report

  @section devWindows Windows Builds

Development of `PSMRTS` was done and tested mainly using Mac and Linux platforms, but includes limited ongoing Windows support. The related scripts are Windows [PowerShell](https://learn.microsoft.com/en-us/powershell/scripting/overview?view=powershell-7.5) specific, and builds require the following to be installed with appropriate pathing:
- [Visual Studio](https://visualstudio.microsoft.com/), with related packages such as Desktop development with C++ (specifically MSVC, C++ CMake tools for Windows, Windows SDK, vcpkg package manager, and GitHub Copilot)
- [CMake](https://cmake.org/download/) Release Version

Similar to above, once changed to an appropriate directory made to hold `PSMRTS`, and cloned using the `git clone https://github.com/UA-LPL/psmrts.git` , the following commands will build the system and run the appropriate tests:

```
git clone https://github.com/UA-LPL/psmrts.git
cd psmrts
.\make_psmrts.ps1 -t
cd build
MSBuild psmrts.sln /p:Configuration=Release
ctest -C Release
```

Note that code coverage is not included in Windows support at this time. For any
errors or complications of above, please reach out to the development team.

\htmlonly[block]
<div style="display:none">
\endhtmlonly

   @subpage developerOnboard
\cond
   @subpage developerInstall
   @subpage developerDesignTopics
   @subpage developerTODO
\endcond
\htmlonly[block]
<div style="display:none">
\endhtmlonly   
