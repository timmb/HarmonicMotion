project(HarmonicMotionCore CXX C)

set(PROJECT_INCLUDE_DIR ${HarmonicMotionIncludeDir}/Core)


# find source files
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_INCLUDE_DIR})
file(GLOB FoundHeaders ${PROJECT_INCLUDE_DIR}/*.h)
file(GLOB FoundSources *.cpp)
file(GLOB FoundHeadersNodes ${PROJECT_INCLUDE_DIR}/nodes/*.h)
file(GLOB FoundSourcesNodes nodes/*.cpp)

# Addons
if (${IncludeOpenNI})
	message("Including OpenNI.")
	file(GLOB VOpenNIHeaders ${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/src/*.h)
	file(GLOB VOpenNISources ${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/src/*.cpp)
else()
	message("Not including OpenNI.")
endif()
if (${IncludeKinectSDK2})
	message("Including Kinect SDK.")
	file(GLOB KinectSdk2Headers ${HarmonicMotionDir}/lib/CinderBlocks/Cinder-KCB2/src/*.h)
	set(KinectSdk2Headers ${KinectSdk2Headers} ${HarmonicMotionDir}/lib/CinderBlocks/Cinder-KCB2/lib/KCBv2Lib.h)
	file(GLOB KinectSdk2Sources ${HarmonicMotionDir}/lib/CinderBlocks/Cinder-KCB2/src/*.cpp)
	include_directories(${KinectSdk2IncludeDirs})
else()
		message("Not including Kinect SDK.")
endif()
file(GLOB OSCHeaders ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/*.h ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/osc/*.h ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/*.h)
file(GLOB OSCSources ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/*.cpp ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/osc/*.cpp ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/*.cpp)
if (${OS} MATCHES "macosx")
	file(GLOB OSCHeaders_platform ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/posix/*.h)
	file(GLOB OSCSources_platform ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/posix/*.cpp)
elseif (${OS} MATCHES "windows")
	file(GLOB OSCHeaders_platform ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/win32/*.h)
	file(GLOB OSCSources_platform ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/win32/*.cpp)
endif()
set(OSCHeaders ${OSCHeaders} ${OSCHeaders_platform})
set(OSCSources ${OSCSources} ${OSCSources_platform})

# Setup only debug and release
if(CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_CONFIGURATION_TYPES Debug;Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)
 endif()



source_group("Source Files" FILES ${FoundHeaders} ${FoundSources})
source_group("Source Files\\nodes" FILES ${FoundHeadersNodes} ${FoundSourcesNodes})
if (${IncludeOpenNI})
	source_group("Blocks\\VOpenNI" FILES ${VOpenNIHeaders} ${VOpenNISources})
endif()
if (${IncludeKinectSDK2})
	source_group("Blocks\\Cinder-KCB2" FILES ${KinectSdk2Headers} ${KinectSdk2Sources})
endif()
source_group("Blocks\\OSC" FILES ${OSCHeaders} ${OSCSources})

include_directories(
		${ExternalIncludeDirs}
)


set(HarmonicMotionCoreFiles 
	${FoundHeaders} 
	${FoundSources} 
	${FoundHeadersNodes} 
	${FoundSourcesNodes}
	${OSCHeaders}
	${OSCSources}
	${VOpenNIHeaders}
	${VOpenNISources}
	${KinectSdk2Headers}
	${KinectSdk2Sources}
)

# if(${IncludeOpenNI})
# 	set(HarmonicMotionCoreFiles
# 		${HarmonicMotionCoreFiles}
# 		${VOpenNIHeaders}
# 		${VOpenNISources}
# 	)
# endif()


add_library(HarmonicMotionCore STATIC ${HarmonicMotionCoreFiles})
