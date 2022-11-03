#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCameraMgr :
	public CBase
{
	DECLARE_SINGLETON(CCameraMgr)
public:
	enum CAMERATYPE { CAMERA_PLAYER, CAMERA_FREE, CAMERA_CUTSCENE, CAMERA_CUTSCENE_ENTER, CAMERA_END };

private:
	CCameraMgr();
	virtual ~CCameraMgr();

public:
	void Initialize();
	void Tick(_float fTimedelta);

public:
	void		Change_Camera(CAMERATYPE _eCameraType);
	CAMERATYPE	Get_CameraType() { return m_eCameraType; }
	class CCamera* Get_Cam(CAMERATYPE _eCameraType);

public:
	//void		Slow_Change();
	//void		Fast_Change();

private:
	CAMERATYPE			m_eCameraType = CAMERA_PLAYER;
	class CCamera*		m_pCameras[CAMERA_END];
	
	// CBase��(��) ���� ��ӵ�
public:
	virtual void Free() override;
};

END