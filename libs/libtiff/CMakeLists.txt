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
  add_library(_TIFF_MSVC STATIC IMPORTED)
  set_property(
    TARGET _TIFF_MSVC
    PROPERTY IMPORTED_LOCATION ${libtiff_dir}/libtiff.lib
  )
  target_include_directories(_TIFF INTERFACE ${libtiff_dir}/include)
  target_link_libraries(_TIFF INTERFACE _TIFF_MSVC)
elseif (APPLE)
  find_package(TIFF REQUIRED)
  find_library(_STATIC_TIFF_LIB NAMES libtiff.a REQUIRED)
  find_library(_STATIC_JPEG_LIB NAMES libjpeg.a REQUIRED)
  target_link_libraries(
    _TIFF INTERFACE ${_STATIC_TIFF_LIB} ${_STATIC_JPEG_LIB}
  )
  target_include_directories(_TIFF INTERFACE ${TIFF_INCLUDE_DIRS})
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
