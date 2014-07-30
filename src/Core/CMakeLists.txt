project(HarmonicMotionCore CXX C)

set(PROJECT_INCLUDE_DIR ${HarmonicMotionIncludeDir}/Core)


# find source files
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_INCLUDE_DIR})
file(GLOB FoundHeaders ${PROJECT_INCLUDE_DIR}/*.h)
file(GLOB FoundSources *.cpp)
file(GLOB FoundHeadersNodes ${PROJECT_INCLUDE_DIR}/nodes/*.h)
file(GLOB FoundSourcesNodes nodes/*.cpp)

# Addons
file(GLOB VOpenNIHeaders ${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/src/*.h)
file(GLOB VOpenNISources ${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/src/*.cpp)
file(GLOB OSCHeaders ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/*.h ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/osc/*.h ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/*.h)
file(GLOB OSCSources ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/*.cpp ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/osc/*.cpp ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/*.cpp)
if (${OS} STREQUAL macosx)
	file(GLOB OSCHeaders_macosx ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/posix/*.h)
	file(GLOB OSCSources_macosx ${HarmonicMotionDir}/lib/CinderBlocks/OSC/src/ip/posix/*.cpp)
	set(OSCHeaders ${OSCHeaders} ${OSCHeaders_macosx})
	set(OSCSources ${OSCSources} ${OSCSources_macosx})
endif()



source_group("Source Files" FILES ${FoundHeaders} ${FoundSources})
source_group("Source Files\\nodes" FILES ${FoundHeadersNodes} ${FoundSourcesNodes})
source_group("Blocks\\VOpenNI" FILES ${VOpenNIHeaders} ${VOpenNISources})
source_group("Blocks\\OSC" FILES ${OSCHeaders} ${OSCSources})

include_directories(
		${ExternalIncludeDirs}
)



add_library(HarmonicMotionCore STATIC 
	${FoundHeaders} 
	${FoundSources} 
	${FoundHeadersNodes} 
	${FoundSourcesNodes}
	${VOpenNIHeaders}
	${VOpenNISources}
	${OSCHeaders}
	${OSCSources}
	)