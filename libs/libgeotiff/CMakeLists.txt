cmake_minimum_required(VERSION 3.1)

project(libgeotiff)

if (TARGET ocpn::geotiff)
    return ()
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(CMAKE_POSITION_INDEPENDENT_CODE "ON")

if (UNIX AND NOT "${BUILD_TYPE}" STREQUAL "flatpak" AND NOT APPLE)
  # Debian
  option(USE_SYSTEM_GEOTIFF "Use system libgeotiff libraries [ON]" ON)
else ()
  option(USE_SYSTEM_GEOTIFF "Use system libgeotiff libraries [OFF]" OFF)
endif ()

set(GEOTIFF_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/geotiff-root CACHE STRING
  "libgeotiff build dependencies installation location"
)

add_library(_GEOTIFF INTERFACE)
add_library(ocpn::geotiff ALIAS _GEOTIFF)

if (WIN32)
  include(${CMAKE_CURRENT_LIST_DIR}/GeoTiff-msvc.cmake)
  target_link_libraries(_GEOTIFF INTERFACE  msvc::geotiff)
  return ()
endif ()

if (USE_SYSTEM_GEOTIFF)
  message(STATUS "Using system libgeotiff")
  find_package(GeoTIFF REQUIRED)
endif ()

if (NOT GeoTIFF_FOUND)
  include(${CMAKE_CURRENT_LIST_DIR}/Proj.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/LibGeotiff.cmake)
endif ()

target_link_libraries(_GEOTIFF
  INTERFACE ${GEOTIFF_LIBRARIES} ${PROJ_LIBRARIES}
)
target_include_directories(_GEOTIFF
  INTERFACE ${PROJ_INCLUDE_DIRS} ${GEOTIFF_INCLUDE_DIRS}
)

if (USE_SYSTEM_GEOTIFF)
  # Bundle required runtime libs in tarball, typically on Debian.
  message(STATUS "Bundling geotiff libraries: ${_geotiff_libs}")
  file(GLOB _geotiff_libs /usr/lib/*/libgeotiff.so.* /usr/lib/*/libproj.so.*)
  install(CODE
    "execute_process(COMMAND cmake -E copy ${_geotiff_libs} app/files/lib)"
  )
endif ()
