# Create a variable called MODULE that contains the name of the module
set(MODULE Test)
# Create a variable called MODULE_DEPENDENCIES that contains the module dependencies
set(MODULE_DEPENDENCIES MemDebugger)

# Collect sources into the variables INFRASTRUCTURE_FILES and SRC_FILES with 
# having to explicitly list each header and source file
file (GLOB INFRASTRUCTURE_FILES
      "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/include/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/*.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/*.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/*.hpp"
)
file (GLOB SRC_FILES
      "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# Add the Threads module to the project
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Set Properties->General->Configuration Type to Application(.exe)
add_executable (${MODULE} ${SRC_FILES} ${INFRASTRUCTURE_FILES})

# Properties->C/C++->General->Additional Include Directories
target_include_directories (${MODULE} PRIVATE 
                              "${CMAKE_CURRENT_SOURCE_DIR}/include"
                              "${CMAKE_CURRENT_SOURCE_DIR}"
                              "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/include"
                              "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure"
)

# Properties->Linker->Input->Additional Dependencies
target_link_libraries(${MODULE} ${MODULE_DEPENDENCIES})

# Setup CTest
include(CTest)

# Download and unpack googletest at configure time
if (ENABLE_GTEST)
      # Set Properties->General->Configuration Type to Application(.exe)
      # Creates ${MODULE}.exe with the listed sources (main.cxx)
      # Adds sources to the Solution Explorer

      Include(FetchContent)
      FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.14.0 # main
      )
      set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
      FetchContent_MakeAvailable(googletest)
      include(GoogleTest)
      enable_testing()

      # Properties->Linker->Input->Additional Dependencies
      target_link_libraries(${MODULE} gtest_main)

      # add compiler definitions
      target_compile_definitions(${MODULE} PRIVATE ENABLE_GTEST)      

      # must be called for gtest to discover the tests
      gtest_discover_tests(${MODULE})

      # Creates a folder "executables" and adds target 
      # project (${MODULE}.vcproj) under it
      set_property(TARGET ${MODULE} PROPERTY FOLDER "executables")

      # Properties->General->Output Directory
      # set_target_properties(${MODULE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

      # Adds logic to INSTALL.vcproj to copy app.exe to destination directory
      install (TARGETS ${MODULE} 
            RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)
endif (ENABLE_GTEST)


# Download and unpack Catch2 at configure time
if (ENABLE_CATCH2TEST)
      if (ENABLE_GTEST)
            set(MODULE_CATCH "Catch2${MODULE}")

            # Set Properties->General->Configuration Type to Application(.exe)
            # Creates ${MODULE_CATCH}.exe with the listed sources (main.cxx)
            # Adds sources to the Solution Explorer
            add_executable (${MODULE_CATCH} ${SRC_FILES} ${INFRASTRUCTURE_FILES})
            target_include_directories (${MODULE_CATCH} PRIVATE 
                                        "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                        "${CMAKE_CURRENT_SOURCE_DIR}"
                                        "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure/include"
                                        "${CMAKE_CURRENT_SOURCE_DIR}/infrastructure"
            )
            target_link_libraries(${MODULE_CATCH} ${MODULE_DEPENDENCIES})
      else ()
            set(MODULE_CATCH ${MODULE})
      endif ()

      Include(FetchContent)
      FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.4.0 # or a later release
      )  
      FetchContent_MakeAvailable(Catch2)
      list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
      include(Catch)
      enable_testing()

      # Properties->Linker->Input->Additional Dependencies
      target_link_libraries(${MODULE_CATCH}  Catch2::Catch2WithMain)

      # add compiler definitions
      target_compile_definitions(${MODULE_CATCH} PRIVATE ENABLE_CATCH2TEST)

      # must be called for catch to discover the tests
      catch_discover_tests(${MODULE_CATCH})

      # Creates a folder "executables" and adds target 
      # project (${MODULE_CATCH}.vcproj) under it
      set_property(TARGET ${MODULE_CATCH} PROPERTY FOLDER "executables")

      # Properties->General->Output Directory
      # set_target_properties(${MODULE_CATCH} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

      # Adds logic to INSTALL.vcproj to copy app.exe to destination directory
      install (TARGETS ${MODULE_CATCH} 
               RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/_install)
endif (ENABLE_CATCH2TEST)
