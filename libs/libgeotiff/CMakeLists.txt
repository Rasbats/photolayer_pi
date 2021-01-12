cmake_minimum_required(VERSION 3.1)

project(libgeotiff)

if (TARGET ocpn::geotiff)
    return ()
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

set(CMAKE_POSITION_INDEPENDENT_CODE "ON")

option(USE_SYSTEM_GEOTIFF "Use system libgeotiff libraries" OFF)

set(GEOTIFF_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/geotiff-root CACHE STRING
  "libgeotiff build dependencies installation location"
)

add_library(_GEOTIFF INTERFACE)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(_GEOTIFF INTERFACE "-Wl,--whole-archive")
endif ()
if (WIN32)
  include(${CMAKE_CURRENT_LIST_DIR}/GeoTiff-msvc.cmake)
  target_link_libraries(_GEOTIFF INTERFACE ${GEOTIFF_LIBRARIES})
  target_include_directories(_GEOTIFF INTERFACE ${GEOTIFF_INCLUDE_DIRS})
elseif (USE_SYSTEM_GEOTIFF)
  find_package(GeoTIFF)
  if (GeoTIFF_FOUND)
    target_link_libraries(_GEOTIFF INTERFACE ${GEOTIFF_LIBRARIES})
    target_include_directories(_GEOTIFF INTERFACE ${GEOTIFF_INCLUDE_DIRS})
    if (UNIX AND NOT APPLE)
      # we need to use the Debian hardening flags to be able to link
      target_compile_options(_GEOTIFF INTERFACE -fPIE -pie -D_FORTIFY_SOURCE=2)
    endif ()
  endif ()
endif ()

if (NOT GeoTIFF_FOUND AND NOT WIN32)
  include(${CMAKE_CURRENT_LIST_DIR}/Proj.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/LibGeotiff.cmake)

  target_link_libraries(_GEOTIFF INTERFACE ${GEOTIFF_LIBRARIES})
  target_include_directories(_GEOTIFF INTERFACE ${GEOTIFF_INCLUDE_DIRS})
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(_GEOTIFF INTERFACE "-Wl,--no-whole-archive")
endif ()

add_library(ocpn::geotiff ALIAS _GEOTIFF)
