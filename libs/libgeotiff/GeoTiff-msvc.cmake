#
# Build the pre-built libgeotiff msvc binary. Exports:
#
# GEOTIFF_MSVC_LIBRARIES     The static library
# GEOTIFF_MSVC_INCLUDE_DIRS  Public header files

set(CMAKE_POSITION_INDEPENDENT_CODE "ON")

file(GLOB _geotiff_dir "${CMAKE_CURRENT_SOURCE_DIR}/libgeotiff-*-msvc")
add_library(GEOTIFF_LIBRARIES STATIC IMPORTED)
set_property(
  TARGET GEOTIFF_MSVC_LIBRARIES
  PROPERTY IMPORTED_LOCATION ${_geotiff_dir}/geotiff.lib
)
set(GEOTIFF_INCLUDE_DIRS ${geotiff_dir}/include)
