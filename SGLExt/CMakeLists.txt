file (GLOB SGLEXT_EXPORTS   "*.h")
file (GLOB SGLEXT_SOURCE    "sglext/*.h"
                            "sglext/*.c")
file (GLOB SGL_HEADERS      "sgl/*.h")

# Create named folders for the sources within the .vcproj
source_group ("" FILES ${SGLEXT_EXPORTS})
source_group ("sglext" FILES ${SGLEXT_SOURCE})

include_directories (.)
include_directories (${PROJECT_SOURCE_DIR}/SGL/)

# Generate SGLExt.DLL
add_library(SGLExt SHARED ${SGLEXT_EXPORTS} ${SGLEXT_SOURCE} ${SGL_HEADERS})

set_target_properties(SGLExt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

install (TARGETS SGLExt
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install
    LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/install)