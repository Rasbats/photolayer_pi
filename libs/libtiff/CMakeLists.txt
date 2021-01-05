cmake_minimum_required(VERSION 3.1)

if (TARGET ocpn::tiff)
    return ()
endif ()

if (NOT CMAKE_MODULE_PATH)
  set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)
endif ()

if (WIN32)
  file(GLOB libtiff_dir "${CMAKE_CURRENT_SOURCE_DIR}/tiff-*")
  add_library(TIFF STATIC IMPORTED)
  set_property(
    TARGET TIFF
    PROPERTY IMPORTED_LOCATION ${libtiff_dir}/libtiff.lib
  )
  add_library(_TIFF INTERFACE)
  target_include_directories(_TIFF INTERFACE ${libtiff_dir}/include)
  target_link_libraries(_TIFF INTERFACE TIFF)
  add_library(ocpn::tiff ALIAS _TIFF)
else ()
  find_package(TIFF REQUIRED)
  add_library(_TIFF INTERFACE)
  target_link_libraries(_TIFF INTERFACE ${TIFF_LIBRARIES})
  target_include_directories(_TIFF INTERFACE ${TIFF_INCLUDE_DIRS})
  add_library(ocpn::tiff ALIAS _TIFF)
endif ()
