--rem @echo off

set ARCH=64
set VCVER=vc141
set VCLIB=vc14
set CASROOT=C:\OpenCASCADE-7.5.0-vc14-64\opencascade-7.5.0
set PRODUCTS_PATH=C:\OpenCASCADE-7.5.0-vc14-64
set CASDEB=

set BUILD_INSPECTOR=false
set CHECK_JDK=false
set CHECK_QT4=false
set PRODUCTS_DEFINES=-DHAVE_TBB -DHAVE_FREEIMAGE -DHAVE_FFMPEG -DHAVE_VTK -DHAVE_D3D -DHAVE_ZLIB -DHAVE_RAPIDJSON -DHAVE_OPENVR -DHAVE_E57
set CSF_DEFINES=HAVE_TBB;HAVE_FREEIMAGE;HAVE_FFMPEG;HAVE_VTK;HAVE_D3D;HAVE_ZLIB;HAVE_RAPIDJSON;HAVE_OPENVR;HAVE_E57;
set CSF_OCCTBinPath=%CASROOT%\win%ARCH%\%VCLIB%\bin%CASDEB%
set CSF_OCCTDataPath=%CASROOT%\data
set CSF_OCCTIncludePath=%CASROOT%\inc
set CSF_OCCTLibPath=%CASROOT%\win%ARCH%\%VCLIB%\lib%CASDEB%
set CSF_OCCTResourcePath=%CASROOT%\src
set CSF_OCCTSamplesPath=%CASROOT%\samples
set CSF_OCCTTestsPath=%CASROOT%\tests
set CSF_OPT_BIN32=%PRODUCTS_PATH%/tcltk-86-64/bin;%PRODUCTS_PATH%/freetype-2.5.5-vc14-64/bin;%PRODUCTS_PATH%/freeimage-3.17.0-vc14-64/bin;%PRODUCTS_PATH%/ffmpeg-3.3.4-64/bin;%PRODUCTS_PATH%/vtk-6.1.0-vc14-64/bin;%PRODUCTS_PATH%/qt5.11.2-vc14-64/bin;%PRODUCTS_PATH%/qt5.11.2-vc14-64/plugins
set CSF_OPT_BIN64=%PRODUCTS_PATH%/tcltk-86-64/bin;%PRODUCTS_PATH%/freetype-2.5.5-vc14-64/bin;%PRODUCTS_PATH%/freeimage-3.17.0-vc14-64/bin;%PRODUCTS_PATH%/ffmpeg-3.3.4-64/bin;%PRODUCTS_PATH%/tbb_2017.0.100/bin/intel64/vc14;%PRODUCTS_PATH%/vtk-6.1.0-vc14-64/bin;%PRODUCTS_PATH%/openvr-1.14.15-64/bin/win64;%PRODUCTS_PATH%/qt5.11.2-vc14-64/bin;%PRODUCTS_PATH%/qt5.11.2-vc14-64/plugins
set CSF_OPT_CMPL=
set CSF_OPT_INC=%PRODUCTS_PATH%/tcltk-86-64/include;%PRODUCTS_PATH%/freetype-2.5.5-vc14-64/include;%PRODUCTS_PATH%/freeimage-3.17.0-vc14-64/include;%PRODUCTS_PATH%/ffmpeg-3.3.4-64/include;%PRODUCTS_PATH%/rapidjson-1.1.0/include;%PRODUCTS_PATH%/tbb_2017.0.100/include;%PRODUCTS_PATH%/vtk-6.1.0-vc14-64/include/vtk-6.1;%PRODUCTS_PATH%/openvr-1.14.15-64/headers;%PRODUCTS_PATH%/qt5.11.2-vc14-64/include;%PRODUCTS_PATH%/qt5.11.2-vc14-64/include/Qt;%PRODUCTS_PATH%/qt5.11.2-vc14-64/include/QtGui;%PRODUCTS_PATH%/qt5.11.2-vc14-64/include/QtCore;%PRODUCTS_PATH%/qt5.11.2-vc14-64/include/QtWidgets;%PRODUCTS_PATH%/qt5.11.2-vc14-64/include/QtXml
set CSF_OPT_LIB32=%PRODUCTS_PATH%/tcltk-86-64/lib;%PRODUCTS_PATH%/freetype-2.5.5-vc14-64/lib;%PRODUCTS_PATH%/freeimage-3.17.0-vc14-64/lib;%PRODUCTS_PATH%/ffmpeg-3.3.4-64/lib;%PRODUCTS_PATH%/vtk-6.1.0-vc14-64/lib;%PRODUCTS_PATH%/qt5.11.2-vc14-64/lib
set CSF_OPT_LIB64=%PRODUCTS_PATH%/tcltk-86-64/lib;%PRODUCTS_PATH%/freetype-2.5.5-vc14-64/lib;%PRODUCTS_PATH%/freeimage-3.17.0-vc14-64/lib;%PRODUCTS_PATH%/ffmpeg-3.3.4-64/lib;%PRODUCTS_PATH%/tbb_2017.0.100/lib/intel64/vc14;%PRODUCTS_PATH%/vtk-6.1.0-vc14-64/lib;%PRODUCTS_PATH%/openvr-1.14.15-64/lib/win64;%PRODUCTS_PATH%/qt5.11.2-vc14-64/lib

set CSF_IGESDEFAULTS=%CSF_OCCTResourcePath%\XSTEPResource
set CSF_MDTVTEXTURESDIRECTORY=%CSF_OCCTResourcePath%\Textures
set CSF_MIGRATION_TYPES=%CSF_OCCTResourcePath%\StdResource\MigrationSheet.txt
set CSF_PLUGINDEFAULTS=%CSF_OCCTResourcePath%\StdResource
set CSF_SHADERSDIRECTORY=%CSF_OCCTResourcePath%\Shaders
set CSF_SHMESSAGE=%CSF_OCCTResourcePath%\SHMessage
set CSF_STANDARDDEFAULTS=%CSF_OCCTResourcePath%\StdResource
set CSF_STANDARDLITEDEFAULTS=%CSF_OCCTResourcePath%\StdResource
set CSF_STEPDEFAULTS=%CSF_OCCTResourcePath%\XSTEPResource
set CSF_TOBJDEFAULTS=%CSF_OCCTResourcePath%\StdResource
set CSF_TOBJMESSAGE=%CSF_OCCTResourcePath%\TObj
set CSF_XCAFDEFAULTS=%CSF_OCCTResourcePath%\StdResource
set CSF_XMLOCAFRESOURCE=%CSF_OCCTResourcePath%\XmlOcafResource
set CSF_XSMESSAGE=%CSF_OCCTResourcePath%\XSMessage

set HAVE_D3D=true
set HAVE_FFMPEG=true
set HAVE_FREEIMAGE=true
set HAVE_GLES2=false
set HAVE_LIBLZMA=false
set HAVE_OPENCL=false
set HAVE_OPENVR=true
set HAVE_RAPIDJSON=true
set HAVE_TBB=true
set HAVE_VTK=true
set PRJFMT=vcxproj
set SHORTCUT_HEADERS=false
set VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\vcvarsall.bat

set QTDIR=%PRODUCTS_PATH%\qt5.11.2-vc14-64
set QT_PLUGIN_PATH=%PRODUCTS_PATH%/qt5.11.2-vc14-64/plugins

set PATH=%CSF_OCCTBinPath%;%CSF_OPT_BIN64%;%PATH%