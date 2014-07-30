CMAKE_MINIMUM_REQUIRED (VERSION 2.8.9)
PROJECT (HarmonicMotionGui CXX C)

set(QT5_PATH $ENV{HOME}/Qt/5.3/clang_64/ CACHE PATH "Path to Qt5")
set(QT5_MODULE_PATH ${QT5_PATH}/lib/cmake)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT5_MODULE_PATH})

# Currently only setup for OSX
SET(STATIC_LIBRARY_EXTENSION a)
SET(DYNAMIC_LIBRARY_EXTENSION dylib)

set(OS macosx)


# Setup only debug and release
if(CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_CONFIGURATION_TYPES Debug Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)
 endif()

#find required packages for the project
#FIND_PACKAGE(Qt4 REQUIRED)
##FIND_PACKAGE(OpenGL REQUIRED)
find_package(Qt5Core)

set(CMAKE_AUTOMOC ON)

# Cinder dependencies for OSX
# http://www.itk.org/Wiki/CMake:HowToUseExistingOSXFrameworks
FIND_LIBRARY(ACCELERATE_FRAMEWORK Accelerate)
FIND_LIBRARY(APPKIT_FRAMEWORK AppKit)
FIND_LIBRARY(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
FIND_LIBRARY(AUDIOUNIT_FRAMEWORK AudioUnit)
FIND_LIBRARY(COCOA_FRAMEWORK Cocoa)
FIND_LIBRARY(COREAUDIO_FRAMEWORK CoreAudio)
FIND_LIBRARY(COREVIDEO_FRAMEWORK CoreVideo)
FIND_LIBRARY(FOUNDATION_FRAMEWORK Foundation)
FIND_LIBRARY(OPENGL_FRAMEWORK OpenGL)
FIND_LIBRARY(QTKIT_FRAMEWORK QTKit)

MARK_AS_ADVANCED(ACCELERATE_FRAMEWORK AUDIOTOOLBOX_FRAMEWORK AUDIOUNIT_FRAMEWORK COCOA_FRAMEWORK COREAUDIO_FRAMEWORK COREVIDEO_FRAMEWORK OPENGL_FRAMEWORK APPKIT_FRAMEWORK FOUNDATION_FRAMEWORK QTKIT_FRAMEWORK)

SET(CINDER_DEPENDENCIES 
${ACCELERATE_FRAMEWORK}
${APPKIT_FRAMEWORK}
${AUDIOTOOLBOX_FRAMEWORK}
${AUDIOUNIT_FRAMEWORK}
${COCOA_FRAMEWORK}
${COREAUDIO_FRAMEWORK}
${COREVIDEO_FRAMEWORK}
${FOUNDATION_FRAMEWORK}
${OPENGL_FRAMEWORK}
${QTKIT_FRAMEWORK}
libboost_date_time.${STATIC_LIBRARY_EXTENSION}
libboost_filesystem.${STATIC_LIBRARY_EXTENSION}
libboost_system.${STATIC_LIBRARY_EXTENSION}
libboost_chrono.${STATIC_LIBRARY_EXTENSION}
libboost_thread.${STATIC_LIBRARY_EXTENSION}
libz.${STATIC_LIBRARY_EXTENSION}
)

# Cinder dependency requires C++11
# This enables it in Xcode.
set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

#Remove Qt keywords
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
##set QT libraries
#SET(QT_USE_QTOPENGL TRUE)
##This will addQT header location
#INCLUDE(${QT_USE_FILE})


#Include folders 
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
FILE(GLOB FoundHeaders *.h)
FILE(GLOB FoundSources *.cpp)


#Here we add our source files
#SET (myprj_src
#	main.cpp
#     )

#Define QT resource file.this file may define icons,images.
SET(myprj_rccs
    ../resources/myresources.qrc
   )


#QT headers needed to be processed with QT MOH compiler
#SET(MocHeaders
#	#	Add header files here
#	MainWindow.h
#	Controller.h
#	NodeOscOutWidget.h
#	WidgetNode.h
#   )

#QT UI files needed to be processed
SET(myprj_uis
#	Add ui files here
)




##Generate CPP files from moh headers
#QT4_WRAP_CPP(GeneratedMocSources ${MocHeaders})

#Generate resource header files 
QT5_ADD_RESOURCES(myprj_rccs_srcs ${myprj_rccs})

#Generate headers using QT UI files
if (myprj_uis)
	QT_WRAP_UI(myprj_ui_hdrs ${myprj_uis})
endif (myprj_uis)

set(HarmonicMotionDir ${PROJECT_SOURCE_DIR}/../../Core)
set(CinderDir ${HarmonicMotionDir}/../../Cinder)


# Include directories - one per line
INCLUDE_DIRECTORIES(
	${HarmonicMotionDir}/include
	${CinderDir}/include
	${CinderDir}/boost
	${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/include/Include
	${HarmonicMotionDir}/lib/CinderBlocks/OSC/src
	${CinderDir}/blocks/Cinder-OpenCV/include
)

# Link directories - one per line
LINK_DIRECTORIES(
	${HarmonicMotionDir}/bin
	${HarmonicMotionDir}/lib
	${HarmonicMotionDir}/lib/${OS}
#	${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/lib/macosx
	${CinderDir}/blocks/Cinder-OpenCV/lib/${OS}
)

ADD_EXECUTABLE(HarmonicMotionGui MACOSX_BUNDLE WIN32 ${FoundSources} ${myprj_ui_hdrs} ${GeneratedMocSources} ${FoundHeaders} ${myprj_rccs_srcs} )

qt5_use_modules(HarmonicMotionGui Widgets OpenGL)

# Preprocessor definitions
set_property(
	DIRECTORY
	PROPERTY COMPILE_DEFINITIONS QT_NO_KEYWORDS QT_DLL QT_OPENGL_LIB QT_GUI_LIB QT_CORE_LIB 
)

set_property(
	DIRECTORY
	PROPERTY COMPILE_DEFINITIONS_DEBUG DEBUG _DEBUG QT_DEBUG CMAKE_INTDIR="Debug"
	HM_LOG_DEBUG
)

set_property(
	DIRECTORY
	PROPERTY COMPILE_DEFINITIONS_OPTIMIZED _NDEBUG NDEBUG QT_NO_DEBUG CMAKE_INTDIR="Release" 
)


TARGET_LINK_LIBRARIES(HarmonicMotionGui
${QT_LIBRARIES} 
# Additional libraries - one per line
# prefix with debug/optimized to only include in that configuration
optimized libHarmonicMotion.${STATIC_LIBRARY_EXTENSION}
debug libHarmonicMotiond.${STATIC_LIBRARY_EXTENSION}
optimized libcinder.${STATIC_LIBRARY_EXTENSION}
debug libcinder_d.${STATIC_LIBRARY_EXTENSION}
${CINDER_DEPENDENCIES}

# OpenNI
#libltdl.7.${DYNAMIC_LIBRARY_EXTENSION}
libusb-1.0.0.${DYNAMIC_LIBRARY_EXTENSION}
libnimCodecs.${DYNAMIC_LIBRARY_EXTENSION}
libnimMockNodes.${DYNAMIC_LIBRARY_EXTENSION}
libnimRecorder.${DYNAMIC_LIBRARY_EXTENSION}
libOpenNI.${DYNAMIC_LIBRARY_EXTENSION}

#libXnCore.${DYNAMIC_LIBRARY_EXTENSION}
#libXnDDK.${DYNAMIC_LIBRARY_EXTENSION}
#libXnDeviceFile.${DYNAMIC_LIBRARY_EXTENSION}
#libXnDeviceSensorV2KM.${DYNAMIC_LIBRARY_EXTENSION}
#libXnFormats.${DYNAMIC_LIBRARY_EXTENSION}
#libXnVCNITE_1_3_1.${DYNAMIC_LIBRARY_EXTENSION}
#libXnVFeatures_1_3_1.${DYNAMIC_LIBRARY_EXTENSION}
#libXnVHandGenerator_1_3_1.${DYNAMIC_LIBRARY_EXTENSION}
#libXnVNite_1_3_1.${DYNAMIC_LIBRARY_EXTENSION}

# OpenCV
libopencv_core.${DYNAMIC_LIBRARY_EXTENSION}
libopencv_imgproc.${DYNAMIC_LIBRARY_EXTENSION}
libopencv_features2d.${DYNAMIC_LIBRARY_EXTENSION}
)

# Custom script to copy in dylibs in Xcode
add_custom_command(TARGET HarmonicMotionGui
	POST_BUILD
	COMMAND /bin/bash ${PROJECT_SOURCE_DIR}/../other/copy_in_dylibs.sh
)

# # Copy dynamic libraries to bundle
# set(plugin_dest_dir bin)
# set(qtconf_dest_dir bin)
# if(APPLE)
# 	set(plugin_dest_dir HarmonicMotionGui.app/Contents/MacOS)
# 	set(QTCONF_DEST_DIR HarmonicMotionGui.app/Contents/Resources)
# 	set(APPS "\${CMAKE_INSTALL_PREFIX}/HarmonicMotionGui.app")
# endif(APPLE)
# if(WIN32)
# 	set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/HarmonicMotionGui.exe")
# endif(WIN32)

# #--------------------------------------------------------------------------------
# # Install the application, on Apple, the bundle is at the root of the
# # install tree, and on other platforms it'll go into the bin directory.
# INSTALL(TARGETS HarmonicMotionGui
# 	BUNDLE DESTINATION . COMPONENT Runtime
# 	RUNTIME DESTINATION bin COMPONENT Runtime
# 	)

# #--------------------------------------------------------------------------------
# # Install needed Qt plugins by copying directories from the qt installation
# # One can cull what gets copied by using 'REGEX "..." EXCLUDE'
# INSTALL(DIRECTORY "${QT_PLUGINS_DIR}/platforms" 
# 	DESTINATION ${plugin_dest_dir}/plugins 
# 	COMPONENT Runtime
# 	)

# #--------------------------------------------------------------------------------
# # install a qt.conf file
# # this inserts some cmake code into the install script to write the file
# INSTALL(CODE "
#     file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
#     " COMPONENT Runtime)


# #--------------------------------------------------------------------------------
# # Use BundleUtilities to get all other dependencies for the application to work.
# # It takes a bundle or executable along with possible plugins and inspects it
# # for dependencies.  If they are not system dependencies, they are copied.

# # directories to look for dependencies
# SET(DIRS ${QT_LIBRARY_DIRS})
# if (APPLE)
# 	set(DIRS ${DIRS} ${PROJECT_SOURCE_DIR}/../../lib/macosx)
# endif (APPLE)

# # Now the work of copying dependencies into the bundle/package
# # The quotes are escaped and variables to use at install time have their $ escaped
# # An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
# # Note that the image plugins depend on QtSvg and QtXml, and it got those copied
# # over.
# INSTALL(CODE "
#     file(GLOB_RECURSE QTPLUGINS
#       \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
#     include(BundleUtilities)
#     fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
#     " COMPONENT Runtime)


# # To Create a package, one can run "cpack -G DragNDrop CPackConfig.cmake" on Mac OS X
# # where CPackConfig.cmake is created by including CPack
# # And then there's ways to customize this as well
# set(CPACK_BINARY_DRAGNDROP ON)
# include(CPack)


