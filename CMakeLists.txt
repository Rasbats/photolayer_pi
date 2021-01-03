# ~~~
# Author:      Rasbats aka. Mike Rossiter
# Copyright:
# License:     wxWidgets License
# ~~~

# -------- Cmake setup ---------
#
cmake_minimum_required(VERSION 3.5.1)

cmake_policy(SET CMP0042 NEW)

if (POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif ()

# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")

# Locations where cmake looks for cmake modules.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build ${CMAKE_SOURCE_DIR}/cmake)

if (WIN32)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/buildwin)
endif ()
message(STATUS "Cmake version: ${CMAKE_VERSION}.")


# -------- Build setup (mostly in PluginCompiler) --------
#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
message(STATUS "Building: ${BUILD_TYPE}")


# -------- Options ----------

set(OCPN_TEST_REPO
    "opencpn/photolayer-alpha"
    CACHE STRING "Default repository for untagged builds"
)
set(OCPN_BETA_REPO
    "opencpn/photolayer-beta"
    CACHE STRING 
    "Default repository for tagged builds matching 'beta'"
)
set(OCPN_RELEASE_REPO
    "opencpn/photolayer-prod"
    CACHE STRING 
    "Default repository for tagged builds not matching 'beta'"
)
option(PHOTOLAYER_USE_SVG "Use SVG graphics" ON)


# -------  Plugin setup --------
#
project(photolayer_pi VERSION 3.2.0)

set(DISPLAY_NAME Photolayer)    # Dialogs, installer artifacts, ...
set(PLUGIN_API_NAME photolayer) # As of GetCommonName() in plugin API
set(CPACK_PACKAGE_CONTACT "Mike Rossiter")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Import GeoTiff satellite photo overlay files")
set(CPACK_PACKAGE_DESCRIPTION [=[
Plugin for OpenCPN using GeoTiff satellite photo overlay files.
The transparency can be adjusted, allowing easy comparison of features. OpenGL gives the best results.
GeoTiff files can be downloaded from 'VentureFarther.com' or converted from kaps using the MAP2CMAPCNET program from John Thorn (Mercator lat/lon). 
]=])

set(PKG_RELEASE "1")
set(PKG_AUTHOR "Mike Rossiter")

if(WIN32)
	set(geotiff ${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2)
	set(geotiff "${PARENT}.lib")
	set(libtiff ${CMAKE_SOURCE_DIR}/src/tiff-4.0.10)
	set(libtiff "${PARENT}.lib")

	link_directories(${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2)
	link_directories(${CMAKE_SOURCE_DIR}/src/tiff-4.0.10)

	include_directories(${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2/include)
	include_directories(${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2/libxtiff)
	include_directories(${CMAKE_SOURCE_DIR}/src/tiff-4.0.10/include)

endif ()

set(SRC
            src/PhotoLayer_pi.cpp
	        src/PhotoLayer_pi.h
            src/PhotoLayer.cpp
	        src/PhotoLayer.h
            src/PhotoLayerUI.cpp
            src/PhotoLayerUI.h
            src/PhotoLayerImage.cpp
            src/PhotoLayerImage.h
            src/AboutDialog.cpp
            src/AboutDialog.h
            src/icons.cpp
            src/icons.h
            src/wximgkap.cpp
            src/wximgkap.h
)

include("cmake/FindGeoTIFF.cmake") 
find_package(GeoTIFF QUIET)



# -------- Setup completed, build the plugin --------
#
add_library(${CMAKE_PROJECT_NAME} SHARED ${SRC})
include_directories(${CMAKE_BINARY_DIR}/include)

add_subdirectory("libs/api-16")
target_link_libraries(${CMAKE_PROJECT_NAME} ocpn::api)

set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(PACKAGE_NAME ${CMAKE_PROJECT_NAME})

# Create the minimim ocpn version id string like ov50
string(REGEX REPLACE "\([0-9]\)\.\([0-9]\).*" "ov\\1\\2"
       OCPN_MIN_VERSION ${API_OCPN_MIN_VERSION})

if (PHOTOLAYER_USE_SVG)
  add_definitions(-DPHOTOLAYER_USE_SVG)
endif ()

if(WIN32)
find_library(geotff ${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2)
find_library(libtiff ${CMAKE_SOURCE_DIR}/src/tiff-4.0.10)

target_link_libraries(${PACKAGE_NAME} geotiff libtiff)
endif ()

# Set up targets. Targets sets up a recursive call with BUILD_TYPE set to
# 'flatpak', 'pkg' or 'tarball'. The initial call without BUILD_TYPE ends
# here.
#
include(Targets)
create_targets(
  ${PROJECT_SOURCE_DIR}/flatpak/org.opencpn.OpenCPN.Plugin.photolayer.yaml
)
if ("${BUILD_TYPE}" STREQUAL "")
  return ()
endif ()

if (NOT ${BUILD_TYPE} STREQUAL "flatpak")
  # Build package as required (flatpak already dealt with).
  #
  include(PluginCompiler)
  include(PluginLibs)
  include(PluginLocalization)
  include(PluginInstall)
  include(PluginPackage)

  add_subdirectory("libs/jsoncpp")
  target_link_libraries(${PACKAGE_NAME} ocpn::jsoncpp)
  
  add_subdirectory("libs/tinyxml")
  target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)
  
  add_subdirectory("libs/wxJSON")
  target_link_libraries(${PACKAGE_NAME} ocpn::wxjson)
  
  if(GEOTIFF_FOUND)
  message (STATUS "Building with system libGeoTIFF")
  include_directories(${GEOTIFF_INCLUDE_DIR})
  target_link_libraries(${PACKAGE_NAME} ${GEOTIFF_LIBRARY})
  else()
	message (STATUS "libGeoTIFF not found, either disable the support for it or fail ro use an embedded version of it")
  endif ()
endif ()

configure_file(
  # The cloudsmith upload script
  ${CMAKE_SOURCE_DIR}/ci/upload.sh.in ${CMAKE_BINARY_DIR}/upload.sh
  @ONLY
)
configure_file(
  # The cloudsmith upload script, windows bat file.
  ${CMAKE_SOURCE_DIR}/ci/upload.bat.in ${CMAKE_BINARY_DIR}/upload.bat
  @ONLY
)


set(checksum "@checksum@")
configure_file(
  # The XML metadata file
  ${CMAKE_SOURCE_DIR}/photolayer-plugin.xml.in
  ${CMAKE_BINARY_DIR}/${pkg_displayname}.xml.in
  @ONLY
)
