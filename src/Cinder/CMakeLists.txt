project(Cinder CXX C)

set(PROJECT_SOURCE_DIR ${CinderDir}/src)
set(PROJECT_INCLUDE_DIR ${CinderDir}/include)


# find source files
file(GLOB_RECURSE FoundHeaders ${PROJECT_INCLUDE_DIR}/cinder/*.h)
file(GLOB_RECURSE FoundSources ${PROJECT_SOURCE_DIR}/cinder/*.cpp)

set(FoundHeaders
${CinderDir}/include/cinder/Arcball.h 
${CinderDir}/include/cinder/Area.h 
${CinderDir}/include/cinder/AxisAlignedBox.h 
${CinderDir}/include/cinder/BandedMatrix.h 
${CinderDir}/include/cinder/Base64.h 
${CinderDir}/include/cinder/BSpline.h 
${CinderDir}/include/cinder/BSplineFit.h 
${CinderDir}/include/cinder/Buffer.h 
${CinderDir}/include/cinder/Camera.h 
${CinderDir}/include/cinder/Capture.h 
${CinderDir}/include/cinder/CaptureImplAvFoundation.h 
${CinderDir}/include/cinder/CaptureImplCocoaDummy.h 
${CinderDir}/include/cinder/CaptureImplDirectShow.h 
${CinderDir}/include/cinder/CaptureImplQtKit.h 
${CinderDir}/include/cinder/Channel.h 
${CinderDir}/include/cinder/ChanTraits.h 
${CinderDir}/include/cinder/Cinder.h 
${CinderDir}/include/cinder/CinderAssert.h 
${CinderDir}/include/cinder/CinderMath.h 
${CinderDir}/include/cinder/CinderResources.h 
${CinderDir}/include/cinder/Clipboard.h 
${CinderDir}/include/cinder/Color.h 
${CinderDir}/include/cinder/ConcurrentCircularBuffer.h 
${CinderDir}/include/cinder/ConvexHull.h 
${CinderDir}/include/cinder/CurrentFunction.h 
${CinderDir}/include/cinder/DataSource.h 
${CinderDir}/include/cinder/DataTarget.h 
${CinderDir}/include/cinder/Display.h 
${CinderDir}/include/cinder/Easing.h 
${CinderDir}/include/cinder/Exception.h 
${CinderDir}/include/cinder/Filesystem.h 
${CinderDir}/include/cinder/Filter.h 
${CinderDir}/include/cinder/Font.h 
${CinderDir}/include/cinder/Frustum.h 
${CinderDir}/include/cinder/Function.h 
${CinderDir}/include/cinder/ImageIo.h 
# ${CinderDir}/include/cinder/ImageSourceFileQuartz.h 
${CinderDir}/include/cinder/ImageSourceFileWic.h 
${CinderDir}/include/cinder/ImageSourcePng.h 
# ${CinderDir}/include/cinder/ImageTargetFileQuartz.h 
${CinderDir}/include/cinder/ImageTargetFileWic.h 
${CinderDir}/include/cinder/Json.h 
${CinderDir}/include/cinder/KdTree.h 
${CinderDir}/include/cinder/Matrix.h 
${CinderDir}/include/cinder/Matrix22.h 
${CinderDir}/include/cinder/Matrix33.h 
${CinderDir}/include/cinder/Matrix44.h 
${CinderDir}/include/cinder/MatrixAffine2.h 
${CinderDir}/include/cinder/MatrixAlgo.h 
${CinderDir}/include/cinder/MatrixStack.h 
${CinderDir}/include/cinder/MayaCamUI.h 
${CinderDir}/include/cinder/ObjLoader.h 
${CinderDir}/include/cinder/Path2d.h 
${CinderDir}/include/cinder/Perlin.h 
${CinderDir}/include/cinder/Plane.h 
${CinderDir}/include/cinder/PolyLine.h 
${CinderDir}/include/cinder/Quaternion.h 
${CinderDir}/include/cinder/Rand.h 
${CinderDir}/include/cinder/Ray.h 
${CinderDir}/include/cinder/Rect.h 
${CinderDir}/include/cinder/Serial.h 
${CinderDir}/include/cinder/Shape2d.h 
${CinderDir}/include/cinder/Sphere.h 
${CinderDir}/include/cinder/Stream.h 
${CinderDir}/include/cinder/Surface.h 
${CinderDir}/include/cinder/System.h 
${CinderDir}/include/cinder/Text.h 
${CinderDir}/include/cinder/Thread.h 
${CinderDir}/include/cinder/Timeline.h 
${CinderDir}/include/cinder/TimelineItem.h 
${CinderDir}/include/cinder/Timer.h 
${CinderDir}/include/cinder/Triangulate.h 
${CinderDir}/include/cinder/TriMesh.h 
${CinderDir}/include/cinder/Tween.h 
${CinderDir}/include/cinder/Unicode.h 
${CinderDir}/include/cinder/Url.h 
# ${CinderDir}/include/cinder/UrlImplCocoa.h 
# ${CinderDir}/include/cinder/UrlImplCurl.h 
${CinderDir}/include/cinder/UrlImplWinInet.h 
${CinderDir}/include/cinder/Utilities.h 
${CinderDir}/include/cinder/Vector.h 
# ${CinderDir}/include/cinder/WinRTUtils.h 
${CinderDir}/include/cinder/Xml.h 
# ${CinderDir}/include/cinder/app/App.h 
# ${CinderDir}/include/cinder/app/AppBasic.h 
# ${CinderDir}/include/cinder/app/AppCocoaTouch.h 
# ${CinderDir}/include/cinder/app/AppCocoaView.h 
# ${CinderDir}/include/cinder/app/AppImplCocoaBasic.h 
# ${CinderDir}/include/cinder/app/AppImplCocoaRendererGl.h 
# ${CinderDir}/include/cinder/app/AppImplCocoaRendererQuartz.h 
# ${CinderDir}/include/cinder/app/AppImplCocoaScreenSaver.h 
# ${CinderDir}/include/cinder/app/AppImplCocoaTouchRendererGl.h 
# ${CinderDir}/include/cinder/app/AppImplCocoaTouchRendererQuartz.h 
# ${CinderDir}/include/cinder/app/AppImplMsw.h 
# ${CinderDir}/include/cinder/app/AppImplMswBasic.h 
# ${CinderDir}/include/cinder/app/AppImplMswRenderer.h 
# ${CinderDir}/include/cinder/app/AppImplMswRendererDx.h 
# ${CinderDir}/include/cinder/app/AppImplMswRendererGdi.h 
# ${CinderDir}/include/cinder/app/AppImplMswRendererGl.h 
# ${CinderDir}/include/cinder/app/AppImplMswScreenSaver.h 
# ${CinderDir}/include/cinder/app/AppImplWinRT.h 
# ${CinderDir}/include/cinder/app/AppImplWinRTBasic.h 
# ${CinderDir}/include/cinder/app/AppNative.h 
# ${CinderDir}/include/cinder/app/AppScreenSaver.h 
# ${CinderDir}/include/cinder/app/CinderView.h 
# ${CinderDir}/include/cinder/app/CinderViewCocoaTouch.h 
# ${CinderDir}/include/cinder/app/Event.h 
# ${CinderDir}/include/cinder/app/FileDropEvent.h 
# ${CinderDir}/include/cinder/app/KeyEvent.h 
# ${CinderDir}/include/cinder/app/MouseEvent.h 
# ${CinderDir}/include/cinder/app/Renderer.h 
# ${CinderDir}/include/cinder/app/RendererDx.h 
# ${CinderDir}/include/cinder/app/TouchEvent.h 
# ${CinderDir}/include/cinder/app/Window.h 
# ${CinderDir}/include/cinder/app/WinRTApp.h 
# ${CinderDir}/include/cinder/audio/Buffer.h 
# ${CinderDir}/include/cinder/audio/ChannelRouterNode.h 
# ${CinderDir}/include/cinder/audio/Context.h 
# ${CinderDir}/include/cinder/audio/Debug.h 
# ${CinderDir}/include/cinder/audio/DelayNode.h 
# ${CinderDir}/include/cinder/audio/Device.h 
# ${CinderDir}/include/cinder/audio/Exception.h 
# ${CinderDir}/include/cinder/audio/FileOggVorbis.h 
# ${CinderDir}/include/cinder/audio/FilterNode.h 
# ${CinderDir}/include/cinder/audio/GainNode.h 
# ${CinderDir}/include/cinder/audio/GenNode.h 
# ${CinderDir}/include/cinder/audio/InputNode.h 
# ${CinderDir}/include/cinder/audio/MonitorNode.h 
# ${CinderDir}/include/cinder/audio/Node.h 
# ${CinderDir}/include/cinder/audio/NodeEffects.h 
# ${CinderDir}/include/cinder/audio/NodeMath.h 
# ${CinderDir}/include/cinder/audio/OutputNode.h 
# ${CinderDir}/include/cinder/audio/PanNode.h 
# ${CinderDir}/include/cinder/audio/Param.h 
# ${CinderDir}/include/cinder/audio/SamplePlayerNode.h 
# ${CinderDir}/include/cinder/audio/SampleRecorderNode.h 
# ${CinderDir}/include/cinder/audio/SampleType.h 
# ${CinderDir}/include/cinder/audio/Source.h 
# ${CinderDir}/include/cinder/audio/Target.h 
# ${CinderDir}/include/cinder/audio/Utilities.h 
# ${CinderDir}/include/cinder/audio/Voice.h 
# ${CinderDir}/include/cinder/audio/WaveformType.h 
# ${CinderDir}/include/cinder/audio/WaveTable.h 
# ${CinderDir}/include/cinder/audio/cocoa/CinderCoreAudio.h 
# ${CinderDir}/include/cinder/audio/cocoa/ContextAudioUnit.h 
# ${CinderDir}/include/cinder/audio/cocoa/DeviceManagerAudioSession.h 
# ${CinderDir}/include/cinder/audio/cocoa/DeviceManagerCoreAudio.h 
# ${CinderDir}/include/cinder/audio/cocoa/FileCoreAudio.h 
# ${CinderDir}/include/cinder/audio/dsp/Biquad.h 
# ${CinderDir}/include/cinder/audio/dsp/Converter.h 
# ${CinderDir}/include/cinder/audio/dsp/ConverterR8brain.h 
# ${CinderDir}/include/cinder/audio/dsp/Dsp.h 
# ${CinderDir}/include/cinder/audio/dsp/Fft.h 
# ${CinderDir}/include/cinder/audio/dsp/RingBuffer.h 
# ${CinderDir}/include/cinder/audio/dsp/ooura/fftsg.h 
# ${CinderDir}/include/cinder/audio/msw/ContextWasapi.h 
# ${CinderDir}/include/cinder/audio/msw/ContextXAudio.h 
# ${CinderDir}/include/cinder/audio/msw/DeviceManagerWasapi.h 
# ${CinderDir}/include/cinder/audio/msw/FileMediaFoundation.h 
# ${CinderDir}/include/cinder/audio/msw/MswUtil.h 
${CinderDir}/include/cinder/cocoa/CinderCocoa.h 
${CinderDir}/include/cinder/cocoa/CinderCocoaTouch.h 
${CinderDir}/include/cinder/dx/DDS.h 
${CinderDir}/include/cinder/dx/DDSTextureLoader.h 
${CinderDir}/include/cinder/dx/dx.h 
${CinderDir}/include/cinder/dx/DxLight.h 
${CinderDir}/include/cinder/dx/DxRenderTarget.h 
${CinderDir}/include/cinder/dx/DxTexture.h 
${CinderDir}/include/cinder/dx/DxTextureFont.h 
${CinderDir}/include/cinder/dx/DxVbo.h 
# ${CinderDir}/include/cinder/dx/FontEnumerator.h 
${CinderDir}/include/cinder/dx/gldx.h 
${CinderDir}/include/cinder/dx/HlslProg.h 
${CinderDir}/include/cinder/dx/PlatformHelpers.h 
${CinderDir}/include/cinder/gl/DisplayList.h 
${CinderDir}/include/cinder/gl/Fbo.h 
${CinderDir}/include/cinder/gl/gl.h 
${CinderDir}/include/cinder/gl/GLee.h 
${CinderDir}/include/cinder/gl/GlslProg.h 
${CinderDir}/include/cinder/gl/Light.h 
${CinderDir}/include/cinder/gl/Material.h 
${CinderDir}/include/cinder/gl/StereoAutoFocuser.h 
${CinderDir}/include/cinder/gl/Texture.h 
${CinderDir}/include/cinder/gl/TextureFont.h 
${CinderDir}/include/cinder/gl/TileRender.h 
${CinderDir}/include/cinder/gl/Vbo.h 
${CinderDir}/include/cinder/ip/Blend.h 
${CinderDir}/include/cinder/ip/EdgeDetect.h 
${CinderDir}/include/cinder/ip/Fill.h 
${CinderDir}/include/cinder/ip/Flip.h 
${CinderDir}/include/cinder/ip/Grayscale.h 
${CinderDir}/include/cinder/ip/Hdr.h 
${CinderDir}/include/cinder/ip/Premultiply.h 
${CinderDir}/include/cinder/ip/Resize.h 
${CinderDir}/include/cinder/ip/Threshold.h 
${CinderDir}/include/cinder/ip/Trim.h 
${CinderDir}/include/cinder/msw/CinderMsw.h 
${CinderDir}/include/cinder/msw/CinderMswGdiPlus.h 
${CinderDir}/include/cinder/msw/OutputDebugStringStream.h 
${CinderDir}/include/cinder/msw/StackWalker.h 
${CinderDir}/include/cinder/params/Params.h 
${CinderDir}/include/cinder/qtime/MovieWriter.h 
${CinderDir}/include/cinder/qtime/QuickTime.h 
${CinderDir}/include/cinder/qtime/QuickTimeUtils.h 
${CinderDir}/include/cinder/svg/Svg.h 
${CinderDir}/include/cinder/svg/SvgGl.h 

)

set(FoundSources
${CinderDir}/src/cinder/Area.cpp 
${CinderDir}/src/cinder/AxisAlignedBox.cpp 
${CinderDir}/src/cinder/BandedMatrix.cpp 
${CinderDir}/src/cinder/Base64.cpp 
${CinderDir}/src/cinder/BSpline.cpp 
${CinderDir}/src/cinder/BSplineFit.cpp 
${CinderDir}/src/cinder/Buffer.cpp 
${CinderDir}/src/cinder/Camera.cpp 
${CinderDir}/src/cinder/Capture.cpp 
${CinderDir}/src/cinder/CaptureImplDirectShow.cpp 
${CinderDir}/src/cinder/Channel.cpp 
${CinderDir}/src/cinder/CinderAssert.cpp 
${CinderDir}/src/cinder/CinderMath.cpp 
${CinderDir}/src/cinder/Clipboard.cpp 
${CinderDir}/src/cinder/Color.cpp 
${CinderDir}/src/cinder/ConvexHull.cpp 
${CinderDir}/src/cinder/DataSource.cpp 
${CinderDir}/src/cinder/DataTarget.cpp 
${CinderDir}/src/cinder/Display.cpp 
${CinderDir}/src/cinder/Exception.cpp 
${CinderDir}/src/cinder/Font.cpp 
${CinderDir}/src/cinder/Frustum.cpp 
${CinderDir}/src/cinder/ImageIo.cpp 
# ${CinderDir}/src/cinder/ImageSourceFileQuartz.cpp 
${CinderDir}/src/cinder/ImageSourceFileWic.cpp 
${CinderDir}/src/cinder/ImageSourcePng.cpp 
# ${CinderDir}/src/cinder/ImageTargetFileQuartz.cpp 
${CinderDir}/src/cinder/ImageTargetFileWic.cpp 
${CinderDir}/src/cinder/Json.cpp 
${CinderDir}/src/cinder/Matrix.cpp 
${CinderDir}/src/cinder/MatrixStack.cpp 
${CinderDir}/src/cinder/ObjLoader.cpp 
${CinderDir}/src/cinder/Path2d.cpp 
${CinderDir}/src/cinder/Perlin.cpp 
${CinderDir}/src/cinder/Plane.cpp 
${CinderDir}/src/cinder/PolyLine.cpp 
${CinderDir}/src/cinder/Rand.cpp 
${CinderDir}/src/cinder/Ray.cpp 
${CinderDir}/src/cinder/Rect.cpp 
${CinderDir}/src/cinder/Serial.cpp 
${CinderDir}/src/cinder/Shape2d.cpp 
${CinderDir}/src/cinder/Sphere.cpp 
${CinderDir}/src/cinder/Stream.cpp 
${CinderDir}/src/cinder/Surface.cpp 
${CinderDir}/src/cinder/System.cpp 
${CinderDir}/src/cinder/Text.cpp 
${CinderDir}/src/cinder/Timeline.cpp 
${CinderDir}/src/cinder/TimelineItem.cpp 
${CinderDir}/src/cinder/Timer.cpp 
${CinderDir}/src/cinder/Triangulate.cpp 
${CinderDir}/src/cinder/TriMesh.cpp 
${CinderDir}/src/cinder/Tween.cpp 
${CinderDir}/src/cinder/Unicode.cpp 
${CinderDir}/src/cinder/Url.cpp 
# ${CinderDir}/src/cinder/UrlImplCurl.cpp 
${CinderDir}/src/cinder/UrlImplWinInet.cpp 
${CinderDir}/src/cinder/Utilities.cpp 
# ${CinderDir}/src/cinder/WinRTUtils.cpp 
${CinderDir}/src/cinder/Xml.cpp 
# ${CinderDir}/src/cinder/app/App.cpp 
# ${CinderDir}/src/cinder/app/AppBasic.cpp 
# ${CinderDir}/src/cinder/app/AppImplMsw.cpp 
# ${CinderDir}/src/cinder/app/AppImplMswBasic.cpp 
# ${CinderDir}/src/cinder/app/AppImplMswRendererDx.cpp 
# ${CinderDir}/src/cinder/app/AppImplMswRendererGdi.cpp 
# ${CinderDir}/src/cinder/app/AppImplMswRendererGl.cpp 
# ${CinderDir}/src/cinder/app/AppImplMswScreenSaver.cpp 
# ${CinderDir}/src/cinder/app/AppImplWinRT.cpp 
# ${CinderDir}/src/cinder/app/AppImplWinRTBasic.cpp 
# ${CinderDir}/src/cinder/app/AppScreenSaver.cpp 
# ${CinderDir}/src/cinder/app/KeyEvent.cpp 
# ${CinderDir}/src/cinder/app/Renderer.cpp 
# ${CinderDir}/src/cinder/app/RendererDx.cpp 
# ${CinderDir}/src/cinder/app/Window.cpp 
# ${CinderDir}/src/cinder/app/WinRTApp.cpp 
# ${CinderDir}/src/cinder/audio/ChannelRouterNode.cpp 
# ${CinderDir}/src/cinder/audio/Context.cpp 
# ${CinderDir}/src/cinder/audio/DelayNode.cpp 
# ${CinderDir}/src/cinder/audio/Device.cpp 
# ${CinderDir}/src/cinder/audio/FileOggVorbis.cpp 
# ${CinderDir}/src/cinder/audio/FilterNode.cpp 
# ${CinderDir}/src/cinder/audio/GenNode.cpp 
# ${CinderDir}/src/cinder/audio/InputNode.cpp 
# ${CinderDir}/src/cinder/audio/MonitorNode.cpp 
# ${CinderDir}/src/cinder/audio/Node.cpp 
# ${CinderDir}/src/cinder/audio/NodeMath.cpp 
# ${CinderDir}/src/cinder/audio/OutputNode.cpp 
# ${CinderDir}/src/cinder/audio/PanNode.cpp 
# ${CinderDir}/src/cinder/audio/Param.cpp 
# ${CinderDir}/src/cinder/audio/SamplePlayerNode.cpp 
# ${CinderDir}/src/cinder/audio/SampleRecorderNode.cpp 
# ${CinderDir}/src/cinder/audio/Source.cpp 
# ${CinderDir}/src/cinder/audio/Target.cpp 
# ${CinderDir}/src/cinder/audio/Utilities.cpp 
# ${CinderDir}/src/cinder/audio/Voice.cpp 
# ${CinderDir}/src/cinder/audio/WaveTable.cpp 
# ${CinderDir}/src/cinder/audio/cocoa/CinderCoreAudio.cpp 
# ${CinderDir}/src/cinder/audio/cocoa/ContextAudioUnit.cpp 
# ${CinderDir}/src/cinder/audio/cocoa/DeviceManagerCoreAudio.cpp 
# ${CinderDir}/src/cinder/audio/cocoa/FileCoreAudio.cpp 
# ${CinderDir}/src/cinder/audio/dsp/Biquad.cpp 
# ${CinderDir}/src/cinder/audio/dsp/Converter.cpp 
# ${CinderDir}/src/cinder/audio/dsp/ConverterR8brain.cpp 
# ${CinderDir}/src/cinder/audio/dsp/Dsp.cpp 
# ${CinderDir}/src/cinder/audio/dsp/Fft.cpp 
# ${CinderDir}/src/cinder/audio/dsp/ooura/fftsg.cpp 
# ${CinderDir}/src/cinder/audio/msw/ContextWasapi.cpp 
# ${CinderDir}/src/cinder/audio/msw/ContextXAudio.cpp 
# ${CinderDir}/src/cinder/audio/msw/DeviceManagerWasapi.cpp 
# ${CinderDir}/src/cinder/audio/msw/FileMediaFoundation.cpp 
# ${CinderDir}/src/cinder/audio/msw/MswUtil.cpp 
${CinderDir}/src/cinder/dx/DDSTextureLoader.cpp 
${CinderDir}/src/cinder/dx/dx.cpp 
${CinderDir}/src/cinder/dx/DxLight.cpp 
${CinderDir}/src/cinder/dx/DxRenderTarget.cpp 
${CinderDir}/src/cinder/dx/DxTexture.cpp 
${CinderDir}/src/cinder/dx/DxTextureFont.cpp 
${CinderDir}/src/cinder/dx/DxVbo.cpp 
# ${CinderDir}/src/cinder/dx/FontEnumerator.cpp 
${CinderDir}/src/cinder/dx/HlslProg.cpp 
${CinderDir}/src/cinder/gl/DisplayList.cpp 
${CinderDir}/src/cinder/gl/Fbo.cpp 
${CinderDir}/src/cinder/gl/gl.cpp 
${CinderDir}/src/cinder/gl/GLee.c 
${CinderDir}/src/cinder/gl/GlslProg.cpp 
${CinderDir}/src/cinder/gl/Light.cpp 
${CinderDir}/src/cinder/gl/Material.cpp 
${CinderDir}/src/cinder/gl/StereoAutoFocuser.cpp 
${CinderDir}/src/cinder/gl/Texture.cpp 
${CinderDir}/src/cinder/gl/TextureFont.cpp 
${CinderDir}/src/cinder/gl/TileRender.cpp 
${CinderDir}/src/cinder/gl/Vbo.cpp 
${CinderDir}/src/cinder/ip/Blend.cpp 
${CinderDir}/src/cinder/ip/EdgeDetect.cpp 
${CinderDir}/src/cinder/ip/Fill.cpp 
${CinderDir}/src/cinder/ip/Flip.cpp 
${CinderDir}/src/cinder/ip/Grayscale.cpp 
${CinderDir}/src/cinder/ip/Hdr.cpp 
${CinderDir}/src/cinder/ip/Premultiply.cpp 
${CinderDir}/src/cinder/ip/Resize.cpp 
${CinderDir}/src/cinder/ip/Threshold.cpp 
${CinderDir}/src/cinder/ip/Trim.cpp 
${CinderDir}/src/cinder/msw/CinderMsw.cpp 
${CinderDir}/src/cinder/msw/CinderMswGdiPlus.cpp 
${CinderDir}/src/cinder/msw/StackWalker.cpp 
${CinderDir}/src/cinder/params/Params.cpp 
${CinderDir}/src/cinder/qtime/MovieWriter.cpp 
${CinderDir}/src/cinder/qtime/QuickTime.cpp 
${CinderDir}/src/cinder/qtime/QuickTimeUtils.cpp 
${CinderDir}/src/cinder/svg/Svg.cpp 

)

source_group("Source Files" FILES ${FoundHeaders} ${FoundSources})

include_directories(
	${PROJECT_INCLUDE_DIR}
	${ExternalIncludeDirs}

	${CinderDir}/boost
	${CinderDir}/include/msw/png
	${CinderDir}/include/msw/zlib
	${CinderDir}/include/msw
	${CinderDir}/include/oggvorbis
	${CinderDir}/blocks/QuickTime/include/msw
	${CinderDir}/src/AntTweakBar
	${CinderDir}/src/libtess2
	${CinderDir}/src/r8brain
	${CinderDir}/src/linebreak
	${CinderDir}/src/DxShaders
)


set(CinderFiles 
	${FoundHeaders} 
	${FoundSources} 
)

add_library(Cinder STATIC ${CinderFiles})