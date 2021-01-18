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

if (WIN32 OR APPLE)
  # libgeotiff is statically linked
  return ()
endif ()

if ("${BUILD_TYPE}" STREQUAL "flatpak")
  # libtiff is part of flatpak runtime
  return ()
endif ()

# Neither Windows, Macos nor Flatpak i. e., Linux/Debian.
find_package(TIFF REQUIRED)

target_link_libraries(_TIFF INTERFACE ${TIFF_LIBRARIES})
target_include_directories(_TIFF INTERFACE ${TIFF_INCLUDE_DIRS})

# Bundle required runtime libs in tarball
file(GLOB _tiff_libs /usr/lib/*/libtiff.so.* /usr/lib*/libtiff.so.*)
install(CODE
  "execute_process(COMMAND cmake -E copy ${_tiff_libs} app/files/lib)"
)
