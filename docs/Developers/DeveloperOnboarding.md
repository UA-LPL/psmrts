@page developerOnboard Developer Onboarding
@tableofcontents

This document is a HOWTO of sorts to aid/guide developers in the process and procedures to contribute to PSMRTS. Topics covered here include: configuring a development computer system (this covers the Mac platform for now), acquiring and familiarization of resources and tools, installing PSMRTS for development, and setting up the development environment. These processes will enable the PSMRTS developer to create an PSMRTS system, make changes to existing code and/or add new software to PSMRTS, build and install PSMRTS, develop unit and application tests, and add documentation. These activities are all performed on your local system. The next step is to submit your contributions to the USGS/Astro PSMRTS repo for distribution.

@section sectionDeveloperKnowledge What Developers Need To Know

There are considerable resources for PSMRTS developers available to assist development processes. Adding your software contribution to the PSMRTS repository takes considerable understanding and compliance with polices and best practices of open source software development standards. Below are some references to topics and tools that are useful to assist developers in understanding and learning about the PSMRTS development processes.

@section sectionDeveloperResources Developer Resources
- [Rocky Linux](https://rockylinux.org)
- [Bash Shell](https://pubs.opengroup.org/onlinepubs/9699919799/utilities/toc.html)
- [Visual Studio Code](https://code.visualstudio.com) (vscode)
- [Markdown Guide](https://www.markdownguide.org) - The Common Language of Communicating Code
- [Anaconda](https://anaconda.org)/[Miniconda](https://docs.conda.io/projects/miniconda/en/latest/) - Python Environment Management
- [Git]( https://git-scm.com) - Fundamenantal to Most All Public Code Repositories
- [GitHub](https://github.com) - Public Source Code Repository Platform
- [GitLab](https://gilab.com) - Public Source Code Repository Platform
- [CMAKE](https://cmake.org) - Source Code Build System
- [C++](https://cplusplus.com) - Cool!
- [C++ Reference](https://en.cppreference.com/w/) - All About C++ Standards
- [C++ FAQ](https://isocpp.org/faq) - Very Good C++ FAQ
- [vcpkg](https://vcpkg.io/en/) - A C++ Dependency Manager
- [Catch2](https://github.com/catchorg/Catch2) - A C++ Testing Framework


@section sectionMacConfig Mac Configuration for PSMRTS Development

The Apple Mac platform provides a well-suited platform for PSMRTS development that is not too complicated to setup. The basic requirement is to install the latest version of Xcode to get started. Note that development on the new ARM platform is not fully supported in `vcpkg` but does seem to work for most of the current PSMRTS dependencies. The major reason for this is due to lack of ARM compatibility of many of the libraries in the `vcpkg` ports.

If you have a Mac ARM computer you wish to use for PSMRTS development, please see this [ISIS issues post](https://github.com/DOI-USGS/ISIS3/issues/5188) regarding how to set up your Mac to enable ISIS development on the ARM platform. This post is also relevant for any project using `conda` but also projects that use other configurations.

If you do not have a Mac ARM platform, after the basic Xcode installation, you should be able to begin PSMRTS development. However, I recommend you review the PSMRTS resources for additional information regarding PSMRTS development details.


### <a name="#psmrtssetup">PSMRTS Development Procedures</a>
It is recommended to create a dedicated directory to contain all PSMRTS working development directories. One possible configuration would be `mkdir -p ~/PSMRTS/GitCheckOuts`. All PSMRTS working directories would then be installed in `~/PSMRTS/GitCheckOuts`. I tend to name development directories the same as the branch name I will publish the modifications to. There aer several types of development branches that prefix the name of the branch and use a `/` as a separator. For example, the branch `feature/psmrts-raytrace-tests` could indicate its not a bug fix but a new `feature`` or contribution to PSMRTS. The name of the branch, `psmrts-raytrace-tests`, indicates the changes provide Catch2 tests for ray tracing operations. Here is a sequence to check out and build this branch:

1. cd ~/PSMRTS/GitCheckouts
1. mkdir PsmrtsRaytraceTests
1. cd  PsmrtsRaytraceTests
1. git clone http://github.com/UA-LPL/psmrts.git
1. cd PSMRTS
1. ./make_install.sh -x -t

Here the `-t` will build tests and `-x` will build extras. Building tests is needed to ensure you the Catch2 tests framework is enabled for development. `-x` may include support applications and other tools.

### <a name="#psmrtstests">PSMRTS Testing Procedures</a>
PSMRTS uses the [Catch2](https://github.com/catchorg/Catch2) Testing Framework for unit and app testing.


### <a name="#psrmtspullrequests">PSMRTS Pull Requests</a>
To begin an PSMRTS pull request, you must first create an PSMRTS

Once the PR is ready you visit your branch and choose the `Contribute` button and then `Open Pull Request`. This will launch a web GUI that needs to be filled out. Note that your original PSMRTS issue describing this PR update must be added in the **Related Issue** section of the PR request. I also tend to write what was committed in to the PR branch for merge into PSMRTS.

Also, you must make an entry into the [CHANGELOG.md](http://orgit:7990/projects/ISIS/repos/psmrts/browse/CHANGELOG.md) file with a _single_ entry that includes a reference to the PR post.

This process is typically done by _publishing_ your PSMRTS working directory to a repo on your own GitHub account/site. The GitHub website can then be used to create a pull request (PR) for your contribution to the UA/PSMRTS repo `main` development branch. At that point, a UA/PSMRTS developer will conduct a review of your work and give feedback for changes that may need to made before the PR will be accepted and merged.

Any suggestions/requests for change will be made in your original PSMRTS working directory branch and then git `pushed` to the branch you provided in the PR request. The PR will update automatically whenever you change the PR branch.

When the PSMRTS developers decide to accept your contribution, it will be merged into the `main` UA/PSMRTS repo and you can then remove the branch if you prefer (I save all mine).

