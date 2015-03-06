#ifdef SVSCAMERA_EXPORTS
#define SVSCAMERA_API __declspec(dllexport)
#else
#define SVSCAMERA_API __declspec(dllimport)
#endif

SVSCAMERA_API bool openCamera();
SVSCAMERA_API bool closeCamera();
SVSCAMERA_API bool startGrab();
SVSCAMERA_API bool stopGrab();
SVSCAMERA_API bool setExposureTime(float);
SVSCAMERA_API bool singleGrab(unsigned char**, int*, int*);
SVSCAMERA_API unsigned char* singleGrab(int*, int*);
SVSCAMERA_API bool getRGBImgSize(int*);
SVSCAMERA_API bool singleGrab(unsigned char*);