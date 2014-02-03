CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (HarmonicMotionGui CXX C)

# Currently only setup for OSX
SET(STATIC_LIBRARY_EXTENSION a)
SET(DYNAMIC_LIBRARY_EXTENSION dylib)


# Setup only debug and release
if(CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_CONFIGURATION_TYPES Debug Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)
 endif()

#find required packages for the project
FIND_PACKAGE(Qt4 REQUIRED) 
#FIND_PACKAGE(OpenGL REQUIRED)

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
#set QT libraries
SET(QT_USE_QTOPENGL TRUE)
#This will addQT header location
INCLUDE(${QT_USE_FILE})


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
SET(MocHeaders
	#	Add header files here
	MainWindow.h
	Controller.h
	NodeOscOutWidget.h
	WidgetNode.h
    )

#QT UI files needed to be processed
SET(myprj_uis
#	Add ui files here
)




#Generate CPP files from moh headers
QT4_WRAP_CPP(GeneratedMocSources ${MocHeaders})

#Generate resource header files 
QT4_ADD_RESOURCES(myprj_rccs_srcs ${myprj_rccs})

#Generate headers using QT UI files
QT4_WRAP_UI(myprj_ui_hdrs ${myprj_uis})

# Include directories - one per line
INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/../../include
	${PROJECT_SOURCE_DIR}/../../../Cinder/include
	${PROJECT_SOURCE_DIR}/../../../Cinder/boost
	${PROJECT_SOURCE_DIR}/../../lib/CinderBlocks/BlockOpenNI/include/Include
	${PROJECT_SOURCE_DIR}/../../lib/CinderBlocks/OSC/src
)

# Link directories - one per line
LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/../../bin
	${PROJECT_SOURCE_DIR}/../../lib
	${PROJECT_SOURCE_DIR}/../../lib/macosx
	${PROJECT_SOURCE_DIR}/../../lib/CinderBlocks/BlockOpenNI/lib/macosx
)

ADD_EXECUTABLE(HarmonicMotionGui MACOSX_BUNDLE ${FoundSources} ${myprj_ui_hdrs} ${GeneratedMocSources} ${FoundHeaders} ${myprj_rccs_srcs} )

# Preprocessor definitions
set_property(
	DIRECTORY
	PROPERTY COMPILE_DEFINITIONS QT_NO_KEYWORDS QT_DLL QT_OPENGL_LIB QT_GUI_LIB QT_CORE_LIB 
)

set_property(
	DIRECTORY
	PROPERTY COMPILE_DEFINITIONS_DEBUG _DEBUG QT_DEBUG CMAKE_INTDIR="Debug" 
)

set_property(
	DIRECTORY
	PROPERTY COMPILE_DEFINITIONS_OPTIMIZED NDEBUG QT_NO_DEBUG CMAKE_INTDIR="Release" 
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
libltdl.7.${DYNAMIC_LIBRARY_EXTENSION}
libusb-1.0.0.${DYNAMIC_LIBRARY_EXTENSION}
libnimCodecs.${DYNAMIC_LIBRARY_EXTENSION}
libnimMockNodes.${DYNAMIC_LIBRARY_EXTENSION}
libnimRecorder.${DYNAMIC_LIBRARY_EXTENSION}
libOpenNI.${DYNAMIC_LIBRARY_EXTENSION}
)
