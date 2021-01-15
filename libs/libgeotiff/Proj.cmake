#
# Build the proj dependency.
#
# Exports:
#   EXT_PROJ           The external project
#   PROJ_LIBRARIES     The static library built
#   PROJ_INCLUDE_DIRS  Public header files
#
# Enviroment:
#   MAX_JOBS           Max number of concurrent jobs (make -j MAX_JOBS)
#

include(ExternalProject)
include(ProcessorCount)

include(${CMAKE_SOURCE_DIR}/build-deps/external_deps.cmake)


set(CMAKE_POSITION_INDEPENDENT_CODE "ON")

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

ExternalProject_Add(
  EXT_PROJ
  URL                 ${EXT_PROJ_URL}
  URL_HASH            ${EXT_PROJ_HASH}
  DOWNLOAD_DIR        ${PROJECT_SOURCE_DIR}/cache
  SOURCE_DIR          ${PROJECT_SOURCE_DIR}/proj-7.2.1
  BUILD_IN_SOURCE     1
  CONFIGURE_COMMAND autoreconf -fi &&
    ./configure CFLAGS=-fPIC --prefix=${_install_root} --enable-tiff=no
  BUILD_COMMAND       make -j${MAX_JOBS}
  INSTALL_COMMAND     make install
)
set(_proj_obj_filename
  ${CMAKE_STATIC_LIBRARY_PREFIX}proj${CMAKE_STATIC_LIBRARY_SUFFIX}
)
set(_proj_obj_path ${_install_root}/lib/${_proj_obj_filename})

add_library(_proj_implib STATIC IMPORTED)
set_property(
  TARGET _proj_implib
  PROPERTY IMPORTED_LOCATION ${_proj_obj_path}
)
add_dependencies(_proj_implib EXT_PROJ)

set(PROJ_LIBRARIES _proj_implib)
set(PROJ_INCLUDE_DIRS ${_install_root}/include)
