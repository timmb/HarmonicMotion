PROJECT (HarmonicMotionGui CXX C)


set(HarmonicMotionDir ${PROJECT_SOURCE_DIR}/../..)
set(CinderDir ${HarmonicMotionDir}/../Cinder)
set(PROJECT_INCLUDE_DIR ${HarmonicMotionIncludeDir}/Gui)


set(CMAKE_DEBUG_POSTFIX "Debug")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${HarmonicMotionDir}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${HarmonicMotionDir}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${HarmonicMotionDir}/bin)

# Setup only debug and release
if(CMAKE_CONFIGURATION_TYPES)
   set(CMAKE_CONFIGURATION_TYPES Debug;Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)
 endif()

# Enable c++11 support in Xcode
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
# #-->

if (${OS} MATCHES "macosx")
	set(QT5_PATH $ENV{HOME}/Qt/5.6/clang_64/ CACHE PATH "Path to Qt5")

	SET(STATIC_LIBRARY_EXTENSION a)
	SET(DYNAMIC_LIBRARY_EXTENSION dylib)
elseif (${OS} MATCHES "windows")
	set(QT5_PATH C:/Qt/5.5/msvc2013_64 CACHE PATH "Path to Qt5")
	set(STATIC_LIBRARY_EXTENSION lib)
	set(DYNAMIC_LIBRARY_EXTENSION dll)
endif()

set(QT5_MODULE_PATH ${QT5_PATH}/lib/cmake)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT5_MODULE_PATH})


#find required packages for the project
find_package(Qt5Core)

set(CMAKE_AUTOMOC ON)

if (${OS} MATCHES "macosx")
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
	)
endif()


# For windows: the boost libraries have a suffix
if (${OS} MATCHES "windows")
	set(boost_suffix_optimized -vc120-mt-1_56)
	set(boost_suffix_debug -vc120-mt-gd-1_56)
else()
	set(boost_suffix_optimized "")
	set(boost_suffix_debug "")
endif()


SET(CINDER_DEPENDENCIES 
	${CINDER_DEPENDENCIES}
	optimized libboost_date_time${boost_suffix_optimized}.${STATIC_LIBRARY_EXTENSION}
	optimized libboost_filesystem${boost_suffix_optimized}.${STATIC_LIBRARY_EXTENSION}
	optimized libboost_system${boost_suffix_optimized}.${STATIC_LIBRARY_EXTENSION}
	optimized libboost_chrono${boost_suffix_optimized}.${STATIC_LIBRARY_EXTENSION}
	optimized libboost_thread${boost_suffix_optimized}.${STATIC_LIBRARY_EXTENSION}
	debug libboost_date_time${boost_suffix_debug}.${STATIC_LIBRARY_EXTENSION}
	debug libboost_filesystem${boost_suffix_debug}.${STATIC_LIBRARY_EXTENSION}
	debug libboost_system${boost_suffix_debug}.${STATIC_LIBRARY_EXTENSION}
	debug libboost_chrono${boost_suffix_debug}.${STATIC_LIBRARY_EXTENSION}
	debug libboost_thread${boost_suffix_debug}.${STATIC_LIBRARY_EXTENSION}
)

if (${OS} MATCHES "macosx")
set(CINDER_DEPENDENCIES
	${CINDER_DEPENDENCIES}
	libz.${STATIC_LIBRARY_EXTENSION}
)
elseif(${OS} MATCHES "windows")
set(CINDER_DEPENDENCIES
	${CINDER_DEPENDENCIES}	
	zlib.${STATIC_LIBRARY_EXTENSION}
)
endif()


# Include folders

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ${PROJECT_INCLUDE_DIR})
FILE(GLOB FoundHeaders ${PROJECT_INCLUDE_DIR}/*.h)
FILE(GLOB FoundSources *.cpp)

#Define QT resource file.this file may define icons,images.
SET(myprj_rccs
    ${HarmonicMotionDir}/resources/myresources.qrc
   )


#QT UI files needed to be processed
SET(myprj_uis
#	Add ui files here
)

# Generate resource header files 
QT5_ADD_RESOURCES(myprj_rccs_srcs ${myprj_rccs})

# Generate headers using QT UI files
if (myprj_uis)
	QT_WRAP_UI(myprj_ui_hdrs ${myprj_uis})
endif (myprj_uis)



# Include directories - one per line
INCLUDE_DIRECTORIES(
	${HarmonicMotionDir}/include/Core
	${ExternalIncludeDirs}
)

# Link directories - one per line
LINK_DIRECTORIES(
	${HarmonicMotionDir}/lib/lib/${OS}
)

if (${OS} MATCHES "macosx")
link_directories(
#	${CinderDir}/lib/macosx
)
	if (${IncludeOpenNI})
		link_directories(
			${HarmonicMotionDir}/lib/CinderBlocks/BlockOpenNI/lib/macosx
			)
	endif()

elseif(${OS} MATCHES "windows")
link_directories(
	#	${CinderDir}/lib/msw/x64
	# Cinder is shipped with boost builds that link to static runtime library
	# but we have to use dynamic linking to runtime library (for Qt) so we
	# use our own builds of boost instead. Likewise for OpenCV
	# ${HarmonicMotionDir}/lib/CinderBlocks/Cinder-OpenCV/lib/vc2013/x64
	${HarmonicMotionDir}/lib/windows/x64
)
endif()

if (${IncludeKinectSDK2})
	include_directories(${KinectSdk2IncludeDirs})
	link_directories(${KinectSdk2LibDirs})
endif()

ADD_EXECUTABLE(HarmonicMotionGui MACOSX_BUNDLE WIN32 ${FoundSources} ${myprj_ui_hdrs} ${GeneratedMocSources} ${FoundHeaders} ${myprj_rccs_srcs} )

qt5_use_modules(HarmonicMotionGui Widgets OpenGL)

# Preprocessor definitions
set_property(
	DIRECTORY APPEND PROPERTY
	COMPILE_DEFINITIONS
	QT_NO_KEYWORDS QT_DLL QT_OPENGL_LIB QT_GUI_LIB QT_CORE_LIB
	$<$<CONFIG:Debug>:QT_DEBUG>
	$<$<CONFIG:Release>:QT_NO_DEBUG>
)


TARGET_LINK_LIBRARIES(HarmonicMotionGui
	${QT_LIBRARIES} 
	# Additional libraries - one per line
	# prefix with debug/optimized to only include in that configuration

	HarmonicMotionCore
	#Cinder

	${CINDER_DEPENDENCIES}

)

if (${OS} MATCHES "macosx")
	TARGET_LINK_LIBRARIES(HarmonicMotionGui
		optimized libcinder.a
		debug libcinder_d.a
		# OpenCV
		debug libopencv_calib3dd.a
		debug libopencv_contribd.a
		debug libopencv_cored.a
		debug libopencv_features2dd.a
		debug libopencv_flannd.a
		debug libopencv_gpud.a
		debug libopencv_highguid.a
		debug libopencv_imgprocd.a
		debug libopencv_legacyd.a
		debug libopencv_mld.a
		debug libopencv_nonfreed.a
		debug libopencv_objdetectd.a
		debug libopencv_photod.a
		debug libopencv_stitchingd.a
		debug libopencv_tsd.a
		debug libopencv_videod.a
		debug libopencv_videostabd.a
		optimized libopencv_calib3d.a
		optimized libopencv_contrib.a
		optimized libopencv_core.a
		optimized libopencv_features2d.a
		optimized libopencv_flann.a
		optimized libopencv_gpu.a
		optimized libopencv_highgui.a
		optimized libopencv_imgproc.a
		optimized libopencv_legacy.a
		optimized libopencv_ml.a
		optimized libopencv_nonfree.a
		optimized libopencv_objdetect.a
		optimized libopencv_photo.a
		optimized libopencv_stitching.a
		optimized libopencv_ts.a
		optimized libopencv_video.a
		optimized libopencv_videostab.a
	)
elseif (${OS} MATCHES "windows")
	TARGET_LINK_LIBRARIES(HarmonicMotionGui
		optimized cinder-v120.${STATIC_LIBRARY_EXTENSION}
		debug cinder-v120_d.${STATIC_LIBRARY_EXTENSION}
		# OpenCV
		optimized opencv_core249.${STATIC_LIBRARY_EXTENSION}
		optimized opencv_imgproc249.${STATIC_LIBRARY_EXTENSION}
		optimized opencv_features2d249.${STATIC_LIBRARY_EXTENSION}
		optimized opencv_flann249.${STATIC_LIBRARY_EXTENSION}
		optimized opencv_video249.lib
		debug opencv_core249d.${STATIC_LIBRARY_EXTENSION}
		debug opencv_imgproc249d.${STATIC_LIBRARY_EXTENSION}
		debug opencv_features2d249d.${STATIC_LIBRARY_EXTENSION}
		debug opencv_flann249d.${STATIC_LIBRARY_EXTENSION}
		debug opencv_video249d.lib
	)
endif()

if (${IncludeOpenNI})
	TARGET_LINK_LIBRARIES(HarmonicMotionGui
		# OpenNI
		libusb-1.0.0.${DYNAMIC_LIBRARY_EXTENSION}
		libnimCodecs.${DYNAMIC_LIBRARY_EXTENSION}
		libnimMockNodes.${DYNAMIC_LIBRARY_EXTENSION}
		libnimRecorder.${DYNAMIC_LIBRARY_EXTENSION}
		libOpenNI.${DYNAMIC_LIBRARY_EXTENSION}
	)
endif()

if (${IncludeKinectSDK2})
	target_link_libraries(HarmonicMotionGui ${KinectSdk2Libs})
endif()

if (${OS} MATCHES "macosx")
	# Custom script to copy in dylibs in Xcode
	add_custom_command(TARGET HarmonicMotionGui
		POST_BUILD
		COMMAND /bin/bash ${HarmonicMotionDir}/other/copy_in_dylibs.sh
	)
endif()

if (${OS} MATCHES "windows")
# Required from Cinder
	set(Qt5BinDebug 
			Qt5Cored.dll 
			Qt5Guid.dll 
			Qt5OpenGLd.dll 
			Qt5Widgetsd.dll
		)
	set(Qt5PlatformDebug
			qwindowsd.dll 
		)

	set(Qt5ImageFormatsDebug
			qddsd.dll
			qgifd.dll
			qicod.dll
			qicnsd.dll
			qicod.dll
			qjp2d.dll
			qjpegd.dll
			qmngd.dll
			qsvgd.dll
			qtgad.dll
			qtiffd.dll
			qwbmpd.dll
			qwebpd.dll
		)
	set(Qt5BinRelease
			Qt5Core.dll 
			Qt5Gui.dll 
			Qt5OpenGL.dll 
			Qt5Widgets.dll
		)
	set(Qt5PlatformRelease
			qwindows.dll 
		)
	set(Qt5ImageFormatsRelease
			qdds.dll
			qgif.dll
			qico.dll
			qicns.dll
			qico.dll
			qjp2.dll
			qjpeg.dll
			qmng.dll
			qsvg.dll
			qtga.dll
			qtiff.dll
			qwbmp.dll
			qwebp.dll
		)
	set(Qt5BinBoth
			icudt52.dll
			icuin52.dll
			icuuc52.dll
		)
	set(OtherDlls
			${HarmonicMotionDir}/lib/CinderBlocks/Cinder-KCB2/lib/x64/KCBv2.dll
	)

# Until we figure a working method to make add_custom_command configuration-specific
# if(CMAKE_BUILD_TYPE MATCHES DEBUG)
# 	set(BIN_DLLS ${Qt5BinDebug} ${Qt5BinBoth})
# 	set(PLUGIN_DLLS ${Qt5PluginDebug})
# else()
# 	set(BIN_DLLS ${Qt5BinRelease} ${Qt5BinBoth})
# 	set(PLUGIN_DLLS ${Qt5PluginRelease})
# endif()
set(BIN_DLLS ${Qt5BinDebug} ${Qt5BinRelease} ${Qt5BinBoth})
set(PLATFORM_DLLS ${Qt5PlatformDebug} ${Qt5PlatformRelease})
set(IMAGEFORMAT_DLLS ${Qt5ImageFormatsDebug} ${Qt5ImageFormatsRelease})
	set_target_properties(HarmonicMotionGui PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libcmt")
	# Custom script to copy in dlls in VS2013
	add_custom_command(TARGET HarmonicMotionGui
		POST_BUILD
		COMMAND set BIN_DLLS=${BIN_DLLS}
		COMMAND set PLATFORM_DLLS=${PLATFORM_DLLS}
		COMMAND set IMAGEFORMAT_DLLS=${IMAGEFORMAT_DLLS}
		COMMAND set QT5_PATH=${QT5_PATH}
		COMMAND set EXE_PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>
		COMMAND set OTHER_DLLS=${OtherDlls}
		COMMAND ${HarmonicMotionDir}/other/copy_in_dlls.bat
		)
endif()


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


