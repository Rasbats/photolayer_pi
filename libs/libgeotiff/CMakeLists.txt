cmake_minimum_required(VERSION 3.13)

if (TARGET ocpn::geotiff)
    return ()
endif ()

if (NOT CMAKE_MODULE_PATH)
  set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)
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
  find_package(GeoTIFF REQUIRED)
endif ()
