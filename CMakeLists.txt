##---------------------------------------------------------------------------
## Author:      Mike Rossiter with massive help from Sean D'Epagnier
## Copyright:
## License:     GPLv2
##---------------------------------------------------------------------------

##
## ----- Modify section below to include all the details for your plugin ----- ##
##

set(CMLOC "CMakeLists: ")

# define minimum cmake version
cmake_minimum_required(VERSION 3.1.1)
if(COMMAND cmake_policy)
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif(POLICY CMP0043)
    cmake_policy(SET CMP0048 NEW)
    if(POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
    endif(POLICY CMP0077)
endif(COMMAND cmake_policy)

# define plugin name, owner and versions
set(VERBOSE_NAME "photolayer_pi")
set(COMMON_NAME "photolayer")
set(TITLE_NAME "photolayer")
set(PACKAGE_CONTACT "Mike Rossiter")
set(PACKAGE "photolayer")

set(VERSION_MAJOR "3")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION_TWEAK "90")
set(VERSION_DATE "10/07/2020")
set(OCPN_MIN_VERSION "ov50")
set(OCPN_API_VERSION_MAJOR "1")
set(OCPN_API_VERSION_MINOR "16")
set(TP_COMMENT "  * Release for O5 using CI")

set(PARENT "opencpn")

# The next line allows for setting up your own local webserver with git on it for testing purposes, the default is github.com
set(GIT_REPOSITORY_SERVER "github.com")

# Specifies cloudsmith repository sufficies
set(PROD "prod") # Standard Repos
set(BETA "beta") # Standard Repos
set(ALPHA "alpha") # Standard Repos

set(CLOUDSMITH_USER "opencpn")
# set this if your cloudsmith repository name does not match your git repository name set(CLOUDSMITH_BASE_REPOSITORY "testplugin_pi")
set(CLOUDSMITH_BASE_REPOSITORY "photolayer")

set(XML_INFO_URL "https://opencpn.org/wiki/dokuwiki/doku.php?id=opencpn:developer_manual:plugins:beta_plugins:photolayer")
set(XML_SUMMARY "Plugin to overlay satellite geotiff photo overlay files with tranparency on the navigation chart")
set(XML_DESCRIPTION "PhotoLayer reads satellite geotiff photo overlay files with tranparency")

##
## ----- Modify section above to include all the details for your plugin ----- ##
##

##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

OPTION(PHOTOLAYER_USE_SVG "Use SVG graphics" ON)

set(CMAKE_CXX_STANDARD 11)

# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")
# Don't use local version of GLU library
set(USE_LOCAL_GLU FALSE)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE
        "Release"
        CACHE STRING "Choose the type of build" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
message(STATUS "${CMLOC}Build type: ${CMAKE_BUILD_TYPE}")

##
## ----- The statements below are used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
##

project(${PACKAGE})

include("cmake/PluginSetup.cmake")

##
## ----- The above is used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
##

##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

set(CMAKE_VERBOSE_MAKEFILE ON)

option(Plugin_CXX11 "Use c++11" OFF)

##
## ----- Modify section above if there are special requirements for the plugin ----- ##
##

##
## ----- do not change next section - needed to configure build process ----- ##
##

include("cmake/PluginConfigure.cmake")

##
## ----- do not change section above - needed to configure build process ----- ##
##

#
# ----- Change below to match project requirements for source, headers, etc. ----- ##
#

add_definitions(-DUSE_S57)


IF(WIN32)
	SET(geotiff ${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2)
	SET(geotiff "${PARENT}.lib")
	SET(libtiff ${CMAKE_SOURCE_DIR}/src/tiff-4.0.10)
	SET(libtiff "${PARENT}.lib")

	LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2)
	LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/tiff-4.0.10)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2/include)
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2/libxtiff)
INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/src/tiff-4.0.10/include)

SET(SRC_PHOTOLAYER
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
INCLUDE("cmake/FindGeoTIFF.cmake")     
INCLUDE("cmake/FindTIFF.cmake")  
INCLUDE("cmake/FindTinyXML.cmake")
     
 
 FIND_PACKAGE(GeoTIFF QUIET)
 
 FIND_PACKAGE(TIFF QUIET)
 
 FIND_PACKAGE(TinyXML QUIET)
 


IF(TINYXML_FOUND)
    message (STATUS "${CMLOC}Building with system tinyxml")
    INCLUDE_DIRECTORIES(${TINYXML_INCLUDE_DIR})
    ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_PHOTOLAYER})
    TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${TINYXML_LIBRARIES})
ELSE(TINYXML_FOUND)
    message (STATUS "${CMLOC}Building with embedded tinyxml")
 INCLUDE_DIRECTORIES(src/tinyxml/)
 SET(SRC_LTINYXML
            src/tinyxml/tinyxml.cpp
            src/tinyxml/tinyxmlparser.cpp
            src/tinyxml/tinyxmlerror.cpp
 )
 ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_PHOTOLAYER} ${SRC_LTINYXML})
ENDIF(TINYXML_FOUND)

ADD_DEFINITIONS(-DTIXML_USE_STL)

IF(GEOTIFF_FOUND)
    message (STATUS "${CMLOC}Building with system libGeoTIFF")
    INCLUDE_DIRECTORIES(BEFORE ${GEOTIFF_INCLUDE_DIR})
    TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${GEOTIFF_LIBRARY})
ELSE(GEOTIFF_FOUND)
    message (STATUS "${CMLOC}libGeoTIFF not found, either disable the support for it or fail ro use an embedded version of it")
ENDIF(GEOTIFF_FOUND)

IF(TIFF_FOUND)
    message (STATUS "${CMLOC}Building with system libTIFF")
    INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${LIBTIFF_LIBRARY})
ELSE(TIFF_FOUND)
    message (STATUS "${CMLOC}libTIFF not found, either disable the support for it or fail ro use an embedded version of it")
ENDIF(TIFF_FOUND)


TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${EXTRA_LIBS})

set(OCPNSRC ocpnsrc/TexFont.cpp)

set(OCPNHDRS ocpninclude/ocpn_plugin.h ocpninclude/pluginmanager.h ocpninclude/TexFont.h)

set(EXTSRC)

set(EXTINCLUDE extinclude/nlohmann/json.hpp extinclude/ODAPI.h)

set(LIBSSRC libs/wxJSON/src/jsonreader.cpp libs/wxJSON/src/jsonval.cpp libs/wxJSON/src/jsonwriter.cpp)

set(LIBHDRS
    libs/GL/include/gl.h
    libs/GL/include/gl_private.h
    libs/GL/include/glext.h
    libs/GL/include/glu.h
    libs/wx/include/json_defs.h
    libs/wx/include/jsonreader.h
    libs/wx/include/jsonval.h
    libs/wx/include/jsonwriter.h)

add_definitions(-DPHOTOLAYER_USE_SVG)

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/ocpninclude)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/extinclude)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs/wx/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/ocpninclude/wxcurl)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/ocpninclude/wxcurl/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/ocpnsrc/lz4)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/ocpnsrc/bitmaps)


IF(WIN32)
FIND_LIBRARY(geotff ${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2)
FIND_LIBRARY(libtiff ${CMAKE_SOURCE_DIR}/src/tiff-4.0.10)

TARGET_LINK_LIBRARIES(${PACKAGE_NAME} geotiff libtiff)
ENDIF(WIN32)


##
## ----- Change above to match project requirements ----- ##
##

##
## ----- do not change next section - needed to configure build process ----- ##
##



include("cmake/PluginInstall.cmake")
include("cmake/PluginLocalization.cmake")
include("cmake/PluginPackage.cmake")

##
## ----- do not change section above - needed to configure build process ----- ##
##

