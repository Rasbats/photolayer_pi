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
add_library(ocpn::geotiff ALIAS _GEOTIFF)

if (WIN32)
  include(${CMAKE_CURRENT_LIST_DIR}/GeoTiff-msvc.cmake)
  target_link_libraries(_GEOTIFF INTERFACE  msvc::geotiff)
  return ()
endif ()

if (USE_SYSTEM_GEOTIFF)
  find_package(GeoTIFF)
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
