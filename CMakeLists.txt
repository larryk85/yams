cmake_minimum_required(VERSION 3.14)

project(yams VERSION 1.0.0.0)

message( STATUS "Building yams v${yams_VERSION}..." )

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads)
link_libraries(Threads::Threads)

include(FetchContent)

#set(FETCHCONTENT_QUIET OFF)
#FetchContent_Declare(
#	google_protobufs
#	GIT_REPOSITORY https://github.com/protocolbuffers/protobuf
#	GIT_TAG main
#)
#FetchContent_MakeAvailable(google_protobufs)

#FetchContent_Declare(
#   grpc
#   GIT_REPOSITORY https://github.com/grpc/grpc
#   GIT_TAG v1.35.0
#)
#FetchContent_MakeAvailable(grpc)


include(CMakeDependentOption)
option(YAMS_ENABLE_INSTALL "enable installation" ON)
option(YAMS_ENABLE_TESTS "enable building of unit tests" ON)

if (MSVC)
   if (CMAKE_SIZEOF_VOID_P EQUAL 8)
      add_compile_definitions(_AMD64_)
   else()
      add_compile_definitions(_X86_)
   endif()
endif()

add_subdirectory(external)

add_library(yams INTERFACE)
target_include_directories(yams INTERFACE
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:include>)
add_library(yams::yams ALIAS yams)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(YAMS_ENABLE_TESTS)
   FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2
      GIT_TAG v3.6.0
   )
   
   FetchContent_GetProperties(Catch2)
   if (NOT Catch2_POPULATED)
      FetchContent_MakeAvailable(Catch2)
      list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/contrib") 
   endif()

   include(CTest)
   include(Catch)

   enable_testing()
   add_subdirectory(tests)
endif()

# ##################################################################################################
# Configure version info.
# ##################################################################################################
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/modules/yams-config-version.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/yams-config-version.cmake @ONLY)

# ##################################################################################################
# Installation.
# ##################################################################################################
if(yams_ENABLE_INSTALL)
   include(CMakePackageConfigHelpers)
   include(GNUInstallDirs)

   message(STATUS "Installing yams ...")
   set(yams_INCLUDE_DIRS ${CMAKE_INSTALL_INCLUDEDIR})
   configure_package_config_file(modules/yams-config.cmake.in
      ${CMAKE_CURRENT_BINARY_DIR}/yams-config.cmake
      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yams
      PATH_VARS yams_INCLUDE_DIRS
   )
   write_basic_package_version_file(
      ${CMAKE_CURRENT_BINARY_DIR}/yams-config-version.cmake
      VERSION ${yams_VERSION}
      COMPATIBILITY SameMajorVersion
   )

   install(TARGETS yams
           EXPORT yams-targets
           LIBRARY
           PUBLIC_HEADER
              DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
              COMPONENT Headers
   )
   install(EXPORT yams-targets
      NAMESPACE fast::
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yams
   )
   install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/fast
           DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   )
   install(FILES ${CMAKE_CURRENT_BINARY_DIR}/yams-config.cmake
                 ${CMAKE_CURRENT_BINARY_DIR}/yams-config-version.cmake
                 DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/yams
   )
endif()