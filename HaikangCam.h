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

	// ö���豸
	int EnumDevices();

	// ���豸
	int Open(int i);

	// �ر��豸
	int Close();

	// ����ץͼ
	int StartGrabbing();

	// ֹͣץͼ
	int StopGrabbing();

	// ע��ͼ�����ݻص�
	int RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser);

	// ����AOI
	int SetAOI(const unsigned int offsetX, const unsigned int Width, const unsigned int offsetY, const unsigned int Height);

	// ����֡��
	int SetFrame(const unsigned int framerate);

	// �����Զ��ع�
	int SetExposure(const unsigned int TimeLower, const unsigned int TimeUpper);
	
	// �رմ���
	int CloseTrigger();

	// ��������ģʽ
	int SetAcquisitionMode();

private:
	void* m_hDevHandle;
	MV_CC_DEVICE_INFO_LIST pstDevList;

public:
	float hk_frame;

};


#endif