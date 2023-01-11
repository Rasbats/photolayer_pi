# ~~~
# Summary:      Local, non-generic plugin setup
# Copyright (c) 2020-2021 Mike Rossiter
# License:      GPLv3+
# ~~~

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.


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

#
#
# -------  Plugin setup --------
#
set(PKG_NAME photolayer_pi)
set(PKG_VERSION  4.0.0)
set(PKG_PRERELEASE "")  # Empty, or a tag like 'beta'

set(DISPLAY_NAME photolayer)    # Dialogs, installer artifacts, ...
set(PLUGIN_API_NAME photolayer) # As of GetCommonName() in plugin API
set(PKG_SUMMARY "Display geotiff files with transparency")
set(PKG_DESCRIPTION [=[
Display geotiff files with adjustable transparency, as an overlay.
]=])

set(PKG_AUTHOR "Mike Rossiter")
set(PKG_IS_OPEN_SOURCE "yes")
set(PKG_HOMEPAGE https://github.com/Rasbats/photolayer_pi)
set(PKG_INFO_URL https://opencpn.org/OpenCPN/plugins/photolayer.html)

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

set(PKG_API_LIB api-16)  #  A directory in libs/ e. g., api-17 or api-16

macro(late_init)
  # Perform initialization after the PACKAGE_NAME library, compilers
  # and ocpn::api is available.
  if (PHOTOLAYER_USE_SVG)
    target_compile_definitions(${PACKAGE_NAME} PUBLIC PHOTOLAYER_USE_SVG)
  endif ()

  add_definitions(-DocpnUSE_GL)

  if (QT_ANDROID)
    add_definitions(-DUSE_ANDROID_GLES2)
  endif ()
  
endmacro ()

macro(add_plugin_libraries)
   add_subdirectory("libs/jsoncpp")
  target_link_libraries(${PACKAGE_NAME} ocpn::jsoncpp)

  add_subdirectory("libs/tinyxml")
  target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)

  add_subdirectory("libs/wxJSON")
  target_link_libraries(${PACKAGE_NAME} ocpn::wxjson)

  add_subdirectory("libs/libtiff")
  target_link_libraries(${PACKAGE_NAME} ocpn::tiff)

  add_subdirectory("libs/libgeotiff")
  target_link_libraries(${PACKAGE_NAME} ocpn::geotiff)

endmacro ()
