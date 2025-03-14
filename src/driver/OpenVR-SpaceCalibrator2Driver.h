#pragma once

#ifdef OPENVRSpaceCalibrator2DRIVER_EXPORTS
#define OPENVRSpaceCalibrator2DRIVER_API extern "C" __declspec(dllexport)
#else
#define OPENVRSpaceCalibrator2DRIVER_API extern "C" __declspec(dllimport)
#endif
