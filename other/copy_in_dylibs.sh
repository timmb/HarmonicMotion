#!/bin/bash

# This shell script is called as a build phase by Xcode to copy the required
# dylibs into the application bundle

FRAMEWORKS_DIR="$TARGET_BUILD_DIR/$TARGET_NAME.app/Contents/Frameworks/"
PLUGINS_DIR="$TARGET_BUILD_DIR/$TARGET_NAME.app/Contents/plugins/"
APP_FILE="$TARGET_BUILD_DIR/$TARGET_NAME.app/Contents/MacOS/$PRODUCT_NAME"
QT_DIR=$(find ~/Qt/ -type d -name '5.?' -maxdepth 1 | sort --reverse| sed '/^\s*$/d' | head -n 1)/clang_64

rm -rf "$FRAMEWORKS_DIR"
mkdir -p "$FRAMEWORKS_DIR"

rm -rf "$PLUGINS_DIR"
mkdir -p "$PLUGINS_DIR"

echo "Copying in dylibs..."
for DYLIB in `ls "$SRCROOT/../lib/lib/macosx/" | grep dylib`
do
	echo $DYLIB
	cp -f "$SRCROOT/../lib/lib/macosx/$DYLIB" "$FRAMEWORKS_DIR"
	install_name_tool -change @executable_path/$DYLIB @executable_path/../Frameworks/$DYLIB "$APP_FILE"
done

echo "Updating link from main executable for OpenNI libs..."
for DYLIB in libnimCodecs.dylib libnimMockNodes.dylib libnimRecorder.dylib libOpenNI.dylib libusb-1.0.0.dylib
do
	if [ -f "$FRAMEWORKS_DIR/$DYLIB" ]; then
		echo "$FRAMEWORKS_DIR/$DYLIB"
		install_name_tool -change $DYLIB @
		executable_path/../Frameworks/$DYLIB "$APP_FILE"
	else
		echo "Skipping $DYLIB as not present."
	fi
done

if [ ! -d "$QT_DIR" ]; then
	echo "Failed to locate a QT version in ~/Qt. Check copy_in_dylibs.sh for details."
fi

if [ -d "$QT_DIR" ]; then
	echo "Copying Qt libs, Updating id, link from main executable and inter-references..."
	QT_MODULES="QtGui QtCore QtWidgets QtOpenGL QtPrintSupport QtSvg"
	for MODULE in $QT_MODULES
	do
		echo $MODULE
		cp -fR "$QT_DIR/lib/$MODULE.framework" "$FRAMEWORKS_DIR"
		install_name_tool -change "$QT_DIR/lib/$MODULE.framework/Versions/5/$MODULE" @executable_path/../Frameworks/$MODULE.framework/Versions/5/$MODULE "$APP_FILE"
		install_name_tool -id @executable_path/../Frameworks/$MODULE "$FRAMEWORKS_DIR/$MODULE.framework/Versions/5/$MODULE"
		for B in $QT_MODULES
		do
			install_name_tool -change "$QT_DIR/lib/$B.framework/Versions/5/$B" @executable_path/../Frameworks/$B.framework/Versions/5/$B "$FRAMEWORKS_DIR/$MODULE.framework/Versions/5/$MODULE"
		done
	done


	echo "Copying Qt plugins"
	mkdir -p "$PLUGINS_DIR/platforms"
	cp -fR "$QT_DIR/plugins/platforms/libqcocoa.dylib" "$PLUGINS_DIR/platforms"
	install_name_tool -id @executable_path/../plugins/platforms/libqcocoa.dylib "$PLUGINS_DIR/platforms/libqcocoa.dylib"
	for MODULE in $QT_MODULES
	do
		install_name_tool -change "$QT_DIR/lib/$MODULE.framework/Versions/5/$MODULE" @executable_path/../Frameworks/$MODULE.framework/Versions/5/$MODULE "$PLUGINS_DIR/platforms/libqcocoa.dylib"
	done

	IMAGE_FORMATS="libqdds.dylib libqgif.dylib libqicns.dylib libqico.dylib libqjpeg.dylib libqsvg.dylib libqtga.dylib libqtiff.dylib libqwbmp.dylib libqwebp.dylib"
	mkdir -p "$PLUGINS_DIR/imageformats"
	for FORMAT in $IMAGE_FORMATS
	do
		cp -fR "$QT_DIR/plugins/imageformats/$FORMAT" "$PLUGINS_DIR/imageformats/"
		install_name_tool -id @executable_path/../plugins/platforms/$FORMAT "$PLUGINS_DIR/imageformats/$FORMAT"
		for MODULE in $QT_MODULES
		do
			install_name_tool -change "$QT_DIR/lib/$MODULE.framework/Versions/5/$MODULE" @executable_path/../Frameworks/$MODULE.framework/Versions/5/$MODULE "$PLUGINS_DIR/imageformats/$FORMAT"
		done
	done

fi




if [ -f "$FRAMEWORKS_DIR/libusb-1.0.0.dylib" ]; then
	echo "Copying libusb"
	install_name_tool -change /opt/local/lib/libusb-1.0.0.dylib @executable_path/../Frameworks/libusb-1.0.0.dylib "$APP_FILE"
	if [ -f "$FRAMEWORKS_DIR/libOpenNI.dylib" ]; then
		install_name_tool -change /opt/local/lib/libusb-1.0.0.dylib @executable_path/../Frameworks/libusb-1.0.0.dylib "$FRAMEWORKS_DIR/libOpenNI.dylib"
	fi
fi

for DYLIB in libltdl.7.dylib libusb-1.0.0.dylib
do
	if [ -f "$FRAMEWORKS_DIR/$DYLIB" ]; then
		echo "Updating id for $FRAMEWORKS_DIR/$DYLIB"
		install_name_tool -id @executable_path/../Frameworks/$DYLIB "$FRAMEWORKS_DIR/$DYLIB"
	fi
done

XNLIBS="libnimCodecs.dylib libOpenNI.dylib libnimMockNodes.dylib libnimRecorder.dylib libusb-1.0.0.dylib" # libXnCore.dylib libXnDDK.dylib libXnFormats.dylib libXnDeviceFile.dylib libXnDeviceSensorV2KM.dylib libXnVFeatures_1_5_2.dylib libXnVHandGenerator_1_5_2.dylib"
echo "Updating id and inter-references for XN libraries..."
for A in $XNLIBS
do
	if [ -f "$FRAMEWORKS_DIR/$A" ]; then
		echo $A
		install_name_tool -id @executable_path/../Frameworks/$A "$FRAMEWORKS_DIR/$A"
		for B in $XNLIBS
		do
			if [ -f "$FRAMEWORKS_DIR/$B" ]; then
				install_name_tool -change ../../Bin/x64-Release/$B @executable_path/../Frameworks/$B "$FRAMEWORKS_DIR/$A"
				install_name_tool -change ../../Bin/x86-Release/$B @executable_path/../Frameworks/$B "$FRAMEWORKS_DIR/$A"
				install_name_tool -change $B @executable_path/../Frameworks/$B "$FRAMEWORKS_DIR/$A"
			fi
		done
	fi
done

export DYLD_PRINT_LIBRARIES=1
