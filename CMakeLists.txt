##---------------------------------------------------------------------------
## Author:      Mike Rossiter with massive help from Sean D'Epagnier
## Copyright:   
## License:     GPLv3
##---------------------------------------------------------------------------
 
# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(PhotoLayer_pi)
SET(PACKAGE_NAME PhotoLayer_pi)
SET(VERBOSE_NAME PhotoLayer)
SET(TITLE_NAME PhotoLayer)
SET(CPACK_PACKAGE_CONTACT "Mike Rossiter")

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")

IF (MSVC)
    INCLUDE_DIRECTORIES(${PLUGIN_SOURCE_DIR}/include/msvc)
ENDIF (MSVC)

INCLUDE("cmake/PluginConfigure.cmake")

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



      
IF(UNIX)
 INCLUDE("cmake/FindTinyXML.cmake")
 FIND_PACKAGE(TinyXML QUIET)
ENDIF(UNIX)

IF(TINYXML_FOUND)
 message (STATUS "Building with system tinyxml")
 INCLUDE_DIRECTORIES(${TINYXML_INCLUDE_DIR})
 ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_PhotoLayer})
 TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${TINYXML_LIBRARIES})
ELSE(TINYXML_FOUND)
 message (STATUS "Building with embedded tinyxml")
 INCLUDE_DIRECTORIES(src/tinyxml/)
 SET(SRC_LTINYXML
            src/tinyxml/tinyxml.cpp
            src/tinyxml/tinyxmlparser.cpp
            src/tinyxml/tinyxmlerror.cpp
 )
 ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_PHOTOLAYER} ${SRC_LTINYXML})
ENDIF(TINYXML_FOUND)
ADD_DEFINITIONS(-DTIXML_USE_STL)



ADD_DEFINITIONS(-DTIXML_USE_STL)


INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")

