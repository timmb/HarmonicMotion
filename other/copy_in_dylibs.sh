#!/bin/bash

# This shell script is called as a build phase by Xcode to copy the required
# dylibs into the application bundle

FRAMEWORKS_DIR="$TARGET_BUILD_DIR/$TARGET_NAME.app/Contents/Frameworks/"
PLUGINS_DIR="$TARGET_BUILD_DIR/$TARGET_NAME.app/Contents/plugins/"
APP_FILE="$TARGET_BUILD_DIR/$TARGET_NAME.app/Contents/MacOS/$PRODUCT_NAME"

rm -rf "$FRAMEWORKS_DIR"
mkdir -p "$FRAMEWORKS_DIR"

rm -rf "$PLUGINS_DIR"
mkdir -p "$PLUGINS_DIR"

echo "Copying in dylibs..."
for DYLIB in `ls "$SRCROOT/../../Core/lib/macosx/" | grep dylib`
do
	echo $DYLIB
	cp -f "$SRCROOT/../../Core/lib/macosx/$DYLIB" "$FRAMEWORKS_DIR"
	install_name_tool -change @executable_path/$DYLIB @executable_path/../Frameworks/$DYLIB "$APP_FILE"
done

echo "Updating link from main executable for OpenNI libs..."
for DYLIB in libnimCodecs.dylib libnimMockNodes.dylib libnimRecorder.dylib libOpenNI.dylib
do
	echo $DYLIB
	install_name_tool -change $DYLIB @executable_path/../Frameworks/$DYLIB "$APP_FILE"
done


echo "Copying Qt libs, Updating id, link from main executable and inter-references..."
QT_MODULES="QtGui QtCore QtWidgets QtOpenGL QtPrintSupport QtSvg"
for MODULE in $QT_MODULES
do
	echo $MODULE
	cp -fR ~/Qt/5.3/clang_64/lib/$MODULE.framework "$FRAMEWORKS_DIR"
	install_name_tool -change ~/Qt/5.3/clang_64/lib/$MODULE.framework/Versions/5/$MODULE @executable_path/../Frameworks/$MODULE.framework/Versions/5/$MODULE "$APP_FILE"
	install_name_tool -id @executable_path/../Frameworks/$MODULE "$FRAMEWORKS_DIR/$MODULE.framework/Versions/5/$MODULE"
	for B in $QT_MODULES
	do
		install_name_tool -change ~/Qt/5.3/clang_64/lib/$B.framework/Versions/5/$B @executable_path/../Frameworks/$B.framework/Versions/5/$B "$FRAMEWORKS_DIR/$MODULE.framework/Versions/5/$MODULE"
	done
done


echo "Copying Qt plugins"
mkdir -p "$PLUGINS_DIR/platforms"
cp -fR ~/Qt/5.3/clang_64/plugins/platforms/libqcocoa.dylib "$PLUGINS_DIR/platforms"
install_name_tool -id @executable_path/../plugins/platforms/libqcocoa.dylib "$PLUGINS_DIR/platforms/libqcocoa.dylib"
for MODULE in $QT_MODULES
do
	install_name_tool -change ~/Qt/5.3/clang_64/lib/$MODULE.framework/Versions/5/$MODULE @executable_path/../Frameworks/$MODULE.framework/Versions/5/$MODULE "$PLUGINS_DIR/platforms/libqcocoa.dylib"
done

IMAGE_FORMATS="libqdds.dylib libqgif.dylib libqicns.dylib libqico.dylib libqjp2.dylib libqjpeg.dylib libqmng.dylib libqsvg.dylib libqtga.dylib libqtiff.dylib libqwbmp.dylib libqwebp.dylib"
mkdir -p "$PLUGINS_DIR/imageformats"
for FORMAT in $IMAGE_FORMATS
do
	cp -fR ~/Qt/5.3/clang_64/plugins/imageformats/$FORMAT "$PLUGINS_DIR/imageformats/"
	install_name_tool -id @executable_path/../plugins/platforms/$FORMAT "$PLUGINS_DIR/imageformats/$FORMAT"
	for MODULE in $QT_MODULES
	do
		install_name_tool -change ~/Qt/5.3/clang_64/lib/$MODULE.framework/Versions/5/$MODULE @executable_path/../Frameworks/$MODULE.framework/Versions/5/$MODULE "$PLUGINS_DIR/imageformats/$FORMAT"
	done
done






echo "Copying libusb"
install_name_tool -change /opt/local/lib/libusb-1.0.0.dylib @executable_path/../Frameworks/libusb-1.0.0.dylib "$APP_FILE"
install_name_tool -change /opt/local/lib/libusb-1.0.0.dylib @executable_path/../Frameworks/libusb-1.0.0.dylib "$FRAMEWORKS_DIR/libOpenNI.dylib"

echo "Updating id for libltdl and libusb..."
for DYLIB in libltdl.7.dylib libusb-1.0.0.dylib
do
	install_name_tool -id @executable_path/../Frameworks/$DYLIB "$FRAMEWORKS_DIR/$DYLIB"
done

XNLIBS="libXnCore.dylib libnimCodecs.dylib libOpenNI.dylib libXnDDK.dylib libXnFormats.dylib libXnDeviceFile.dylib libXnDeviceSensorV2KM.dylib libXnVFeatures_1_5_2.dylib libXnVHandGenerator_1_5_2.dylib libnimMockNodes.dylib libnimRecorder.dylib"
echo "Updating id and inter-references for XN libraries..."
for A in $XNLIBS
do
	echo $A
	install_name_tool -id @executable_path/../Frameworks/$A "$FRAMEWORKS_DIR/$A"
	for B in $XNLIBS
	do
		install_name_tool -change ../../Bin/x64-Release/$B @executable_path/../Frameworks/$B "$FRAMEWORKS_DIR/$A"
		install_name_tool -change ../../Bin/x86-Release/$B @executable_path/../Frameworks/$B "$FRAMEWORKS_DIR/$A"
		install_name_tool -change $B @executable_path/../Frameworks/$B "$FRAMEWORKS_DIR/$A"
	done
done

export DYLD_PRINT_LIBRARIES=1
