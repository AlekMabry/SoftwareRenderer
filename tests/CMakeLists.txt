include_directories (${PROJECT_SOURCE_DIR}/SGL)
include_directories (${PROJECT_SOURCE_DIR}/SGLExt)
include_directories (${PROJECT_SOURCE_DIR}/tests/include/unity)

add_executable(TestVectors "tests/TestVectors.c" "lib/unity/*.c")

# Properties->Linker->Input->Additional Dependencies (.lib)
target_link_libraries(LoadTexture SGL SGLExt)

set_property(TARGET TestVectors PROPERTY FOLDER "tests")

# Properties->General->Output Directory
set_target_properties(TestVectors PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

install (TARGETS TestVectors
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install)