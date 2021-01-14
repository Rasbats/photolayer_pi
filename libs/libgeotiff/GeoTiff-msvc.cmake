#
# Build the pre-built libgeotiff msvc binary. Exports:
#
# msvc::geotiff         Interface library carrying headers and library

set(CMAKE_POSITION_INDEPENDENT_CODE "ON")

file(GLOB _geotiff_dir "${CMAKE_CURRENT_SOURCE_DIR}/libgeotiff-*-msvc")

add_library(_msvc_geotiff INTERFACE)

add_library(GEOTIFF_LIB STATIC IMPORTED)
set_property(
  TARGET GEOTIFF_LIB
  PROPERTY IMPORTED_LOCATION ${_geotiff_dir}/geotiff.lib
)
target_link_libraries(_msvc_geotiff INTERFACE GEOTIFF_LIB)
target_include_directories(_msvc_geotiff
  INTERFACE ${_geotiff_dir}/include ${_geotiff_dir}/libxtiff
)
add_library(msvc::geotiff ALIAS _msvc_geotiff)
