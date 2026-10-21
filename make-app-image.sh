#!/bin/bash
echo "This must be executed in build and the aya binaries must be in bin please fix"
mkdir -p AppDir
rm AppDir/AppRun
# cp ../resources/Aya.AppRun.sh AppDir/AppRun
mkdir -p AppDir/usr/bin/content AppDir/usr/bin/platform AppDir/usr/bin/shaders
rm -f AppDir/usr/bin/Aya.*
cp -r bin/content/* AppDir/usr/bin/content
cp -r bin/platform/* AppDir/usr/bin/platform
cp -r bin/shaders/* AppDir/usr/bin/shaders
cp -r bin/*.dat AppDir/usr/bin
cp -r bin/*.bin AppDir/usr/bin
cp -r bin/*.pak AppDir/usr/bin
cp -r bin/*.json AppDir/usr/bin
cp -r bin/locales AppDir/usr/bin
cp -r bin/*.so AppDir/usr/lib
cp -r bin/*.xml AppDir/usr/bin
cp -r bin/content/*.json AppDir/usr/bin
APPIMGPATH="${LINUXDEPLOY:=$HOME/linuxdeploy-x86_64.AppImage}"
EXECUTABLES="--executable bin/Aya.Studio --executable bin/Aya.WebHelper"
QMAKE=qmake6 NO_STRIP=true LD_LIBRARY_PATH=/usr/lib:bin $APPIMGPATH --appdir AppDir/ --desktop-file ../resources/aya-player.desktop --output appimage --icon-file ../resources/aya-64x64.png --plugin qt $EXECUTABLES 
