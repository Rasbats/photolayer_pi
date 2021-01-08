cmake_minimum_required(VERSION 3.1)

project(libgeotiff)

include(ExternalProject)
include(ProcessorCount)


if (TARGET ocpn::geotiff)
    return ()
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

option(USE_SYSTEM_GEOTIFF "Use system libgeotiff libraries" OFF)

add_library(_GEOTIFF INTERFACE)
if (WIN32)
  file(GLOB geotiff_dir "${CMAKE_CURRENT_SOURCE_DIR}/libgeotiff-*-msvc")
  add_library(GEOTIFF STATIC IMPORTED)
  set_property(
    TARGET GEOTIFF
    PROPERTY IMPORTED_LOCATION ${geotiff_dir}/geotiff.lib
  )
  target_link_libraries(_GEOTIFF INTERFACE GEOTIFF)
  target_include_directories(_GEOTIFF INTERFACE ${geotiff_dir}/include)
  target_include_directories(_GEOTIFF INTERFACE ${geotiff_dir}/libxtiff)
  add_library(ocpn::geotiff ALIAS _GEOTIFF)
elseif (USE_SYSTEM_GEOTIFF)
  find_package(GeoTIFF REQUIRED)
  if (GeoTIFF_FOUND)
    set(GEOTIFF_INCLUDE_DIRS ${GEOTIFF_INCLUDE_DIR})
    set(GEOTIFF_LIBRARIES ${GEOTIFF_LIBRARY} )
    target_link_libraries(_GEOTIFF INTERFACE ${GEOTIFF_LIBRARIES})
    target_include_directories(_GEOTIFF INTERFACE ${GEOTIFF_INCLUDE_DIRS})
  endif ()
endif ()

set(_install_root ${CMAKE_CURRENT_SOURCE_DIR}/root)
if (NOT GeoTIFF_FOUND AND NOT WIN32)
  ProcessorCount(NPROC)
  ExternalProject_Add(
    _proj
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/proj-7.2.1
    CONFIGURE_COMMAND autoreconf -fi && ./configure --prefix=${_install_root}
    BUILD_COMMAND make -j${NPROC}
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 1
  )
  string(CONCAT _proj_implib_path
    ${install_root}/usr/local/lib
    /${CMAKE_STATIC_LIBRARY_PREFIX}proj${CMAKE_STATIC_LIBRARY_SUFFIX}
  )  
  add_library(_proj_implib STATIC IMPORTED)
  set_property(
    TARGET _proj_implib
    PROPERTY IMPORTED_LOCATION ${_proj_implib_path}
  )
  add_dependencies(_proj_implib _proj)
  target_link_libraries(_GEOTIFF INTERFACE _proj_implib)
  target_include_directories(_GEOTIFF INTERFACE ${_install_root}/include)

  ExternalProject_Add(
    _libgeotiff
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/libgeotiff-1.6.0
    CONFIGURE_COMMAND autoreconf -fi &&
      ./configure --with-proj=${_install_root} --prefix=${_install_root}
    BUILD_COMMAND make -j${NPROC}
    INSTALL_COMMAND make install
    BUILD_IN_SOURCE 1
  )
  string(CONCAT _geotiff_implib_path
    ${install_root}/usr/local/lib
    /${CMAKE_STATIC_LIBRARY_PREFIX}geotiff${CMAKE_STATIC_LIBRARY_SUFFIX}
  )  
  add_library(_geotiff_implib STATIC IMPORTED)
  set_property(
    TARGET _geotiff_implib
    PROPERTY IMPORTED_LOCATION ${_geotiff_implib_path}
  )
  add_dependencies(_geotiff_implib _libgeotiff)
  target_link_libraries(_GEOTIFF INTERFACE _geotiff_implib)
  target_include_directories(_GEOTIFF INTERFACE ${_install_root}/include)

  add_dependencies(_libgeotiff _proj)

endif ()
add_library(ocpn::geotiff ALIAS _GEOTIFF)
