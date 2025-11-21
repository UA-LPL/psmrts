# Add sources current directory to include path
target_sources(psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES PsmrtsPLYFormat.hpp )

add_subdirectory(miniply)