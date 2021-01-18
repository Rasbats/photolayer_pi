cmake_minimum_required(VERSION 3.1)

if (TARGET msvc::tiff)
    return ()
endif ()

if (NOT WIN32)
  return ()
endif ()

file(GLOB libtiff_dir "${CMAKE_CURRENT_SOURCE_DIR}/tiff-msvc-*")
add_library(TIFF STATIC IMPORTED)
set_property(
  TARGET TIFF
  PROPERTY IMPORTED_LOCATION ${libtiff_dir}/libtiff.lib
)
add_library(_TIFF INTERFACE)
target_include_directories(_TIFF INTERFACE ${libtiff_dir}/include)
target_link_libraries(_TIFF INTERFACE TIFF)
add_library(msvc::tiff ALIAS _TIFF)
