cmake_minimum_required(VERSION 2.8.12)
#cmake_minimum_required(VERSION 3.0.0)

# use generators to set config-specific properties
cmake_policy(SET CMP0043 NEW)

project(HarmonicMotion CXX C)

set(HarmonicMotionDir ${PROJECT_SOURCE_DIR}/..)
set(CinderDir ${HarmonicMotionDir}/../Cinder CACHE PATH "The location of Cinder (see README.md for details of where to obtain Cinder).")


set(OS macosx)

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
	$<$<CONFIG:Debug>:	DEBUG _DEBUG CMAKE_INTDIR="Debug" HM_LOG_DEBUG >
	$<$<CONFIG:Release>: _NDEBUG NDEBUG CMAKE_INTDIR="Release" >
)

set(ExternalIncludeDirs 	
	${CinderDir}/include
	${CinderDir}/boost
	${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/include/Include
	${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/src
	${HarmonicMotionDir}/lib/CinderBlocks/OSC/src
	${CinderDir}/blocks/Cinder-OpenCV/include
)


set(HarmonicMotionIncludeDir ${HarmonicMotionDir}/include)


subdirs(Core Gui)
