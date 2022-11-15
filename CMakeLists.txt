cmake_minimum_required(VERSION 3.10)

project(SoftwareRenderer C)

# Use folders to organize projects (.vcproj)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(SGL)       # SGL.DLL
add_subdirectory(Demos)     # Demo executables