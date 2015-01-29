# Options for build

   cmake -DUSE_RANGE

   Use Ricardo Yanez's 'range' library to build classes KVRangeYanez/KVRangeYanezMaterial.
	We try to automagically deduce location of library & header file from LD_LIBRARY_PATH.
	If this fails, paths can be given with
	
	cmake -DUSE_RANGE -DRANGE_LIB_DIR=[path to librange.so] -DRANGE_INC_DIR=[path to range.h]
