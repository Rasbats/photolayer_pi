#
# Build the pre-built libgeotiff msvc binary. Exports:
#
# GEOTIFF_LIBRARIES     The static library
# GEOTIFF_INCLUDE_DIRS  Public header files

set(CMAKE_POSITION_INDEPENDENT_CODE "ON")

file(GLOB _geotiff_dir "${CMAKE_CURRENT_SOURCE_DIR}/libgeotiff-*-msvc")
add_library(GEOTIFF_LIBRARIES STATIC IMPORTED)
set_property(
  TARGET GEOTIFF_LIBRARIES
  PROPERTY IMPORTED_LOCATION ${_geotiff_dir}/geotiff.lib
)
message(STATUS "GEOTIFF LIB LOCATION: ${_geotiff_dir}/geotiff.lib")
set(GEOTIFF_INCLUDE_DIRS ${_geotiff_dir}/include ${_geotiff_dir}/libxtiff)
