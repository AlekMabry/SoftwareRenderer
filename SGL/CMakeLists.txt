file (GLOB SGL_EXPORTS  "*.h")
file (GLOB SGL_DEF      "*.def")
file (GLOB SGL_SOURCE   "sgl/*.h"
                        "sgl/*.c")

# Create named folders for the sources within the .vcproj
source_group ("" FILES ${SGL_EXPORTS})
source_group ("sgl" FILES ${SGL_SOURCE})

include_directories (.)

# Generate Renderer.DLL
add_library(SGL SHARED ${SGL_EXPORTS} ${SGL_SOURCE} ${SGL_DEF} "sgl/instance.h")

set_target_properties(SGL PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

install (TARGETS SGL
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install
    LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/install)