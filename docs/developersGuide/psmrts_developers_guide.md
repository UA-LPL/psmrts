@page developersGuide PSMRTS Developers Guide
@tableofcontents

PSMRTS provides C++ and C APIs that produce prioritized ray tracing systems. A ray tracing system is an ordered series of customizable ray tracers that operate on tessellated plates (i.e. facet-based meshes) and ellipsoid models.

This document is a HOWTO of sorts to aid/guide developers in the process and procedures to contribute to PSMRTS. Topics covered here include: configuring a development computer system (this covers the Mac platform for now), acquiring and familiarization of resources and tools, installing PSMRTS for development, and setting up the development environment. These processes will enable the PSMRTS developer to create an PSMRTS system, make changes to existing code and/or add new software to PSMRTS, build and install PSMRTS, develop unit and application tests, and add documentation. These activities are all performed on your local system. The next step is to submit your contributions to the USGS/Astro PSMRTS repo for distribution.

@section devResources Resources
Below is a select few of the numerous resources available to assist in the PSMRTS development process. Contributing to the PSMRTS repository requires an understanding of and compliance with polices and best practices of open source software development standards.

- <a href="https://rockylinux.org" target="_blank" rel="noopener">Rocky Linux</a>
- <a href="https://pubs.opengroup.org/onlinepubs/9699919799/utilities/toc.html" target="_blank" rel="noopener">Bash Shell</a>
- <a href="https://code.visualstudio.com" target="_blank" rel="noopener">Visual Studio Code Editor</a>
- <a href="https://www.markdownguide.org" target="_blank" rel="noopener">Markdown Guide: The Common Language of Communicating Code</a>

- <a href="https://anaconda.org" target="_blank" rel="noopener">Anaconda Python Environment Management</a>
- <a href="https://docs.conda.io/projects/miniconda/en/latest/" target="_blank" rel="noopener">Miniconda Python Environment Management</a>
- <a href="https://git-scm.com" target="_blank" rel="noopener">Git: Free, Open Source Version Control System</a>
- <a href="https://github.com" target="_blank" rel="noopener">GitHub Public Source Code Repository Platform</a> 
- <a href="https://gitlab.com" target="_blank" rel="noopener">GitLab Public Source Code Repository Platform</a> 
- <a href="https://cmake.org" target="_blank" rel="noopener">Cmake Source Code Build System</a> 
- <a href="https://cplusplus.com" target="_blank" rel="noopener">cplusplus.com</a>
- <a href="https://en.cppreference.com/w/" target="_blank" rel="noopener">C++ Reference</a> 
- <a href="https://isocpp.org/faq" target="_blank" rel="noopener">C++ FAQ</a> 
- <a href="https://vcpkg.io/en/" target="_blank" rel="noopener">vcpkg: A C++ Dependency Manager</a> 
- <a href="https://github.com/catchorg/Catch2" target="_blank" rel="noopener">Catch2: A C++ Testing Framework</a> 

@section operatingSystems Operating Systems

@subsection macLinux Mac/Linux 

The Apple Mac platform provides a well-suited platform for PSMRTS development that is not too complicated to setup. The basic requirement is to install the latest version of Xcode to get started. Note that development on the new ARM platform is not fully supported in `vcpkg` but does seem to work for most of the current PSMRTS dependencies. The major reason for this is due to lack of ARM compatibility of many of the libraries in the `vcpkg` ports.

If you want to use a Mac ARM computer for PSMRTS development, see this ISIS <a href="https://github.com/DOI-USGS/ISIS3/issues/5188" target="_blank" rel="noopener">issue</a>. This post is relevant for any project using `conda` in addition to those using other configurations. For the Mac Intel platform, you should be able to immediately begin PSMRTS development after basic Xcode installation. Review the [Resources](#devResources) section above for additional information.

@subsection windows Windows

Development of `PSMRTS` was done and tested mainly using Mac and Linux platforms, but includes limited ongoing Windows support. The related scripts are Windows <a href="https://learn.microsoft.com/en-us/powershell/scripting/overview?view=powershell-7.5" target="_blank" rel="noopener">Power Shell</a> specific, and builds require the following to be installed with appropriate path:
- <a href="https://visualstudio.microsoft.com/" target="_blank" rel="noopener">Visual Studio</a>, with related packages such as Desktop development with C++ (specifically MSVC, C++ CMake tools for Windows, Windows SDK, vcpkg package manager, and GitHub Copilot)
- <a href="https://cmake.org/download/" target="_blank" rel="noopener">CMake</a> Release Version

Similar to above, once changed to an appropriate directory made to hold `PSMRTS`, and cloned using the `git clone https://github.com/UA-LPL/psmrts.git` , the following commands will build the system and run the appropriate tests:

git clone https://github.com/UA-LPL/psmrts.git
cd psmrts
.\make_psmrts.ps1 -t
cd build
MSBuild psmrts.sln /p:Configuration=Release
ctest -C Release

Note that code coverage is not included in Windows support at this time. For any
errors or complications of above, please reach out to the development team.

@section buildInstall Building and Installation

It is recommended to create a dedicated directory to contain all PSMRTS working development directories. One possible configuration would be `mkdir -p ~/PSMRTS/GitCheckOuts`. All PSMRTS working directories would then be installed in `~/PSMRTS/GitCheckOuts`. I tend to name development directories the same as the branch name I will publish the modifications to. There are several types of development branches that prefix the name of the branch and use a `/` as a separator. For example, the branch `feature/psmrts-raytrace-tests` could indicate its not a bug fix but a new `feature`` or contribution to PSMRTS. The name of the branch, `psmrts-raytrace-tests`, indicates the changes provide Catch2 tests for ray tracing operations. Here is a sequence to check out and build this branch:

1. cd ~/PSMRTS/GitCheckouts
2. mkdir PsmrtsRaytraceTests
3. cd  PsmrtsRaytraceTests
4. git clone http://github.com/UA-LPL/psmrts.git
5. cd PSMRTS
6. ./make_install.sh -x -t

Here the `-t` will build tests and `-x` will build extras. Building tests is necessary to ensure that the Catch2 test framework is enabled for development. `-x` may include support applications and other tools.

For the Mac, it is recommended to install from source as it is a better long term solution. There is a universal binary install package for the Mac located here. Simply download the PKG file, click on the package and follow the install instructions.

Building `PSMRTS` requires the `pkg-config` build utility which may not be available on your system. The MacOS does not ship with `pkg-config` and it must be installed. There are several ways to install `pkg-config` such as using <a href="https://formulae.brew.sh/formula/pkgconf" target="_blank" rel="noopener">HomeBrew</a>, Anaconda/Miniconda (`conda install pkg-config`) or from <a href="https://gitlab.freedesktop.org/pkg-config" target="_blank" rel="noopener">source</a>. The Conda solution will always require an active Python environment. Linux and Windows systems may provide this as a package install or other alternatives.

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

@subsection devCondaBuild Using Conda

To build using the `conda` environment, you must first install <a href="https://www.anaconda.com" target="_blank" rel="noopener">Anaconda</a> or <a href="https://www.anaconda.com/docs/getting-started/miniconda/main" target="_blank" rel="noopener">Miniconda</a>(recommended) and install the minimum build packages into a chosen named conda environment (e.g.,`PSMRTS`). It must be activated before building `PSMRTS`. To install Miniconda, follow these basic <a href="https://www.anaconda.com/docs/getting-started/miniconda/install" target="_blank" rel="noopener">instructions</a>. There are several YAML files provided in PSMRTS that can be used to create a development environment for `PSMRTS` depending upon what you want to do.

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
7.  open docs/html/index.html             # On Mac, open documentation
8.  cmake --build build --target coverage # Build code coverage
9.  open build/coverage/index.html        # On Mac, open code coverage report
10. cd build
11. ctest --output-on-failure -j4         # Run PSMRTS tests
```
`PSMRTS` documentation is built in the `./docs/html` directory. Code coverage is build in `./build/coverage`. The results of both are contained in a file named `index.html` and can be viewed with any web browser.

@subsection devVcpkgBuild Using vcpkg

As mentioned, `vcpkg` was used to develop `PSMRTS` due to its ease of setup/installation and the default state of dependency libraries being static archive (.a) libraries. In addition, instead of delivering prebuilt binaries, `vcpkg` builds all dependencies from source and caches them locally for efficient builds. `vcpkg` can also build `PSMRTS` with `classic` or `manifest` mode. The major difference between these modes are `classic` maintains a system wide installation of a `vcpkg` installation whereas `manifest` mode installs dependencies in a local directory, typically in `./build/vcpkg_installed` - and at times dependencies can be installed from a system or user cache and not rebuilt from source. `PSMRTS` detects `classic` mode by checking for the existence of the environment variable called `VCPKG_ROOT`. If it is not set, it sets VCPKG_ROOT=$PWD/vcpkg, clones `vcpkg` in `./psmrts` and runs cmake. This is `manifest` mode. If `VCPKG_ROOT` is set prior to running `make_psmrts.sh` `PSMRTS` does not install `vcpkg` or install any of is dependencies and assumes they are installed in a system-wide `vcpkg` installation. This is `classic` mode.

When building with `vcpkg`, you can build outside a `conda` environment with testing turned on (-t -d) and without code coverage and documentation (exclude -s -c -D). This also requires not preinstall as does when building with a full `conda` environment. The instructions to build in `manifest` mode:

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

In some cases you may need to explicitly specify a `vcpkg` triplet. You may provide a specific triplet to build for other platforms should the `make_psmrts.sh` script fail to determine the proper triplet. See the `vcpkg` documentation describing <a href="https://learn.microsoft.com/en-us/vcpkg/concepts/triplets" target="_blank" rel="noopener">triplets</a> for additional details.

@subsection testing Testing

`PSMRTS` uses the <a href="https://github.com/catchorg/Catch2" target="_blank" rel="noopener">Catch2</a> Testing Framework for unit and app testing in the C++ API. `PSMRTS` tests are organized by features in  `./tests` subdirectories. The `PSMRTS` C API is tested with the `cmocka` C testing framework. Each `./tests` directory configures its own testing environment including code coverage. Developers may use other testing frameworks by adding the package dependency in the `vcpkg.json` or `conda` YAML file and configure appropriately. Each `PSMRTS` feature should build its own test application and add it to the `ctest` system. See the cmake configuration [psmrts/core/tests](../../psmrts/core/tests/CMakeLists.txt) directory for an example.

@subsection codeCoverage Code Coverage

Code coverage can be run on `PSMRTS` code by providing the `-c` flag to the PSMRTS build scripts. `PSMRTS` uses a custom CMake code coverage script called <a href="https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake" target="_blank" rel="noopener">CodeCoverage.cmake</a>. This file is included in the code repository in the `./cmake` directory.

Code coverage prerequisites are provided in the `PSMRTS` package manager configurations. The cmake configurations in each `./tests` subdirectories can customize what files are added in code coverage reports. The `PSMRTS` cmake configuration provides methods to include and exclude source files in the report. See the cmake file [psmrts_register_code_coverage.cmake](../../cmake/psmrts_register_code_coverage.cmake) for details and refer to the tests subdirectories CMakeLists.txt files for examples.

Using the configuration described in the `PSMRTS Documentation` section above, you can create a code coverage HTML report using the following additional commands: 

```
1.  cmake --build build --target coverage      # Build code coverage
2.  open build/coverage/index.html             # On Mac, open the PSMRTS code coverage report
```
@subsection documentation Generating Documentation

`PSMRTS` documentation system is based upon the <a href="https://www.doxygen.nl" target="_blank" rel="noopener">Doxygen</a> generator. The `docs` directory contains the Doxygen file that contains the configuration to create the `PSMRTS` documentation. The CMAKE target `docs` creates the HTML documentation in the `docs/html` directory. The `conda` configuration provides the necessary tools to create the documentation but other means can provide the required apps, namely `doxygen`, `graphviz`, `gcovr` and `lcov`. 

The following commands can be used to create the necessary `conda` environment and build the documentation (and code coverage):

```
1.  git clone https://github.com/UA-LPL/psmrts.git
2.  cd psmrts
3.  conda env create -n psmrts_docs_cov -f psmrts_conda_deps_all.yml
4.  conda activate psmrts_docs_cov
5.  ./make_psmrts.sh -t -x -s -d -c -D -C -j4
6.  cmake --build build --target docs     # Build doxygen documentation
7.  open docs/html/index.html                  # On Mac, open the PSMRTS documentation
```
@section contributing Contributing
To begin an PSMRTS pull request, you must first create a PSMRTS issue.

Once the PR is ready you visit your branch and choose the `Contribute` button and then `Open Pull Request`. This will launch a web GUI that needs to be filled out. Note that your original PSMRTS issue describing this PR update must be added in the **Related Issue** section of the PR request. I also tend to write what was committed in to the PR branch for merge into PSMRTS.

Also, you must make an entry into the <a href="http://orgit:7990/projects/ISIS/repos/psmrts/browse/CHANGELOG.md" target="_blank" rel="noopener">CHANGELOG.md</a> file with a _single_ entry that includes a reference to the PR post.

This process is typically done by _publishing_ your PSMRTS working directory to a repo on your own GitHub account/site. The GitHub website can then be used to create a pull request (PR) for your contribution to the UA/PSMRTS repo `main` development branch. At that point, a UA/PSMRTS developer will conduct a review of your work and give feedback for changes that may need to made before the PR will be accepted and merged.

Any suggestions/requests for change will be made in your original PSMRTS working directory branch and then git `pushed` to the branch you provided in the PR request. The PR will update automatically whenever you change the PR branch.

When the PSMRTS developers decide to accept your contribution, it will be merged into the `main` UA/PSMRTS repo and you can then remove the branch if you prefer (I save all mine).

@section psmrtsPrimer A PSMRTS Primer

`PSMRTS` C++ and C APIs provide custom, optimized ray tracing configurations primarily for (though not limited to) the mapping of small, irregularly-shaped extraterrestrial bodies. These configurations (or _products_) are <i>shapes</i> and <i>tracers</i>.

Shapes can be defined mathematically (e.g. spheres, spheroids, ellipsoids) or as a mesh input from a file (PLY, Wavefront OBJ, and NAIF DSK are currently supported) and converted to a set of vertices and facet indexes. Tracers use shapes for ray tracing. Tracer products are based on ellipsoids or on the <a href="https://github.com/bulletphysics/bullet3" target="_blank" rel="noopener">Bullet Physics</a> library or <a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/dsk.html" target="_blank" rel="noopener">NAIF DSK</a> kernel.

Products are instantiated via the `PSMRTS` <i>product request system</i> and must provide parameter specifications and customized configurations. This is analogous to a transactional system in that it enables users to select and configure mesh data sources and apply them in a ray tracing library or system.

@subsection productSpecsConfigs Product Specifications and Configurations

Each product is configured via
its own uniquely specified set of _features_. Feature values are defined
in product _configurations_. Product
configurations contain _options_ that are feature keywords and value pairs where
the keywords are product specification feature keywords and values conform to
the type and valid values of the feature. Each product configuration can define
no more than one shape and/or one tracer. There are some tracers that accept
shapes specified by a filename. Each of the shape products contain a list of
file suffixes that are used to uniquely and easily identify a supported format
from which a mesh is read and processed.

Each product contains a [ProductSpecification](../../psmrts/core/products/ProductSpecification.hpp) definition that accepts a
[ProductConfiguration](../../psmrts/core/products/ProductConfiguration.hpp) that contains feature option keyword/value pairs that are
compared to its specifications. Each option is evaluated against features by
name or _alias_. A feature _alias_ is simply another keyword that can be
substituted for the real feature option name so that generic configurations can
be used. All 

The PSMRTS application, [psmrts_product_specs](../../psmrts/apps/psmrts_product_specs.cpp) provides the
the product interface specifications of all shapes and tracers. The output is parseable JSON format.

@subsection shapeProductConfigs PsmrtsShape Product Configurations

The class [PsmrtsShape](../../psmrts/shapes/PsmrtsShape.hpp) maintains a set of
shape file readers. The supported shape formats are provided NAIF DSKs by
[DskShape](../../psmrts/shapes/dsk/DskShape.hpp), OBJ by
[ObjShape](../../psmrts/shapes/obj/ObjShape.hpp), PLY by
[PlyShape](../../psmrts/shapes/ply/PlyShape.hpp) and generic mesh in
[MeshShape](../../psmrts/shapes/mesh/MeshShape.hpp). 

To configure a shape, refer to the shape only product configuration as reported
by by running the command `psmrts_product_specs --shapes` or with a specific
shape using `psmrts_product_specs obj`. For the `obj` shape, you will see the
following configuration:
```
{
 "shapes": [
  {
   "obj": {
    "info": {
     "name": "obj",
     "product": "shape",
     "description": "Reads Wavefront OBJ mesh files and creates a PSMRTS mesh object"
    },
    "features": [
     {
      "name": "shape",
      "type": "string",
      "description": "Describe the product type",
      "status": "optional",
      "default": "obj",
      "valid": "obj"
     },
     {
      "name": "obj_file",
      "type": "file",
      "description": "Name of OBJ file/string to read",
      "status": "required",
      "aliases": [
       "file",
       "filename",
       "obj_mesh",
       "mesh_file",
       "shapefile"
      ],
      "file_suffixes": [
       "obj",
       "OBJ"
      ]
     },
     {
      "name": "obj_string",
      "type": "string",
      "description": "Format-compatible string containing contents of an OBJ file",
      "status": "optional",
      "aliases": "obj_mesh_string"
     },
     {
      "name": "obj_data_type",
      "type": "string",
      "description": "Type of mesh vector data requested/read",
      "status": "optional",
      "aliases": [
       "data_type",
       "mesh_data_type"
      ],
      "valid": [
       "double",
       "float"
      ],
      "default": "double"
     },
     {
      "name": "obj_mtl_search_path",
      "type": "directory",
      "description": "Directory path to OBJ materials files",
      "status": "optional",
      "aliases": [
       "obj_materials_dir",
       "obj_materials_directory"
      ]
     }
    ]
   }
  }
 ]
}
```
The minimum requirement to specify this shape is to provide a `obj_file` option
naming a specific file name, such as
`bennu_g_00880mm_alt_obj_0000n00000_v021.obj`, using the
[ProductOption](../../psmrts/core/products/ProductOption.hpp). To more
fully specify the `obj` format directly, you can use the _product_ and _name_
options shown in the __info__ section of the above JSON structure. To configure
this shape in PSMRTS, use the following code:
```
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductCart.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/PsmrtsShape.hpp>

namespace psmrts {

  std::string objfile = "bennu_g_00880mm_alt_obj_0000n00000_v021.obj";
  ProductConfiguration obj_config( objfile,
                                  { ProductOption( "shape", "obj" ), 
                                    ProductOption( "obj_file", objfile ) } );


  // Create the product and read the mesh
  PsmrtsMaker<PsmrtsShape> maker_s( "maker" );
  if ( !maker_s.process_config( obj_config ) ) {
    maker_s.throw_errors();  // To invoke std::runtime_error() or get the errors
                            // using the PsmrtsMaker::error_to_string() method.
  }

  // Get the shape/mesh
  PsmrtsShape obj_s = maker_s.product(); // Will be invalid if not created
  ProductCart obj_c = maker_s.cart();   // Contains the ProductSpecification matched

  // PsmrtsUid.is_valid_uid( obj_c.get_shape_id() )  == true;
  // PsmrtsUid.is_valid_uid( obj_c.get_tracer_id() ) == false;
  if ( !obj_c.has_valid_content() ) {
    // If there are residuals, then the config has unrecognized options which
    //  could be an error
    if ( obj_c.residual_size() > 0 ) {
      throw std::runtime_error( "*** Error - shape has residuals which is invalid: " + 
                                obj_c.residual_config().to_json().dump(-1) );
    }
  }

  // Get the mesh if needed
  PsmrtsMeshData mesh = obj_s.get_mesh();
} // namespace psmrts

```

@subsection tracerProductConfigs PsmrtsTracer Product Configurations

The configuration of PSMRTS tracers are very similar to how shapes are
configured. The `bullet` tracer is perhaps the most versatile and  efficient
tracer in PSMRTS at the moment. It accepts all types of PsmrtsShapes and creates
a very efficient tracer by default. The configuration for this tracer must
include a configuration for a shape as well. This is indicated by the
specification type of _dependency_ in the "shape" feature option as shown in the
bullet specification produced by `psmrts_product_specs bullet`:
```
{
 "tracers": [
  {
   "bullet": {
    "info": {
     "name": "bullet",
     "product": "tracer",
     "description": "The Bullet Physics ray tracing system specification"
    },
    "features": [
     {
      "name": "tracer",
      "type": "string",
      "description": "Describe the product type",
      "status": "required",
      "default": "bullet",
      "valid": "bullet"
     },
     {
      "name": "shape",
      "type": "string",
      "status": "dependency",
      "description": "Bullet requires a file/mesh shape",
      "aliases": [
       "file",
       "filename",
       "obj_file",
       "obj_mesh",
       "obj_string",
       "ply_file",
       "ply_mesh",
       "dsk_file",
       "dsk_mesh",
       "mesh_file",
       "source"
      ]
     },
     {
      "name": "bullet_optimize_bvh",
      "type": "bool",
      "description": "Use optimized bounding volume hierarchy (BVH) when created",
      "status": "optional",
      "default": "true",
      "valid": [
       "true",
       "1",
       "yes",
       "false",
       "0",
       "no"
      ]
     },
     {
      "name": "bullet_compression",
      "type": "bool",
      "description": "Compress Bullet data during construction",
      "status": "optional",
      "default": "true",
      "valid": [
       "true",
       "1",
       "yes",
       "false",
       "0",
       "no"
      ]
     }
    ]
   }
  }
 ]
}
```
This particular specification requires the __tracer=bullet__ configuration
option because it has no other required options other than __shape__ (a
_dependency_ type is a required parameter). Note the _alias_ options for
__shape__. It specifies all the file name feature options and their aliases of
all the valid shape types. This implies that if one of these options is given in
the "bullet" configuration, it is recognized as providing a __shape__ option.
Note that internally, it will be passed as a _residual_ configuration option
that indicates a __shape__ product is required. The `PsmrtsMaker` logic will
then automatically process the shape configuration. Here is the minimal
configuration for a "bullet" tracer.
```
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/core/ProductCart.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/PsmrtsShape.hpp>

namespace psmrts {

  std::string objfile = "bennu_g_00880mm_alt_obj_0000n00000_v021.obj";
  ProductConfiguration bullet_config( "bullet" );
  bullet_config.add( ProductOption( "shape", "obj" ) );
  bullet_config.add( ProductOption( "obj_file", obj_file)  ) );
  bullet_config.add( ProductOption( "tracer", "bullet" ) );
  PsmrtsMaker<PsmrtsTracer> maker_t( "maker" );

  if ( !maker_t.process_config( bullet_config ) ) {
    maker_t.throw_errors();  
  }

  // Get the tracer if it has been successfully created.
  PsmrtsTracer bullet_t = maker_t.product();
}
```

@subsection rayTracingSystem PSMRTS Ray Tracing System

Once you have a tracer, you can now create a rays to process with the tracer. A
ray in PSMRTS follows standard definition of the an observer/look direction
three dimensional (3D) vector system. In this system, there are two vectors that
define a ray trace: a vector describing the position of the observer and a
second unit vector that represents the direction from the observer to perform a
trace. These two vectors combined is a PSMRTS ray trace and are stored in the
[PsmrtsRayTrace](../../psmrts/core/PsmrtsRayTrace.hpp) class.

@subsubsection observerLookDirCoordSys Observer/Look Direction Coordinate System

PSMRTS performs ray traces in the planet body-fixed coordinate system. The
position of the observer is a ray that extends from the center of the planet
body to a point in 3D space. The look vector is a unit vector where the origin
is from the observer position presumably toward the planet/body surface. These
two vectors are parameters to constructors provided in the
[PsmrtsRayTrace](../../psmrts/core/PsmrtsRayTrace.hpp) class. This class is the
fundamental basis for a system of extended classes that perform different types
and varieties of traces. However, the result of every trace in the PSMRTS
system is retained in a PsmrtsRayTrace object after the PSMRTS tracer calculates
surface intercepts of the ray path along look vectors. If the ray successfully
intercepts the planet surface, methods in this class provides or computes other
cartographic/observational geometry and surface properties.

@subsubsection photometricRayTraceOps Photometric Ray Tracing Operations

PSMRTS provides a suite of helper tracing classes that extend tracing operations.
A common operation in planetary observation geometry is to compute photometric
angles. These angles are computed using a second ray from the sun to the
observer/look direction ray trace surface intercept point. This ray requires
the precise position of the sun at the time the observer/look direction vector
was calculated. Therefore, this operation requires three vectors: the two
observer position and unit look vector and the position of the sun relative
to the body of intercept. The sun relative position vector is also in body-fixed
coordinates where the origin is the center of the body. The photometric ray look
direction vector is computed as the vector from the sun to the surface intercept
computed from the observer/look direction vector. 

@subsubsection processRequestRayTraceClasses Process Request Ray Tracing Classes

PSMRTS provides a _process request_ system of classes, called `PRQ`, that
extends ray tracing capabilities. These classes compute one or more ray trace
computations. These classes are defined in the file
[PsmrtsRequest](../../psmrts/core/PsmrtsRequest.hpp). These classes are special
processing class that inherit `PsmrtsRequest` as a base class that tracks errors
and performance metrics. The primary purpose of these classes are to provide
conditional execution of PRQ request class processing operations. 

This is necessary since not all tracers possess the same properties. For example,
mathematical models such as ellipsoids, spheroids and spheres do not have
facets. One datum provided within the PsrmtsRayTrace is a __facet__. A __facet__
is comprised of a 3-vector set required to describe a mesh triangle in 3D space
that originates from the shape mesh and an integer-based vector where each value
in the vector is the index into the mesh data buffer containing the surface
vectors. For mathematical models, there are no
facets. Facets are retrieved in a `PRQFacet` class that is passed to a tracer
method called of the form __process( PRQFacet &facet)__ that must be implemented
in tracers that actually have facets. For tracers that do not have facets, they
don't implement these methods.

In fact, any tracer that does have certain PRQ properties, do not have to
implement any of the tracer process() methods. This minimizes the burden of
writing dummy methods and minimizes code maintenance.

Here is a list and brief description of all the PRQ classes contained in the
[PsrmtsRequest](../../psmrts/core/PsmrtsRequest.hpp) file:

- __PQRRayTrace__ contains a single ray trace and is the fundamental class for all
  ray tracers. It is primarily for precision tracking of ray trace operations.
- __PRQRayTraceArray__ contains a vector of ray traces that are performed in
  sequence (or in threads).
- __PRQPhotometricTrace__ contains two traces, one for observer look direction
  and a second for sun/surface intercept tracing. The sun look direction vector
  is computed from a successful observer/look direction surface intercept.
- __PRQPhotometricTraceArray__ contains an array of photometric trace instances
  containing a vector photometric ray traces.
- __PRQFacet__ Retrieves the mesh facet datum from the tracer of intercept if it
  exists. If it does not exist, then PsmrtsRequest::was_invoked() returns false.

`PsmrtsRequest` contains methods that return timing information from the time a
process() method was invoked and returned (see runtime_ms()), now many times a
process method was called (see run_count()) and if any errors occurred (see
error_count()) among other information. It is recommend these PRQs be utilized
and developed in tracers. 

There is a special template class called __MissingProcessRequestHandler__ that
is part of a defined macro called __PSMRTS_PROCESS_CATCHALL()__ that is designed
to be added at the bottom of the ray tracer classes to trap all unimplemented
`process( PRQ )` methods and log errors and tracking data.

@subsubsection priorityTracerCreation PsmrtsPriorityTracer Creation - The Ultimate PSMRTS Tracing System

In previous discussion of PSMRTS product creation, the products created are
unique instances of shapes and tracers. One of the most critical properties of
PSMRTS is the sharing of products to minimize use and manage memory resources.
The [PsmrtsFactory](../../psmrts/core/PsmrtsFactory.hpp) class maintains a
system-wide inventory of shapes, tracers and priority tracers primarily for reuse
purposes. The fundamental storage mechanism is the
[PsmrtsInventory](../../psmrts/core/PsmrtsInventory.hpp) database. It consists of
product storage containers of PSMRTS shapes, tracers, priority tracers and file
path translators. 

@subsection filePathTranslations File Path Translations

File path translators, called
[PsmrtsTranslations](../../psmrts/core/PsmrtsTranslations.hpp) contain OS shell
environment variables and ISIS DataDirectory-like parameters that are primarily used
to substitute instances of file path elements containing tags that start with a
"$". The file path substitution algorithm performs a parameterized count of
iterations of parameter/environment file path substitutions to ensure multiple
levels or substitutions are expanded properly. These are applied automatically
in the configuration/specification processing phase of product creation. The
expanded paths are stored in configuration _metadata_ containers.

@subsection psmrtsInvoice Resource Management Strategies

The [PsmrtsInvoice](../../psmrts/core/PsmrtsInvoice.hpp) class is the primary
resource management system that should be used to take full advantage of all
PSMRTS capabilities. It contains a local PsmrtsInventory of all shapes, tracers,
priority tracers and translators for maximum efficiency. It also integrates use
of the system-wide PSMRTS factory system for centralized and coordinated
resource management. See
[ProductProcessing](../../psmrts/core/products/ProductProcessing.hpp) for
additional details.

PsmrtsInvoice applies a factory/invoicing model to create priority tracers from
all existing resources. Here is an example of how to utilize this resource to
create the ultimate tracing system of multiple resources and tracing conditions.

```
#include <psmrts/core/PsmrtsUtilities.hpp>
#include <psmrts/shapes/PsmrtsShape.hpp>
#include <psmrts/tracers/PsmrtsTracer.hpp>
#include <psmrts/core/products/ProductOption.hpp>
#include <psmrts/core/products/ProductConfiguration.hpp>
#include <psmrts/core/products/ProductSpecification.hpp>
#include <psmrts/core/products/ProductOrder.hpp>
#include <psmrts/core/products/ProductMaker.hpp>
#include <psmrts/core/products/ProductCart.hpp>
#include <psmrts/core/products/ProductProcessing.hpp>
#include <psmrts/core/PsmrtsInvoice.hpp>

namespace psmrts { 
  using UIDType = psmrts::PsmrtsInventory::UIDType;

  PsmrtsTranslations trans_t = psmrts::PsmrtsTranslations::create();
  PsmrtsInvoice invoice_t( "myisiscube", trans_t );

  std::string objfile = "bennu_g_00880mm_alt_obj_0000n00000_v021.obj";
  ProductConfiguration bullet_t("bulletmaker");
  bullet_t.add( psmrts::ProductOption( "shape", "obj" ) );
  bullet_t.add( psmrts::ProductOption( "obj_file", objfile ) );
  bullet_t.add( psmrts::ProductOption( "tracer", "bullet" ) );
  invoice_t.add_product( bullet_t );

  std::string dskfile = "bennu_g_00400mm_alt_ptm_0000n00000_v021.bds";
  ProductConfiguration naifdsk_t("naifdskmaker");
  naifdsk_t.add( psmrts::ProductOption( "tracer", "naifdsk" ) );
  naifdsk_t.add( psmrts::ProductOption( "dsk_file", dskfile)  );
  invoice_t.add_product( naifdsk_t );

  ProductConfiguration ellipsoid_t("ellipsoidkmaker");
  ellipsoid_t.add( ProductOption( "tracer", "ellipsoid" ) );
  ellipsoid_t.add( ProductOption( "radii", { 0.283065, 0.271215, 0.249720 } ) );
  ellipsoid_t.add( ProductOption( "name", "bennu" ) );
  invoice_t.add_product( ellipsoid_t );

  PsmrtsPriorityTracer priority_t = invoice_t.get_priority_tracer( );

} // namespace psmrts
```

This example shows three separate instances of representations of Bennu shape
models in one priority tracer. You can also apply algorithms to order shapes
directly in a priority tracer using the PsmrtsPriorityTracer::prioritize(). See
the PsmrtsPriorityTracer::reverse_order() method to apply a lambda reverse the
order of tracers in a priority tracer.

@subsection capi C API

This framework lays the foundation for the PSMRTS C/C++ and user interface. It is not lost that some configurations can be quite complex in conveying parameterization for each product requested by a user - particularly when using text strings only. Configurations using `ellisoid`, `spheroid` or `sphere` products are the most basic types. The string configurations expected from the user and applied in the `PSMRTS` C api has the following form:

```
const char *ellipsoid_s = "tracer=ellipsoid;radii=[0.283065,0.271215,0.249720]"; /* or ellipsoid_radii=... */
const char *spheroid_s  = "tracer=spheroid;radii=[0.283065,0.249720]"; /* or spheroid_radii=[0.283065,0.249720] */
const char *sphere_s    = "tracer=sphere;radii=250.0";  /* or sphere_radius=250 */
```
These examples are consistent with configuration strings specified by users that are absorbed and processed by `PSMRTS` that results in a tracer object that either exists in the `PSMRTS` cache (for efficient sharing of resources) or is created anew and added to `PSMRTS`'s internal caching system for efficient reuse. (__OPTION__: Should users be able to opt out of caching their product and return an unshared product at create time?)

So how does this look in a `PSMRTS` C application? The following is a simple C main example demonstrating initialization of the PSMRTS factory system (with the default factory), create an ellipsoid, trace an arbitrary observer/lookdir, compute observational geometry from sun position and finally clean up resources and shut down the factory. This is similar to the [PROJ C API](https://proj.org/en/stable/development/quickstart.html).

```
#include <stdio.h>
#include <psmrts_c.h>

int main( int argc, char *argv[] ) {

  PSMRTS_Factory       *p_factory;
  PSMRTS_Tracer        *ellipsoid;
  PSMRTS_RayTrace      *ray, *sunray;
  PSMRTS_Vector3d      observer, lookdir, sunpos, sundir, position_v, look_v;
  PSMRTS_Vector3d      emission, incidence, phase, normal, sepang, xyz, surfpt, radlonlat;

  double slant_d, surft_dist;
  double radius_km;

  /* Get the standard factory from PSMRTS to kick things off... */
  p_factory = psmrts_get_factory();

  /* Create an ellipsoid tracer */
  const char *ellipsoid_s = "tracer=ellipsoid;ellisoid=[0.283065,0.271215,0.249720]";
  ellipsoid = psmrts_create_tracer_from_string( p_factory, ellipsoid_s );
  if ( !psmrts_tracer_isvalid( ellipsoid ) )  ) {
    printf("\n*** PSMRTS-C - create errors:\n%s\n", psmrts_tracer_error_str( ellipsoid ) );
    exit ( 1 );
  }

  /** Trace a ray */
  observer = psmrt_vector3d( 0.3, 0.0, 0.0 );
  lookdir  = psmrt_negate( observer );

  /* Trace a ray on the ellipsoid. Create a reusable ray structure  */
  /* to minimize memory create/free overhead. */
  ray = psmrts_create_ray( observer, lookdir );
  ray = psmrts_ray_trace( ray, ellipsoid );
  if ( psmrts_ray_has_hit( ray ) ) {

    /* Retrieve/calculate data from trace */
    xyz       = psmrts_ray_xyz( ray );
    surfpt    = psmrts_ray_surfpt( ray );

    normal    = psmrts_ray_normal( ray );

    slant_d   = psmrts_ray_slant_distance( ray );
    surfpt_d  = psmrts_norm( surfpt );

    radius_km = psmrts_ray_radius( ray );
    radius_pt  = psmrts_norm( xyz );

    radlonlat = psmrts_xyz_to_geo( xyz ); /* Consistent with NAIF */

    /* Use sunpos to get observational geometry */
    sunpos = psmrt_vector3d( 300, 1000, 2000 );
    sundir = psmrts_subtract( psmrts_ray_xyz( ray ), sunpos );

    /* Trace from sun position to surface intercept point */
    sunray = psmrts_ray_trace( psmrts_create_ray( sunpos, sundir ), ellipsoid );
    if ( psmrts_ray_has_hit( sunray ) ) {
      emission  = psmrts_emission( ray );
      incidence = psmrts_incidence( ray, sunray );
      phase     = psmrts_phase( ray, sunray );
    }
    else {
      printf("\n*** PSMRTS-C - Trace from sun failed!\n");
    }
  }

  /* Resource cleanup and factory shutdown */
  psmrts_free_ray( ray );
  psmrts_free_ray( sunray );
  psmrts_free_tracer( ellipsoid );
  psmrts_shutdown_factory( p_factory );

  return ( 0 );
}

```
This implies `psmrts_create_tracer_from_string()` always returns an allocated structure that requires memory to be deleted. This is also true of `psmrts_ray_trace( )`.

Note that this style of a C API tends to play nicely with C++, particularly memory management. The pointer based variables can be configured with explicit memory resource release function specified in the creation of a shared point. The recommended form for C++ using this configuration are similar to this:
```
std::unique_ptr<PSMRTS_Factory> factory( psmrts_get_factory(), psmrts_shutdown_factory );
std::shared_ptr<PSMRTS_Tracer>  ellipsoid( psmrts_create_tracer_from_string( p_factory, ellipsoid_s ), psmrts_free_tracer );
std::shared_ptr<PSMRTS_RayTrace>     ray( psmrts_ray_trace( ellipsoid, observer, lookdir ), psmrts_free_ray );
```
This form works well to ensure your applications are neatly memory manageable.

@subsubsection vector3d PSMRTS_Vector3d C Structure

You can see where the PSMRTS_Vector3d plays a large part in the code above. This structure is nothing more that an analogy to Eigen::Vector3d. It is defined as a union of several convenient forms of 3 double precision data values.
```
typedef union {
  struct {
    double x;
    double y;
    double z;
  };
  struct {
    double a;
    double b;
    double c;
  };
  struct {
    double longitude;
    double latitude;
    double radius;
  };   
  double data[3];
} PSMRTS_Vector3d;
```
You can see a similar approach in this <a href="https://proj.org/en/stable/development/reference/datatypes.html#c.PJ_COORD" target="_blank" rel="noopener">PJ_COORD</a> structure in the PROJ system. We may need to have formal types for some of the structs just as PROJ has. Note, just to be sure, we must add a Catch2 REQUIRES test to confirm the structure is properly handled by compilers.
```
REQUIRE( sizeof(PSMRTS_Vector3d) == (3 * sizeof(double)) );
```

@subsubsection raytrace PSMRTS_RayTrace C Structure

The `PSMRTS_RayTrace` type consists of observer position and look direction vectors. Both vectors are provided in units of kilometers (km) although the direction vectors are typically unitless and can be normalized. `PSMRTS_RayTrace` is an opaque pointer to a ray tracer object. In this context, they are actually _PSMRTS request functor_ (PRQ) objects. Specifically, the `PSMRTS_RayTrace` C API type is actually mapped to the `PRQRayTrace` functor object. These PRQ functor objects are well suited for this feature as they all contain inherent error checking/catching, with full accounting of its execution processing. All PRQ functors are contained in the header file `PsmrtsRequest.hpp`.

To implement the opaque pointer method using this technique, declarations occur in both the psmrts_c.h and psmrts_c.cpp files. Keep in mind that content of `psmrts_c.h` must typically contain only code that can be compiled by both the C and C++ compilers. `psmrts_c.cpp` contain C++ elements that define the C++ interface.

