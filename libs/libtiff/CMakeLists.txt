#
#   Locate, link and bundle the libtiff library.
#
#   Exports:
#       ocnp::tiff interface library with headers, libraries etc.
#
#   Bundles:
#       libtiff.so on Debian.

cmake_minimum_required(VERSION 3.1)

if (TARGET ocpn::tiff)
    return ()
endif ()

add_library(_TIFF INTERFACE)
add_library(ocpn::tiff ALIAS _TIFF)

if (WIN32)
  file(GLOB libtiff_dir "${CMAKE_CURRENT_SOURCE_DIR}/tiff-msvc-*")
  add_library(_TIFF_MSCV STATIC IMPORTED)
  set_property(
    TARGET _TIFF_MSCV
    PROPERTY IMPORTED_LOCATION ${libtiff_dir}/libtiff.lib
  )
  target_include_directories(_TIFF INTERFACE ${libtiff_dir}/include)
  target_link_libraries(_TIFF INTERFACE _TIFF_MSCV)
else ()
  find_package(TIFF REQUIRED)
  target_link_libraries(_TIFF INTERFACE ${TIFF_LIBRARIES})
  target_include_directories(_TIFF INTERFACE ${TIFF_INCLUDE_DIRS})
endif ()

if (UNIX AND NOT "${BUILD_TYPE}" STREQUAL "flatpak" AND NOT APPLE)
  # Debian: bundle required runtime libs in tarball.
  file(GLOB _tiff_libs /usr/lib/*/libtiff.so.* /usr/lib*/libtiff.so.*)
  install(CODE
    "execute_process(COMMAND cmake -E copy ${_tiff_libs} app/files/lib)"
   )
endif ()
