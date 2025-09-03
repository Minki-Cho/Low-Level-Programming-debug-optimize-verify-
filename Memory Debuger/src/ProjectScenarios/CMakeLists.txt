# Create a variable called MODULE that contains the name of the module
set(MODULE ProjectScenarios)
# Create a variable called MODULE_DEPENDENCIES that contains the module dependencies
set(MODULE_DEPENDENCIES MemDebugger)

# Collect sources into the variable SRC_FILES with 
# having to explicitly list each header and source file
file (GLOB SRC_FILES
      "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# Set Properties->General->Configuration Type to Application(.exe)
# Creates ${MODULE}.exe with the listed sources (main.cxx)
# Adds sources to the Solution Explorer
add_executable (${MODULE} ${SRC_FILES})

# Properties->C/C++->General->Additional Include Directories
target_include_directories (${MODULE} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Properties->Linker->Input->Additional Dependencies
target_link_libraries (${MODULE} ${MODULE_DEPENDENCIES})

# Creates a folder "executables" and adds target 
# project (${MODULE}.vcproj) under it
set_property(TARGET ${MODULE} PROPERTY FOLDER "executables")

# Properties->General->Output Directory
# set_target_properties(${MODULE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Adds logic to INSTALL.vcproj to copy app.exe to destination directory
install (TARGETS ${MODULE}
         RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)
