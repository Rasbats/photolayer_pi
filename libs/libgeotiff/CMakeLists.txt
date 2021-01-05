cmake_minimum_required(VERSION 3.1)

if (TARGET ocpn::geotiff)
    return ()
endif ()

if (NOT CMAKE_MODULE_PATH)
  set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)
endif ()

find_package(GeoTIFF)
if (GEOTIFF_FOUND)
  add_library(_GEOTIFF INTERFACE)
  target_link_libraries(_GEOTIFF INTERFACE ${GEOTIFF_LIBRARIES})
  target_include_directories(_GEOTIFF INTERFACE ${GEOTIFF_INCLUDE_DIR})
  add_library(ocpn::geotiff ALIAS _GEOTIFF)
  return()
endif ()

if (WIN32)
  file(GLOB geotiff_dir "${CMAKE_CURRENT_SOURCE_DIR}/libgeotiff-*")
  add_library(GEOTIFF STATIC IMPORTED)
  set_property(
    TARGET GEOTIFF
    PROPERTY IMPORTED_LOCATION ${geotiff_dir}/geotiff.lib
  )
  add_library(_GEOTIFF INTERFACE)
  target_link_libraries(_GEOTIFF INTERFACE GEOTIFF)
  target_include_directories(_GEOTIFF INTERFACE ${geotiff_dir}/include)
  target_include_directories(_GEOTIFF INTERFACE ${geotiff_dir}/libxtiff)
  add_library(ocpn::geotiff ALIAS _GEOTIFF)
else ()
  message(FATAL_ERROR "Cannot find required  library libgeotiff")
endif ()
