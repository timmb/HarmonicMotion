CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (HarmonicMotionGui CXX C)

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

#Remove Qt keywords
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
#set QL libraries
SET(QT_USE_QTOPENGL TRUE)
#This will addQT header location
INCLUDE(${QT_USE_FILE})


#Include folders 
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
FILE(GLOB FoundHeaders *.h)
FILE(GLOB myprj_src *.cpp)

# Currently only setup for OSX
SET(STATIC_LIBRARY_EXTENSION a)


#Here we add our source files
SET (myprj_src
	main.cpp
     )

#Define QT resource file.this file may define icons,images.
SET(myprj_rccs
    ../resources/myresources.qrc
   )


#QT headers needed to be processed with QT MOH compiler
SET(myprj_moh_hdrs
#	Add header files here
    )

#QT UI files needed to be processed
SET(myprj_uis
#	Add ui files here
)




#Generate CPP files from moh headers
QT4_WRAP_CPP(myprj_moh_srcs ${myprj_moh_hdrs})

#Generate resource header files 
QT4_ADD_RESOURCES(myprj_rccs_srcs ${myprj_rccs})

#Generate headers using QT UI files
QT4_WRAP_UI(myprj_ui_hdrs ${myprj_uis})

# Include directories - one per line
INCLUDE_DIRECTORIES(
)

# Link directories - one per line
LINK_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/../../bin

)

ADD_EXECUTABLE(HarmonicMotionGui ${myprj_src} ${myprj_ui_hdrs} ${myprj_moh_srcs} ${myprj_rccs_srcs} )

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
)
