##---------------------------------------------------------------------------
## Author:      Mike Rossiter with massive help from Sean D'Epagnier
## Copyright:
## License:     GPLv3
##---------------------------------------------------------------------------

# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(photolayer_pi)
SET(PACKAGE_NAME photolayer_pi)
SET(VERBOSE_NAME photolayer)
SET(TITLE_NAME photolayer)
SET(CPACK_PACKAGE_CONTACT "Mike Rossiter")

SET(VERSION_MAJOR "2")
SET(VERSION_MINOR "1.2")

SET(NAME_SUFFIX "ov50")

set (CMAKE_CXX_STANDARD 11)

OPTION(PHOTOLAYER_USE_SVG "Use SVG graphics" ON)

IF(PHOTOLAYER_USE_SVG)
  ADD_DEFINITIONS(-DPHOTOLAYER_USE_SVG)
ENDIF(PHOTOLAYER_USE_SVG)

INCLUDE("cmake/PluginConfigure.cmake")

IF(WIN32)
	SET(geotiff ${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2)
	SET(geotiff "${PARENT}.lib")
	SET(libtiff ${CMAKE_SOURCE_DIR}/src/tiff-4.0.10)
	SET(libtiff "${PARENT}.lib")

	LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2)
	LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/tiff-4.0.10)

	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2/include)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2/libxtiff)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/tiff-4.0.10/include)



ENDIF(WIN32)

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
 INCLUDE("cmake/FindGeoTIFF.cmake")
 FIND_PACKAGE(GeoTIFF QUIET)
 INCLUDE("cmake/FindTinyXML.cmake")
 FIND_PACKAGE(TinyXML QUIET)
ENDIF(UNIX)

IF(TINYXML_FOUND)
 message (STATUS "Building with system tinyxml")
 INCLUDE_DIRECTORIES(${TINYXML_INCLUDE_DIR})
 ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_PHOTOLAYER})
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

IF(GEOTIFF_FOUND)
  message (STATUS "Building with system libGeoTIFF")
  INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${GEOTIFF_LIBRARY})
ELSE(GEOTIFF_FOUND)
  message (STATUS "libGeoTIFF not found, either disable the support for it or fail ro use an embedded version of it")
ENDIF(GEOTIFF_FOUND)

IF(LIBTIFF_FOUND)
  message (STATUS "Building with system libTIFF")
  INCLUDE_DIRECTORIES(${LIBTIFF_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${LIBTIFF_LIBRARY})
ELSE(LIBTIFF_FOUND)
  message (STATUS "libTIFF not found, either disable the support for it or fail ro use an embedded version of it")
ENDIF(LIBTIFF_FOUND)


IF(WIN32)
FIND_LIBRARY(geotff ${CMAKE_SOURCE_DIR}/src/libgeotiff-1.4.2)
FIND_LIBRARY(libtiff ${CMAKE_SOURCE_DIR}/src/tiff-4.0.10)

TARGET_LINK_LIBRARIES(${PACKAGE_NAME} geotiff libtiff)
ENDIF(WIN32)

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")

