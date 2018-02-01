#ifndef _HAIKANGCAM_H_
#define _HAIKANGCAM_H_
#include "MvCameraControl.h"

#define HK_OK           0
#define HK_FAIL         -1


class HaikangCam
{
public:
	HaikangCam();
	~HaikangCam();

	// 枚举设备
	int EnumDevices();

	// 打开设备
	int Open(int i);

	// 关闭设备
	int Close();

	// 开启抓图
	int StartGrabbing();

	// 停止抓图
	int StopGrabbing();

	// 注册图像数据回调
	int RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser);

	// 设置AOI
	int SetAOI(const unsigned int offsetX, const unsigned int Width, const unsigned int offsetY, const unsigned int Height);

	// 设置帧率
	int SetFrame(const unsigned int framerate);

	// 设置自动曝光
	int SetExposure(const unsigned int TimeLower, const unsigned int TimeUpper);
	
	// 关闭触发
	int CloseTrigger();

	// 设置连续模式
	int SetAcquisitionMode();

private:
	void* m_hDevHandle;
	MV_CC_DEVICE_INFO_LIST pstDevList;

public:
	float hk_frame;

};


#endif