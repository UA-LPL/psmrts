##### Planetary Shape Model and Ray Tracing System (PSMRTS)

## PSMRTS Readme

### `./psmrts/README.md`

## psmrts Overview


#### Installing PSMRTS

The `psmrts` dependencies are provided by the Microsoft Visual Code package manager, [vcpkg](https://vcpkg.io/en/index.html). Packages used by `psmrts` are contained in the `vcpkg.json` file. At this time, there are few dependencies and they are pretty stable so vcpkg is not included as a submodule or specifically included in the repo. Rather, it is installed during `psmrts` installation and packages are installed/built when the `psmrts` is built. This may create issues in the future so be aware of the potential for vcpkg install/build problems as new dependencies are added and systems evolve. We will attempt to maintain concurrency with vcpkg updates as we continuously build our system.

`psmrts` is contained in the PSMRTS repository.
1. `git clone https://github.com/UA-LPL/psmrts.git`

To pull a specific branch, use this form:
1. `git clone -b feature/psmrts-api-first-light https://github.com/UA-LPL/psmrts.git`

#### Configuring the psmrts Development Environment
Building `PSMRTS` requires the `pkg-config` build utility which may not be available on your system. The MacOS does not ship with `pkg-config` and it must be installed. There are several ways to install `pkg-config` such as using [HomeBrew](https://formulae.brew.sh/formula/pkg-config), Anaconda/Miniconda (`conda install pkg-config`) or from [source](https://gitlab.freedesktop.org/pkg-config/pkg-config). The Conda solution will always require an active Python environment. Linux and Windows systems may provide this as a package install or other alternatives.

For the Mac, it is recommended to install from source as it is a better long term solution.  There is a universal binary install package for the Mac located [here](https://github.com/donmccaughey/pkg-config_pkg/releases/tag/v0.29.2-r4). Simply download the PKG file, click on the package and follow the install instructions.


#### Building psmrts

The `psmrts` is contained in the `./psmrts` directory. One command is used to completely build the PSMRTS. However, there are several scripts ran that completes the build.

1. `cd psmrts`
2. `./make_psmrts.sh`
   - Add `-t` to enable testing
   - Add '-c' to enable code coverage

The executable called `psmrts` will be in the ./build directory. It can be copied to any location and ran from anywhere. The script `make_psmrts.sh` runs several other scripts that can be ran separately if needed. Note that these scripts must be run from the `./psmrts` directory.

Should any errors occur, they can be addressed and subsequent building of `psmrts` occurs in the `./build` directory. To continue building `psmrts` from the `./build` directory, just run `make` (the system has already been configured but it may also rerun `cmake` if needed).

The `vcpkg` package manager is downloaded from GitHub and installed at runtime using the `install_vcpkg.sh` script. This script installs the currently available version of `vcpkg` so if something goes wrong with the package build, you can run this script to reinstall `vcpkg` if needed.

`psmrts` dependencies are then built and installed using the `install_vcpkg_packages.sh` script. This script uses the contents of the file `vcpkg.json` that contains `psmrts` dependency packages. This is potentially where to start looking if the build fails for some reason - this part of the build may be the source as package versions could change rather frequently.

And, finally, the `psmrts` is built with the `build_psmrts.sh` script. This script can be ran repeatedly as needed to reconfigure and rebuild `psmrts`. An alternative would be to just remake `psmrts` using the command `make -C build` in the `./psmrts` directory to recompile `psmrts.cpp` and the code (headers) in `./psmrts/psmrts/src`.

#### Testing the psmrts Build

Catch2 is used for testing framework for the `psmrts`. These tests are contained in the `./psmrts/tests` directory. The testing procedures are documented in the `psmrts` test suite [README](./psmrts/tests/README.md). Here are the basic instructions to build tests **after** the initial installation and run the tests.

```
cd $PROJECT_ROOT/psmrts
/bin/rm -rf build
./build_psmrts.sh -t
cd build
ctest
```

#### Code Coverage and Doxygen Documentation in PSMRTS

Code coverage can be ran on psmrts code by providing the `-c` flag to the PSMRTS build scripts. PSMRTS uses a custom CMake code coverage script called [CodeCoverage.cmake](https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake). This file is included in the code repository in the `./cmake` directory.

There are prerequisites for running code coverate on PSMRTS that may need to be installed prior to using this option to build PSMRTS. The [gcovr](https://gcovr.com/en/stable/) utility is used to generate HTML based reports. This utility uses the gcov and lcov scripts to produce HTML based reports that neatly summarizes how many of the lines of code in the current PSMRTS system has been executed. 

For Linux systems, these code coverage utilities are typically installed, however, if the PSMRTS CMake build part of the system fails during configuration/build, then you may need to install these utilities.

For Mac platforms, these utilities are typically not available and need to be installed. You could use [homebrew](https://formulae.brew.sh/formula/gcovr) to install the gcovr/gcov/lcov utilities. I use Minconda to install these utilities on the Mac. You must first install [Miniconda](https://docs.anaconda.com/free/miniconda/miniconda-install/) and then run **conda install gcovr lcov**. You can then run the CMake build to activate code coverage using the `-c` switch while in the conda environment using the following commands:

```
./make_psmrts.sh -t -c
cd build
make coverage
open ./coverage/index.html.
```

The [Doxygen](https://www.doxygen.nl/index.html) configuration file, Doxyfile, is located in the docs directory. To access the documentation enter the following commands:

```
cd docs
doxygen Doxyfile
open ./html/index.html
```
The open commands for each process will load the related interactable HTML reports to the default browser.

#### Running Code Coverage and Doxygen

Activating Code Coverage and Doxygen capabilities for any PSMRTS build first requires a pre-download of gcovr / lcov / Doxygen related utilities. Linux based systems will generally have the lcovr utilities already installed. Mac users can check if it is already available by running `which gcovr` or `which lcov` commands in the terminal. These utilities can be downloaded through Miniconda by running the `conda install gcovr lcov` command. The utility [gcovr](https://gcovr.com/en/stable/), and its' Linux extension lcov, is used to create HTML-based Coverage visualizations for each file, function, and line coded into the PSMRTS system, presented via the user's default browser. Users can get in-depth breakdowns for each file by clicking on their individually designated links in the generated browser window. Doxygen is similar as it also creates an HTML representation of the documentation for each file, including relevant inheretance, collaboration, and call/caller graphs. It requires [doxygen](https://www.doxygen.nl/index.html) and [graphviz](https://graphviz.org) as dependencies.

The build_addons.yml file can be used to create a conda environment that ensures access to these utilities, using the following commands while in the top psmrts directory:
```
conda env create -n <chosen name of environment, eg. MonthDayYear> -f build_addons.yml
conda activate <name of environment from above>
```

Note: Linux users may also use the following commands to ensure lcov/gcovr is installed:
```
sudo apt-get update
sudo apt-get install -y lcov gcovr
```

Enabling gcovr / lcov for any builds requires the addition of `-c` and `-d` in the following command during the cloning process:

```
./make_psmrts.sh -t -c -d
```

To prompt creation of the Coverage report, user must be in the resulting build directory with the `cd build` command and enter the commands below:

```
make coverage
open ./coverage/index.html
```

Linux users may need to rely on alternative open commands, depending on personal browser settings, such as: 

```
firefox ./coverage/index.html
```

Be advised that at the time of these directions, attempting to run multiple ctests with code changes saved between them results in unavoidable errors. Once a correction has been made and saved, you must remove the build directory and rerun the make_psmrts.sh command before another ctest - commands shown below: 

```
cd ..  # (to psmrts directory, if currently in build directory)
/bin/rm -rf build
./build_psmrts.sh -t -c -d
cd build
```

User can then proceed with another ctest.