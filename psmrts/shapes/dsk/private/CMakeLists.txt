# Add sources current directory to include path
target_sources(psmrts PUBLIC
                         FILE_SET psmrts_headers 
                           TYPE HEADERS 
                           FILES PsmrtsDSKFormat.hpp )
