cmake_minimum_required(VERSION 3.16)

# set the project name
project(MemDebugger)

# Declare GoogleTest and Catch2 as optional dependencies
option(ENABLE_GTEST ON)
option(ENABLE_CATCH2TEST OFF)

# Enable GoogleTest or Catch2, we do not need both.  (This is redundant if we set them above, I just want to show you the set command.)
set (ENABLE_GTEST ON)
set (ENABLE_CATCH2TEST OFF)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directories for all targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Print compiler information
message ("Using CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message ("cxx Flags: " ${CMAKE_CXX_FLAGS})
message ("Using CMAKE_CC_COMPILER: ${CMAKE_CC_COMPILER}")
message ("cc Flags: " ${CMAKE_CC_FLAGS})

# Detect clang 
if (CMAKE_CXX_COMPILER MATCHES ".*clang")
  set (CMAKE_COMPILER_IS_CLANGXX 1)
endif ()

# Enable / disable debug code depending upon the build configuration
set (DEBUG_FLAGS "-DDEBUG=1") 
set (RELEASE_FLAGS "-DDEBUG=0") 

# Enable debug code when CMAKE_BUILD_TYPE=Debug
if (CMAKE_BUILD_TYPE MATCHES Deb)
  message ("debug mode")
  if (MSVC)
    set (DEBUG_FLAGS "${DEBUG_FLAGS} /Zi")
  endif ()
  if (CMAKE_COMPILER_IS_GNUCXX)
    set (DEBUG_FLAGS "${DEBUG_FLAGS} -rdynamic -g")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
  endif ()
  if (CMAKE_COMPILER_IS_CLANGXX)
    set (DEBUG_FLAGS "${DEBUG_FLAGS} -fshow-source-location -g")
    # set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-export_dynamic")
  endif ()
endif ()

# Create a cmake file that sets the debug flags for each build configuration
file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/flags.cmake" "")
foreach (FLAGS_TYPE C CXX)
  # Enable debug code when CMAKE_BUILD_TYPE=Debug.
  set (FLAGS_VARIABLE CMAKE_${FLAGS_TYPE}_FLAGS_DEBUG)
  file (APPEND "${CMAKE_CURRENT_BINARY_DIR}/flags.cmake"
        "set(${FLAGS_VARIABLE} \"\${${FLAGS_VARIABLE}} ${DEBUG_FLAGS}\")\n")
        # Disable debug code when CMAKE_BUILD_TYPE is a release config.
  foreach (BUILD_TYPE_SUFFIX RELEASE RELWITHDEBINFO MINSIZEREL)
    set (FLAGS_VARIABLE CMAKE_${FLAGS_TYPE}_FLAGS_${BUILD_TYPE_SUFFIX})
    file (APPEND "${CMAKE_CURRENT_BINARY_DIR}/flags.cmake"
          "set(${FLAGS_VARIABLE} \"\${${FLAGS_VARIABLE}} ${RELEASE_FLAGS}\")\n")
  endforeach (BUILD_TYPE_SUFFIX)
  # If no build configuration is specified, default to release flags to align 
  # with cmake's default build configuration for makefiles
  if (NOT CMAKE_BUILD_TYPE AND UNIX AND NOT APPLE)
    set (FLAGS_VARIABLE CMAKE_${FLAGS_TYPE}_FLAGS)
    file (APPEND "${CMAKE_CURRENT_BINARY_DIR}/flags.cmake"
          "set(${FLAGS_VARIABLE} \"\${${FLAGS_VARIABLE}} ${RELEASE_FLAGS}\")\n")
  endif (NOT CMAKE_BUILD_TYPE AND UNIX AND NOT APPLE)
endforeach (FLAGS_TYPE)
include ("${CMAKE_CURRENT_BINARY_DIR}/flags.cmake")

# Set compiler warning flags
set(C_FLAGS_WARNINGS "")
if (MSVC)
  add_compile_options("/EHsc")
  set(C_FLAGS_WARNINGS "/W4 /WX")
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
  # Set the maximum warning level for gcc
  set(C_FLAGS_WARNINGS "-Wall -Werror")
endif ()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_FLAGS_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_FLAGS_WARNINGS}")

# Set the warning flags for all targets when using the Xcode generator
if (CMAKE_COMPILER_IS_CLANGXX)
  foreach(target ${ALL_TARGETS})
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_WARNING_CFLAGS 
                          "${C_FLAGS_WARNINGS}")
  endforeach(target)
endif ()

# Create a subdirectory for each build configuration when building using makefiles
#   This makes it possible to keep multiple build artifacts 
set(OUTPUT_SUBDIR "")
if (CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  if (CMAKE_BUILD_TYPE STREQUAL "")
    set(OUTPUT_SUBDIR "Release/")
  else()
    set(OUTPUT_SUBDIR "${CMAKE_BUILD_TYPE}/")
  endif ()
endif ()

# Override / reset the output directory for all UNIX targets
if (UNIX)
  set(LIB_OUTPUT_DIR "${OUTPUT_SUBDIR}")
  set(EXE_OUTPUT_DIR "${OUTPUT_SUBDIR}")
  foreach(target ${ALL_TARGETS})
    set_target_properties(
      ${target} PROPERTIES 
      ARCHIVE_OUTPUT_DIRECTORY "${LIB_OUTPUT_DIR}" 
      LIBRARY_OUTPUT_DIRECTORY "${LIB_OUTPUT_DIR}" 
      RUNTIME_OUTPUT_DIRECTORY "${EXE_OUTPUT_DIR}")
  endforeach(target)
endif (UNIX)

# set VS startup project
if (MSVC)
  set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ProjectScenarios)
endif (MSVC)

# Add the project subdirectories
add_subdirectory (src/MemDebugger)
add_subdirectory (src/ProjectScenarios)
# add_subdirectory (src/Test)
