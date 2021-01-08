cmake_minimum_required(VERSION 3.1)

if (TARGET ocpn::geotiff)
    return ()
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

add_library(_GEOTIFF INTERFACE)
if (WIN32)
  file(GLOB geotiff_dir "${CMAKE_CURRENT_SOURCE_DIR}/libgeotiff-*")
  add_library(GEOTIFF STATIC IMPORTED)
  set_property(
    TARGET GEOTIFF
    PROPERTY IMPORTED_LOCATION ${geotiff_dir}/geotiff.lib
  )
  target_link_libraries(_GEOTIFF INTERFACE GEOTIFF)
  target_include_directories(_GEOTIFF INTERFACE ${geotiff_dir}/include)
  target_include_directories(_GEOTIFF INTERFACE ${geotiff_dir}/libxtiff)
  add_library(ocpn::geotiff ALIAS _GEOTIFF)
else ()
  find_package(GeoTIFF REQUIRED)
  set(GEOTIFF_INCLUDE_DIRS ${GEOTIFF_INCLUDE_DIR})
  set(GEOTIFF_LIBRARIES ${GEOTIFF_LIBRARY} )
  target_link_libraries(_GEOTIFF INTERFACE ${GEOTIFF_LIBRARIES})
  target_include_directories(_GEOTIFF INTERFACE ${GEOTIFF_INCLUDE_DIRS})
endif ()
add_library(ocpn::geotiff ALIAS _GEOTIFF)
