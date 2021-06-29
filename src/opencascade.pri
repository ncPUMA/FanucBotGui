INCLUDEPATH += $$quote($$(CSF_OCCTIncludePath))
LIBS += -L$$quote($$(CSF_OCCTLibPath))

LIBS += -lTKernel -lTKMath -lTKService -lTKV3d -lTKOpenGl \
        -lTKBRep -lTKIGES -lTKSTL -lTKVRML -lTKSTEP -lTKSTEPAttr -lTKSTEP209 \
        -lTKSTEPBase -lTKGeomBase -lTKGeomAlgo -lTKG3d -lTKG2d \
        -lTKXSBase -lTKShHealing -lTKHLR -lTKTopAlgo -lTKMesh -lTKPrim \
        -lTKCDF -lTKBool -lTKBO -lTKFillet -lTKOffset -lTKLCAF -lTKCAF -lTKVCAF \
                -lTKBin -lTKXml -lTKRWMesh
