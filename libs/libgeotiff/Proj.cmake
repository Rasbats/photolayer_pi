#
# Build the proj dependency. Exports:
#
# EXT_PROJ           The external project
# PROJ_LIBRARIES     The static library built
# PROJ_INCLUDE_DIRS  Public header files

include(ExternalProject)

include(ProcessorCount)
ProcessorCount(NPROC)

set(CMAKE_POSITION_INDEPENDENT_CODE "ON")

if (GEOTIFF_INSTALL_ROOT)
  set(_install_root ${GEOTIFF_INSTALL_ROOT})
else ()
  message(FATAL_ERROR "Required variable GEOTIFF_INSTALL_ROOT missing")
endif ()

ExternalProject_Add(
  EXT_PROJ
  SOURCE_DIR      ${PROJECT_SOURCE_DIR}/proj-7.2.1
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND 
    autoreconf -fi && ./configure CFLAGS=-fPIC --prefix=${_install_root}
  BUILD_COMMAND   make -j${NPROC}
  INSTALL_COMMAND make install
)
set(_obj_filename
  ${CMAKE_STATIC_LIBRARY_PREFIX}proj${CMAKE_STATIC_LIBRARY_SUFFIX}
)
set(_proj_obj_path ${_install_root}/lib/${_obj_filename})

add_library(_proj_implib STATIC IMPORTED)
set_property(
  TARGET _proj_implib
  PROPERTY IMPORTED_LOCATION ${_proj_obj_path}
)
add_dependencies(_proj_implib EXT_PROJ)

set(PROJ_LIBRARIES _proj_implib)
set(PROJ_INCLUDE_DIRS ${_install_root}/include)
