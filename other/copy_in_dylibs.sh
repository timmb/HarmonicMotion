#!/bin/bash

# This shell script is called as a build phase by Xcode to copy the required
# dylibs into the application bundle


for DYLIB in libltdl.7.dylib libusb-1.0.0.dylib
do
	# export DYLIB=libltdl.7.dylib
	mkdir -p "$TARGET_BUILD_DIR/$TARGET_NAME.app/Contents/Frameworks"
	cp -f "$SRCROOT/../../lib/macosx/$DYLIB" "$TARGET_BUILD_DIR/$TARGET_NAME.app/Contents/Frameworks"
	install_name_tool -change @executable_path/$DYLIB @loader_path/../Frameworks/$DYLIB "$TARGET_BUILD_DIR/$TARGET_NAME.app/Contents/MacOS/$PRODUCT_NAME"
done