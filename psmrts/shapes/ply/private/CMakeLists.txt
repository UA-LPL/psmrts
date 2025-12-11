# Add sources current directory to include path
target_sources(psmrts PRIVATE PlyShape.cpp
                              PsmrtsPLYFormat.hpp
)

add_subdirectory(miniply)