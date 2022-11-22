#pragma once

#include "Base.h"

/* ���� ��������� ������ �����͸� ������ �ִ´�. */
/* ������ ƽ, ������ ����ȣ���Ѵ�. */
/* ��[����ü�ÿ� �������� ����,. ���ο�� ��ü�Ѵ�. */
//CLevel_Manager

BEGIN(Engine)

class CLevel;

class CLevel_Manager final :
	public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

	// ���� ���� �� ���� ����
public:
	HRESULT					Check_Level();
	HRESULT					Loading_Level(const _uint & _iLoadingLevel, CLevel * _pLoadingLevel);
	HRESULT					Reserve_Level(const _uint& _iReserveLevel);

	HRESULT					Open_Level(const _uint& _iLevelIndex, CLevel* _pNewLevel);
	void					Tick(const _float& _fTimeDelta);
	HRESULT					Render();

	const _bool&			Get_bLoading() { return m_bLoading; }
	const _uint&			Get_LoadingLv() { return m_iLoadingLevel; }

	_uint					Get_CurLv() { return m_iCurrentLevel; }

	//����, �Ҹ�
public:
	virtual void			Free() override;

private:
	CLevel*					m_pCurrentLevel;
	CLevel*					m_pLoadingLevel;
	_uint					m_iCurrentLevel;
	_uint					m_iReserveLevel;

	_uint					m_iLoadingLevel;

	_bool					m_bLoading;
};

END