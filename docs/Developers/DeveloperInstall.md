  @page developerInstall Installation

  PSMRTS dependencies are provided by the Microsoft Visual Code package manager, vcpkg. Packages used by PSMRTS are contained in the vcpkg.json file. Currently there are few dependencies and they are generally stable so vcpkg is not included as a submodule or specifically included in the repo. Rather, it is installed during PSMRTS installation and packages are installed/built when the PSMRTS is built. This may create issues in the future so be aware of the potential for vcpkg install/build problems as new dependencies are added and systems evolve. We will attempt to maintain concurrency with vcpkg updates as we continuously build our system.

  - To clone the entire PSMRTS repository: git clone https://github.com/UA-LPL/psmrts.git
  - To pull a particular branch: git clone -b feature/psmrts-api-first-light https://github.com/UA-LPL/psmrts.git 
