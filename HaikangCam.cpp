#include "stdafx.h"
#include "HaikangCam.h"

HaikangCam::HaikangCam()
{
	m_hDevHandle = NULL;
}

HaikangCam::~HaikangCam()
{
	if (m_hDevHandle)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = NULL;
	}
}

int HaikangCam::EnumDevices()
{
	// Enum GIGE Devices
	memset(&pstDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
	int nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &pstDevList);
	if (MV_OK != nRet)
	{
		return HK_FAIL;
	}
	return HK_OK;
}

// 打开设备
int HaikangCam::Open(int i)
{
	if (NULL == pstDevList.pDeviceInfo)
	{
		return HK_FAIL;
	}

	int nRet = MV_CC_CreateHandle(&m_hDevHandle, pstDevList.pDeviceInfo[i]);
	if (MV_OK != nRet)
	{
		return HK_FAIL;
	}

	nRet = MV_CC_OpenDevice(m_hDevHandle, MV_ACCESS_Exclusive, 0);
	if (MV_OK != nRet)
	{
		MV_CC_DestroyHandle(&m_hDevHandle);
		return HK_FAIL;
	}

	return HK_OK;
}


// 关闭设备
int HaikangCam::Close()
{
	if (NULL == m_hDevHandle)
	{
		return HK_FAIL;
	}
	MV_CC_CloseDevice(m_hDevHandle);
	return MV_CC_DestroyHandle(m_hDevHandle);
}


// 开启抓图
int HaikangCam::StartGrabbing()
{
	if (NULL == m_hDevHandle)
	{
		return HK_FAIL;
	}

	return MV_CC_StartGrabbing(m_hDevHandle);
}


// 停止抓图
int HaikangCam::StopGrabbing()
{
	if (NULL == m_hDevHandle)
	{
		return HK_FAIL;
	}
	return MV_CC_StopGrabbing(m_hDevHandle);
}

// 注册图像数据回调
int HaikangCam::RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData,
	MV_FRAME_OUT_INFO_EX* pFrameInfo,
	void* pUser),
	void* pUser)
{
	if (NULL == m_hDevHandle)
	{
		return HK_FAIL;
	}

	return MV_CC_RegisterImageCallBackEx(m_hDevHandle, cbOutput, pUser);
}

int HaikangCam::SetAOI(const unsigned int offsetX, const unsigned int Width, const unsigned int offsetY, const unsigned int Height)
{
	//最大图像尺寸3072*2048
	if (NULL == m_hDevHandle)
	{
		return HK_FAIL;
	}
	//设置AOI偏移量及图像大小
	int nRet = MV_CC_SetWidth(m_hDevHandle, Width);
	/*if (MV_OK != nRet)
	{
	return HK_FAIL;
	}*/
	nRet = MV_CC_SetAOIoffsetX(m_hDevHandle, offsetX);
	/*if (MV_OK != nRet)
	{
		return HK_FAIL;
	}*/
	nRet = MV_CC_SetHeight(m_hDevHandle, Height);
	/*if (MV_OK != nRet)
	{
		return HK_FAIL;
	}*/
	nRet = MV_CC_SetAOIoffsetY(m_hDevHandle, offsetY);
	if (MV_OK != nRet)
	{
		return HK_FAIL;
	}
	return HK_OK;
}

int HaikangCam::SetExposure(const unsigned int TimeLower, const unsigned int TimeUpper)
{
	if (NULL == m_hDevHandle)
	{
		return HK_FAIL;
	}
	//设置相机自动曝光模式
	int nRet = MV_CC_SetExposureAutoMode(m_hDevHandle, MV_EXPOSURE_AUTO_MODE_CONTINUOUS);
	if (MV_OK != nRet)
	{
		return HK_FAIL;
	}
	//设置设备自动曝光值下限值
	nRet = MV_CC_SetAutoExposureTimeLower(m_hDevHandle, 1000);
	if (MV_OK != nRet)
	{
		return HK_FAIL;
	}
	//设置设备自动曝光值上限值
	nRet = MV_CC_SetAutoExposureTimeUpper(m_hDevHandle, 100000);
	if (MV_OK != nRet)
	{
		return HK_FAIL;
	}
	return HK_OK;
}

int HaikangCam::SetFrame(const unsigned int framerate)
{
	if (NULL == m_hDevHandle)
	{
		return HK_FAIL;
	}
	//设置并获取相机采集帧率信息
	int nRet = MV_CC_SetFrameRate(m_hDevHandle, framerate);
	if (MV_OK != nRet)
	{
		return HK_FAIL;
	}
	MVCC_FLOATVALUE struFloatValue = { 0 };
	nRet = MV_CC_GetFrameRate(m_hDevHandle, &struFloatValue);
	if (MV_OK != nRet)
	{
		return HK_FAIL;
	}
	hk_frame = struFloatValue.fCurValue;
	return HK_OK;
}

int HaikangCam::CloseTrigger()
{
	if (NULL == m_hDevHandle)
	{
		return HK_FAIL;
	}
	// 关闭触发
	return MV_CC_SetEnumValue(m_hDevHandle, "TriggerMode", MV_TRIGGER_MODE_OFF);
}


int HaikangCam::SetAcquisitionMode()
{
	if (NULL == m_hDevHandle)
	{
		return HK_FAIL;
	}
	// 使用连续模式
	return MV_CC_SetEnumValue(m_hDevHandle, "AcquisitionMode", MV_ACQ_MODE_CONTINUOUS);
}