#
# GeoTIFF support
#

find_package(GeoTIFF)
set_package_properties(GeoTIFF PROPERTIES TYPE)
if (GEOTIFF_FOUND)
    message("GeoTIFF found")
    include_directories("${GEOTIFF_INCLUDE_DIR}")
    set(PHOTOLAYER_HAVE_LIBGEOTIFF 1)
    mark_as_advanced(CLEAR TIFF_INCLUDE_DIR)
    mark_as_advanced(CLEAR TIFF_LIBRARY)
    if (WIN32)
        set(TIFF_NAMES libtiff_i)
    endif(WIN32)
else()
    message(FATAL_ERROR "GeoTIFF support is required")
endif()
