# Properties

# Include <sgl.h> and <sgl/*.h>
include_directories (${PROJECT_SOURCE_DIR}/SGL)
include_directories (${PROJECT_SOURCE_DIR}/demos/include/SDL2-2.24.2)

file (GLOB SDL2 "lib/SDL2-2.24.2/x64/*.lib")

add_executable(LoadTexture "demos/LoadTexture.c")
add_executable(RenderTriangle "demos/RenderTriangle.c")
add_executable(PBRScene "demos/PBRScene.c")

# Properties->Linker->Input->Additional Dependencies (.lib)
target_link_libraries(LoadTexture SGL ${SDL2})
target_link_libraries(RenderTriangle SGL ${SDL2})
target_link_libraries(PBRScene SGL ${SDL2})

set_property(TARGET LoadTexture PROPERTY FOLDER "demos")
set_property(TARGET RenderTriangle PROPERTY FOLDER "demos")
set_property(TARGET PBRScene PROPERTY FOLDER "demos")

# Properties->General->Output Directory
set_target_properties(LoadTexture PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set_target_properties(RenderTriangle PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set_target_properties(PBRScene PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_custom_target(COPY_ASSETS ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/demos/assets
    ${PROJECT_BINARY_DIR}/bin/assets)

add_custom_target(COPY_SDL ALL
    COMMAND ${CMAKE_COMMAND} -E copy
    ${PROJECT_SOURCE_DIR}/demos/lib/SDL2-2.24.2/x64/SDL2.dll
    ${PROJECT_BINARY_DIR}/bin/)

add_dependencies(LoadTexture COPY_ASSETS COPY_SDL)

install (TARGETS LoadTexture
    RUNTIME DESTINATION )
install (TARGETS RenderTriangle
    RUNTIME DESTINATION )
install (TARGETS PBRScene
    RUNTIME DESTINATION )

install (TARGETS LoadTexture
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install)
install (TARGETS RenderTriangle
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install)
install (TARGETS PBRScene
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/install)