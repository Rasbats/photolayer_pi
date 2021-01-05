

#

# - Find GEOTIFF library
# Find the native GEOTIFF includes and library
#
#   GEOTIFF_FOUND        - True if GEOTIFF found.
#   GEOTIFF_INCLUDE_DIRS - where to find geotiff.h
#   GEOTIFF_LIBRARIES    - List of libraries to link when using GEOTIFF.
#

find_path(GEOTIFF_INCLUDE_DIR geotiff.h PATH_SUFFIXES geotiff libgeotiff)

set(GEOTIFF_NAMES ${GEOTIFF_NAMES} geotiff_i geotiff libgeotiff_i libgeotiff)
find_library(GEOTIFF_LIBRARY NAMES ${GEOTIFF_NAMES})

mark_as_advanced(GEOTIFF_INCLUDE_DIR GEOTIFF_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  GeoTIFF DEFAULT_MSG GEOTIFF_LIBRARY GEOTIFF_INCLUDE_DIR
)

if (GeoTIFF_FOUND)
  set(GEOTIFF_INCLUDE_DIRS ${GEOTIFF_INCLUDE_DIR})
  set(GEOTIFF_LIBRARIES ${GEOTIFF_LIBRARY} )
  add_library(_GEOTIFF INTERFACE)
  target_link_libraries(_GEOTIFF INTERFACE ${GEOTIFF_LIBRARIES})
  target_include_directories(_GEOTIFF INTERFACE ${GEOTIFF_INCLUDE_DIRS})
  add_library(ocpn::geotiff ALIAS _GEOTIFF)
endif ()
