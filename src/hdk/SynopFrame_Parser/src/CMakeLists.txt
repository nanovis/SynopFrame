cmake_minimum_required( VERSION 3.6 )

project( SynopSpace_Parser )

list( APPEND CMAKE_PREFIX_PATH "C:/Program Files/Side Effects Software/Houdini 19.5.435/toolkit/cmake")

# Locate Houdini's libraries and header files
# Registers and imported library target named 'Houdini'
find_package( Houdini REQUIRED )

# the name of the library to be generated 
set( library_name SynopSpace_Parser )

# depends on whether there exists DS snippet in the source code 
# enable the following line if there is 
# houdini_generate_proto_headers( FILES source.C )

add_library( ${library_name} SHARED
    GEO_oxdna_dat_Translator.C

    GEO_synopspace_hb_Translator.h
    GEO_synopspace_hb_Translator.C

    GEO_synopspace_custom_Translator.h
    GEO_synopspace_custom_Translator.C
)

# Link against the Houdini libraries, and add required include directories and 
# compile definitions 
target_link_libraries( ${library_name} Houdini)

# Include ${CMAKE_CURRENT_BINARY_DIR } for the generated header.
target_include_directories( ${library_name} PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Sets several common target properties, such as the library's output directory.
houdini_configure_target( ${library_name} )

