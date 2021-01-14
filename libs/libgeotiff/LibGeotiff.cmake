#
# Build the libgeotiff  dependency. 
#
# Exports:
#   EXT_GEOTIFF           The external project
#   GEOTIFF_LIBRARIES     The static library built
#   GEOTIFF_INCLUDE_DIRS  Public header files
#
# Depends
#   EXT_PROJ              The proj external project

include(ExternalProject)
include(ProcessorCount)

if (GEOTIFF_INSTALL_PREFIX)
  string(REPLACE "~" "$ENV{HOME}" _install_root ${GEOTIFF_INSTALL_PREFIX})
else ()
  message(FATAL_ERROR "Required variable GEOTIFF_INSTALL_PREFIX missing")
endif ()

ProcessorCount(NPROC)
if ("$ENV{MAX_JOBS}" STREQUAL "")
  set(MAX_JOBS ${NPROC})
else ()
  set(MAX_JOBS $ENV{MAX_JOBS})
endif ()


set(CMAKE_POSITION_INDEPENDENT_CODE "ON")

ExternalProject_Add(
  EXT_GEOTIFF
  URL http://download.osgeo.org/geotiff/libgeotiff/libgeotiff-1.6.0.tar.gz
  URL_HASH
    SHA256=9311017e5284cffb86f2c7b7a9df1fb5ebcdc61c30468fb2e6bca36e4272ebca
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/libgeotiff-1.6.0
  DOWNLOAD_DIR ${PROJECT_SOURCE_DIR}/cache
  CONFIGURE_COMMAND autoreconf -fi && ./configure CFLAGS="${CMAKE_C_FLAGS}"
    --with-proj=${_install_root}
    --prefix=${_install_root}
    --disable-shared
  BUILD_COMMAND make -j${MAX_JOBS}
  INSTALL_COMMAND make install
  BUILD_IN_SOURCE 1
  DEPENDS EXT_PROJ
)

set(_obj_filename 
  ${CMAKE_STATIC_LIBRARY_PREFIX}geotiff${CMAKE_STATIC_LIBRARY_SUFFIX}
)
set(_geotiff_implib_path ${_install_root}/lib/${_obj_filename})

add_library(_geotiff_implib STATIC IMPORTED)
set_property(
  TARGET _geotiff_implib
  PROPERTY IMPORTED_LOCATION ${_geotiff_implib_path}
)
add_dependencies(_geotiff_implib EXT_GEOTIFF)

set(GEOTIFF_LIBRARIES _geotiff_implib)
set(GEOTIFF_INCLUDE_DIRS ${_install_root}/include)
