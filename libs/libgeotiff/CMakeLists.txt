cmake_minimum_required(VERSION 3.13)

if (TARGET ocpn::geotiff)
    return ()
endif ()


set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


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
  add_library(_GeoTIFF INTERFACE)
  target_link_libraries(_GeoTIFF INTERFACE ${GEOTIFF_LIBRARIES})
  target_include_directories(_GeoTIFF INTERFACE ${GEOTIFF_INCLUDE_DIRS})
  add_library(ocpn::geotiff ALIAS _GeoTIFF)
endif ()
