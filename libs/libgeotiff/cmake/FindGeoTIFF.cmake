

#

# - Find GEOTIFF library
# Find the native GEOTIFF includes and library
#
#   GEOTIFF_FOUND        - True if GEOTIFF found.
#   GEOTIFF_INCLUDE_DIRS - where to find geotiff.h
#   GEOTIFF_LIBRARIES    - List of libraries to link when using GEOTIFF.
#


find_path(
  GEOTIFF_INCLUDE_DIR geotiff.h
  PATH_SUFFIXES geotiff libgeotiff
  HINTS ${GEOTIFF_INSTALL_PREFIX}/include $ENV{HOME}/.local/include
)

find_library(GEOTIFF_LIBRARY NAMES geotiff HINTS $ENV{HOME}/.local/lib)

set(GEOTIFF_LIBRARIES ${GEOTIFF_LIBRARY})
set(GEOTIFF_INCLUDE_DIRS ${GEOTIFF_INCLUDE_DIR})

mark_as_advanced(GEOTIFF_INCLUDE_DIR GEOTIFF_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  GeoTIFF DEFAULT_MSG GEOTIFF_LIBRARY GEOTIFF_INCLUDE_DIR
)
