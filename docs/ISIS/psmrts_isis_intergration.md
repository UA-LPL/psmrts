# <a name="#psmrts_isis_integration">PSMRTS ISIS Intergration</a>
The Planetary Shape Model and Ray Tracing System (PSMRTS) has been developed to make use of a variety of ray tracing libraries for image orthorectification on planetary shape models. These models are typically in the format of tesselated plate models. They can be in a variety of formats and resolution, including regional and global coverage, requires a flexible, managed approach for multiple models for a single image. Many cartographic mapping and geometric operations prioritize ray traces on a set of these models for effective utility of the large variety of these types of models and provide a highly accurate and flexible management system of the models and tracing operations.

During the OSIRIS-REx mission to Bennu, there was a significant effort to accurately model Bennu for a successful sample collection from the surface. Accurate modelling of Bennu's surface was required in order to ensure the safety of the spacecraft during descent and collection of the sample.

As a results there were more than 50 versions of shape models released consisting of a variety of resolutions (44cm to ~12.5m global, 5cm and 20cm regional models) and coverage areas. This created a significant challenge of effective and comprehensive use of these products to proccess and produce high quality and accurate orthorectified maps and other cartographic products and functionality.

PSMRTS is the result of software originally developed in the Integrated Software for Imagers and Spectometers (ISIS) that was utilized in mapping Bennu at very high resolution (1mm pixel resolution local, 5cm global mosaics) and contributed to the successful sample acquisition on October 20, 2020. Ultimately, every shape model produced by the mission can be used in this system. We anticipate an ongoing and increasing need for this type of capabiity for current and future NASA missions, particularly small, irregularly shaped bodies such as asteroids, comets, moons and other small bodies. And because of PSMRTS' ability to use and prioritize multiple shape models per image, it is well suited for large bodies using many small, regigional shape models.

## <a name="#psmrtsrelease">PSMRTS in the ISIS Environment</a>
This document describes the first application of PSMRTS in an image processing environment. The PSMRTS C++ API was used to develop an implemetation in ISIS that fits into the [ShapeModel](https://github.com/DOI-USGS/ISIS3/tree/dev/isis/src/base/objs/ShapeModel) class design. This is the obvious first choice since it was initially developed in an older version of ISIS.

PSMRTS is integrated as a project within a project serving as an ISIS dependency. The current PSMRTS API is C++ only, however, a C API is planned to promote wide community support. Efforts we made to ensure the [PSMRTS footprint in ISIS](https://github.com/KrisBecker/ISIS3/commit/3b83702234a90fec6649cdf453e1cd8e9b92e83b#diff-210a370df9b62aa88783259030be7430ce12081393baca6b5fd69c66cc5241a2) is small and effective. The ISIS system contains a PSMRTS-based ShapeModel class that currently supports the NAIF DSK format of tessellated plate models. The [PsmrtsIsisShapeModel](http://orgit:7990/projects/ISIS/repos/psmrts/browse/isis/src/base/objs/PsmrtsIsisShapeModel?at=refs%2Fheads%2Ffeature%2Fpsmrts-isis-integration) was developed to utilize the PSMRTS sytem in the ISIS envinment. There will be support added for other formats including OBJ, as well as framework to easily include other ray tracing libraries, such as Bullet and Embree, creating options to fit the needs of an evolving capability for mapping planetary bodies.

To use the `PSMRTS` shape model, you must select it using an ISIS Preferences file, that contains the selection for the PSMRTS ray tracing engine. Set `RayTraceEngine = PSMRTS` to choose the PSMRTS shape model in ISIS. The `OnError = Fail` means that if `PSMRTS` errors out, it will not try the rest of the models. `Tolerance = 1.0e-6` specifies the maximum distance in km on any surface that is deemed the same intercept point. The value of `1.0e-6` is 1mm precision which means that intercepts from the s/c (observer) and the is Sun to detect shadows must intersect the surface with a 1mm radius of the s/c intercept point or its in the shadows. Again, not the only format of the shape models supported is NAIF DSK. Here is an example of the preferences file that only needs to be provided when you run `spiceinit`. After that, it will automatically select `PSMRTS`. Here is a preferences files called `PsmrtsEngineSelect.pref`:

```
# PsmrtsEngineSelect.pref
Group = ShapeModel
  RayTraceEngine = Psmrts
  OnError = Fail
  Tolerance = 1.0e-6
EndGroup

End
```

The `PsmrtsIsisShapeModel` provides the ability to load more than one DSK for any image. The list of images is provided in an external file with a `.conf` file extention. It must contain the `ShapeModel` keyword with the list of (DSK) shape models to load. The content and format of the `.conf` shape model configuration file will certainly evolve. Here is an example of a file called `two_dsks.conf`:

```
# two_dsks.conf
Group = ShapeModel

# Use both the 5cm regional and 88cm global models
  ShapeModel = ( $osirisrex/kernels/dsk/bennu_l_00050mm_alt_dtm_5595n04217_v020.bds, 
                 $osirisrex/kernels/dsk/bennu_g_00880mm_alt_obj_0000n00000_v020.bds )

# Use only the 88cm global
# ShapeModel = $osirisrex/kernels/dsk/bennu_g_00880mm_alt_obj_0000n00000_v020.bds

# Use the distributed 5cm DTM
# ShapeModel = $osirisrex/kernels/dsk/bennu_l_00050mm_alt_dtm_5595n04217_v020.bds

# UA local version of the 5cm using Poisson reconstruction
#  ShapeModel = /opt/isis3/data/osirisrex/kernels/dsk/l_00050mm_alt_ptm_5595n04217_v020.bds

EndGroup
End
```

`PSMRTS/ISIS` has been test on a OREx PolyCam image near the sample collection site of a [Recon](https://sbnarchive.psi.edu/pds4/orex/orex.ocams/data_calibrated/recon/) phase image, [20191026T215933S071_pol_iofL2pan](https://sbnarchive.psi.edu/pds4/orex/orex.ocams/data_calibrated/recon/20191026T215933S071_pol_iofL2pan.fits). After you have initialized PSMRTS/ISIS in the preferred manner, you can run the following set of commands:

```
#!/bin/sh
# process_orex_ocams.sh

# Run PSMRTS/ISIS on an OSIRIS-REx OCAMS PolyCam image near the sample site
wget -P . https://sbnarchive.psi.edu/pds4/orex/orex.ocams/data_calibrated/recon/20191026T215933S071_pol_iofL2pan.fits

ocams2isis from=20191026T215933S071_pol_iofL2pan.fits to=20191026T215933S071_pol_iofL2pan.cub

spiceinit from= 20191026T215933S071_pol_iofL2pan.cub shape=user model=$PWD/two_dsks.conf  -pref=PsmrtsEngineSelect.pref

campt from=20191026T215933S071_pol_iofL2pan.cub

phocube from=20191026T215933S071_pol_iofL2pan.cub DN=true LOCALEMISSION=true LOCALINCIDENCE=true pixelresolution=true to= 20191026T215933S071_pol_iofL2pan.pho.cub
```

The custom way to run spiceinit works as expected using the following form
```
spiceinit from=20191026T215933S071_pol_iofL2pan.cub shape=user model='$osirisrex/kernels/dsk/bennu_l_00050mm_alt_dtm_5595n04217_v020.bds' -pref=PsmrtsEngineSelect.pref
```

It is important to consider the consequences of using a `.conf` configuration file. First, it is recommended to always provide a full path the config file. This because that filename is also recorded in the cube label for subsequent reference and ISIS must be able to find it even when it is copied. And this leds to the second issue: you can edit this file at any time and switch to any other shape model supported by ISIS.


Instructions to build `PSMRTS` follow.

## <a name="#fordevelopers">ISIS Resources - What Developers Need To Know</a>
There are considerable resources for ISIS developers available to assist development processes using the [Astrogeology/ISIS3](https://github.com/DOI-USGS/ISIS3) repository. Adding your software contribution to the ISIS repository takes considerable understanding and compliance with DOI, USGS, Astrogeology polices and best practices of open source software development standards. When accepted, USGS/Astro will then maintain the software. Again, please study the process before you begin your development work.

### <a name="#helpfulreferences">ISIS GitHub Repository and Helpful References</a>
  - [Astrogeology/ISIS3](https://github.com/DOI-USGS/ISIS3)
  - [Submit ISIS Bug Reports/Issues](https://github.com/DOI-USGS/ISIS3/issues)
  - [Submit ISIS Pull Request (PR)](https://github.com/DOI-USGS/ISIS3/pulls)
  - [Developing ISIS3 with cmake](https://github.com/DOI-USGS/ISIS3/wiki/Developing-ISIS3-with-cmake) (builds/installs `$ISISROOT`)
  - [Writing ISIS Tests Using GTest and CTest](https://github.com/DOI-USGS/ISIS3/wiki/Writing-ISIS-Tests-Using-Gtest-and-Ctest)
  - [The ISIS Data Area](https://github.com/DOI-USGS/ISIS3#the-isis-data-area) (`$ISISDATA`)
  - [ISIS Test Data](https://github.com/DOI-USGS/ISIS3#isis-test-data) (`$ISISTESTDATA`)
  - [ISIS Discussion Forums](https://github.com/DOI-USGS/ISIS3/discussions)
  - [ISIS Technical Committee](https://github.com/planetarysoftware/ISIS_TC) (ISIS/TC)


## <a name="#macsetup">Mac Configuration for ISIS Development</a>
The Apple Mac platform provides a well-suited platform for ISIS development that is not too complicated to setup. The basic requirement is to install the latest version of Xcode to get started. Note that development on the new ARM platform is not fully supported but is possible. The main reason for this is that Anaconda, the basic development and installation environment for ISIS, does not have MAC ARM versions of many of the required software libraries/packages for the platform. In addition, the USGS maintains several ISIS dependencies that are not supported on the Mac ARM platform.

If you have a Mac ARM computer you wish to use for ISIS development, please see this [ISIS issues post](https://github.com/DOI-USGS/ISIS3/issues/5188) regarding how to set up your Mac to enable ISIS development on the ARM platform.

If you do not have a Mac ARM platform, after the basic Xcode installation, you should be able to begin ISIS development. However, I recommend you review the ISIS resources for additional information regarding ISIS development details.

## <a name="#isisdevsetup">ISIS Installation and Development Setup</a>
Generally, it s recommended to follow the USGS/Astro ISIS [development guides](https://github.com/DOI-USGS/ISIS3/wiki/Developing-ISIS3-with-cmake) to create an ISIS environment (i.e., a local development branch) on your computer. In general, occasionally the need to have at least several ISIS branches active does occur. To help manage these situations it is recommended to determine a naming method that is easily associated with your ISIS working directory. One method that we have used is to name an ISIS working directory using, for example, the date of the form `Dec082023` to clone the ISIS main development branch. You can prepend or append additional text to indicate the theme of the work being done in this branch. That may be helpful if you have more than one ISIS working directory going on that particular day. **Important** It is highly recommended that you always create the required ISIS development Conda environment the same name as the ISIS working directory. This is to avoid confusion about which conda environment is used for any given ISIS working directory. Note that USGS/Astro may change the configuration in the `main` development repo branch.  That can break your environment when you merge the USGS/ISIS `main` branch into your working branch, which is required before you issue a PR to merge your work into the USGS/ISIS repo.

### <a name="#minicondasetup">Installing Anaconda</a>
A Conda-based Python environment is required to develop in the ISIS system. USGS/Astro suggests you install [Anaconda](https://www.anaconda.com/download/). However, I prefer using [Miniconda](https://docs.conda.io/projects/miniconda/en/latest/). The main reason I prefer Miniconda over Anaconda is that the Miniconda Python environments are much smaller than Anaconda environments. Anaconda installs a lot of packages in every environment created. Miniconda does not install very many packages in your environment when created. This keeps resources at a minimum and serves as check for a minimal ISIS install regarding Conda environments.

For Mac systems, I use the Bash 64-bit release to install Miniconda.

1. Run the `Terminal Rosetta` app.
    - `uname -m` should report `x86_64`
2.  Download and install the Bash version of [Mac Intel Miniconda](https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh)
    - `bash Miniconda3-latest-MacOSX-x86_64.sh`
3. Configure conda according to the ISIS installation instructions. This is mainly to setup the proper channels and establish priority.
    - `conda config --env --add channels conda-forge`
    - `conda config --env --add channels usgs-astrogeology`

At this point, your Mac system should be configured to develop ISIS.

### <a name="#psmrtsisissetup">Installing and Building PSMRTS/ISIS</a>
It is recommended to create a dedicated directory to contain all ISIS working development directories. One possible configuration would be `mkdir -p ~/ISIS/GitCheckOuts`. All ISIS working directories would then be installed in `~/ISIS/GitCheckOuts`. The commands to install and configure an ISIS build for our purposes are slightly different that what is presented/suggested in the USGS/Astro [instructions](https://github.com/DOI-USGS/ISIS3/wiki/Developing-ISIS3-with-cmake#building-isis3) to build ISIS. Following either procedure will work. Below are the instructions to install, configure and build PSMRTS/ISIS. 

**NOTE** that in order to correctly install `PSMRTS`, you must log into UA/OREX VPN. The integration/installation of PSMRTS is made through the CMake procedure [FetchContent](https://cmake.org/cmake/help/v3.28/module/FetchContent.html#command:fetchcontent_declare) and refers to our BitBucket server. `PSMRTS` installation will error during the cmake build configuration if the you are not logged into the UA VPN and have been granted access to the git server. We expect this change soon as we now have both a GitLab and GitHub accounts for university research that will become the home for `PSMRTS`. Please contact me if you have an problems.

1. Log into the UA/VPN
1. cd ~/ISIS/GitCheckouts
1. mkdir PsmrtsIsis
1. cd  PsmrtsIsis
1. git clone -b feature/psmrts-isis-integration --recurse-submodules https://github.com/KrisBecker/ISIS3.git
1. cd ISIS3
1. conda env create -n PsmrtsIsis -f environment.yml
1. conda activate PsmrtsIsis
1. mkdir build install
1. export ISISROOT=$PWD/build
1. export ISISINSTALL=$PWD/install
1. export ISISDATA=/opt/isis/data
1. export ISISTESTDATA=/opt/isis/testData
1. cd build
1. `cmake  -DJPK2KFLAG=OFF -DisisData=/opt/isis/data -DisisTestData=/opt/isis/testData -DJPK2KFLAG=OFF -DCMAKE_INSTALL_PREFIX=$ISISINSTALL -DCMAKE_BUILD_TYPE=RELEASE  -GNinja  ../isis`
1. ninja install

This configuration is integrated into version 8.1 pulled from `dev`, on March 19, 2024. You should be able to merge this branch with any other ensuing ISIS version, but that has not be tested. Yout may certainly encounter merge conflicts.

You can also use the following command to clone a clean version of `PSMRTS` and run the Catch2 tests.

```
# Log into the UA OREX VPN
git clone -b feature/psmrts-isis-integration http://orgit:7990/scm/isis/psmrts.git
cd psmrts
./build_psmrts.sh -t
cd build
ctest
```

No other setup is required.

### <a name="#issues">Outstanding Issues</a>
During this work, we encountered several issues that I hope we can discuss.

1. For some reason, the radii values in the Target object passed to ShapeModelFactor never has valid values. They are always Null. I had to go to extreme lenghts to get valid values, which ulitimately come from the Pvl object - the ISIS label! The radii from all sources but the Pvl label are Null() and crash PSMRTS/ISIS. This because an ellipsoid shape model is instantiated from the Target. I thought I would try the Spice object but it gets its radii from Target and is also Null(). I then try to use NAIF with a direct call to `bodvar` and it also fails. The only work around is to extract the radii from the ISIS label provide by pvl. This is concerning and could easly be my implementation. Nonetheless, it might be worth seeing if this occurs for other models.
1. I found the ShapeModel class API very confusing. What's the difference between the hasEllipsoidIntersection(), hasNormal() and hasLocalNormal()? Is the ellipsoid normal the same as the normal?
1. There are inherent restrictions in the API that does not provide virtual options to critical processes that may need specialization.
1. There appears to be some redundant methods that interact with the same variables but have conflict behaviors.

And finally, the CMake intergration needs help. It is quite messy now but works in pinch. It is not at all clean and it could use some polishing up.

