## PSMRTS helloworld

This example shows how to created a main PSMRTS C++ application in the conda
environment.

To gets started follow the procedures outline in the PSMRTS repo that creates a
PSMRTS conda environment. Then build and install PSMRTS directly in the conda
enviroment. This establishes a base that can be used to develop PSMRST
applications.

The following steps outline how to create a PSMRTS conda development enviroment
and build the helloword PSMRTS application:

1.  git clone https://github.com/UA-LPL/psmrts.git
2.  cd psmrts
3.  conda env create -n psmrts -f psmrts_conda_deps_all.yml
4.  conda activate psmrts
5.  ./make_psmrts.sh -x  -s -V -j4
6.  cmake --install build --prefix $CONDA_PREFIX

To build and run the example helloword app:

1. cd examples/helloworld
2. cmake -B build -S . -DCMAKE_PREFIX_PATH=$CONDA_PREFIX
3. cmake --build build
4. ./build/helloworld
