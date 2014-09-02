cmake_minimum_required(VERSION 2.8.12)
#cmake_minimum_required(VERSION 3.0.0)

# use generators to set config-specific properties
cmake_policy(SET CMP0043 NEW)

project(HarmonicMotion CXX C)

set(HarmonicMotionDir ${PROJECT_SOURCE_DIR}/..)
set(CinderDir ${HarmonicMotionDir}/../Cinder CACHE PATH "The location of Cinder (see README.md for details of where to obtain Cinder).")


if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
	message("macosx detected")
	set(OS macosx CACHE STRING "Operating system")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	message("windows detected")
	set(OS windows CACHE STRING "Operating system")
endif()

set(IncludeOpenNI (${OS} MATCHES macosx) CACHE BOOL "Include OpenNI (only supported on Mac OSX)")


set(CMAKE_DEBUG_POSTFIX "Debug")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${HarmonicMotionDir}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${HarmonicMotionDir}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${HarmonicMotionDir}/bin)

# Setup only debug and release
if(CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_CONFIGURATION_TYPES Debug Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)
 endif()

# Enable c++11 support
set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

# Preprocessor definitions
set_property(
	DIRECTORY APPEND PROPERTY 
	COMPILE_DEFINITIONS
	$<$<CONFIG:Debug>:	DEBUG _DEBUG  CMAKE_INTDIR="Debug" HM_LOG_DEBUG >
	$<$<CONFIG:Release>: _NDEBUG NDEBUG CMAKE_INTDIR="Release" >
)

set_property(
	DIRECTORY APPEND PROPERTY
	COMPILE_DEFINITIONS
	UNICODE _UNICODE
)

if (${OS} MATCHES "macosx")
	set_property(
		DIRECTORY APPEND PROPERTY
		COMPILE_DEFINITIONS
		HM_MACOSX
	)
elseif (${OS} MATCHES "windows")
	set_property(
		DIRECTORY APPEND PROPERTY
		COMPILE_DEFINITIONS
		HM_WINDOWS
		WIN32
		__WIN32__
		# This disables warning about unsafe C functions (printf, etc) that requests using their _s counterpart.
		# Good advice but the usage is in included libraries which we do not want to modify.
		_CRT_SECURE_NO_WARNINGS
	)
		# For some reason using pragma to disable double to float conversion warnings doesn't work in a number of places
		# so we disable them all here.
	add_definitions(/wd4244)
		# This disables warning for conversion from size_t to int, which is common in included libraries.
	add_definitions(/wd4267)
		# Enable multiprocessor compilation
	add_definitions(/MP)
		# Enable c++ exceptions
	add_definitions(/EHsc)
endif()

if (${INCLUDE_OPENNI})
	set_property(
		DIRECTORY APPEND PROPERTY
		COMPILE_DEFINITIONS
		HM_OPENNI
	)
endif()

set(ExternalIncludeDirs 	
	${CinderDir}/include
	${CinderDir}/boost
	${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/include/Include
	${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/src
	${HarmonicMotionDir}/lib/CinderBlocks/OSC/src
	${HarmonicMotionDir}/lib/CinderBlocks/Cinder-OpenCV/include
)


set(HarmonicMotionIncludeDir ${HarmonicMotionDir}/include)


subdirs(Core Gui Cinder)

#include(${PROJECT_SOURCE_DIR}/CMake/MSVCStaticRuntime.cmake)



