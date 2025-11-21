# Add sources current directory to include path
target_sources(psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES PsmrtsOBJFormat.hpp )

add_subdirectory(tinyobjloader)