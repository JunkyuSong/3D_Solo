#pragma once
#include "Base.h"
#include "Cell.h"
BEGIN(Engine)
class ENGINE_DLL CPointInCell :
	public CBase
{
private:
	CPointInCell();
	virtual ~CPointInCell() = default;

public:
	HRESULT				Initialize(_float3	_vPoint);
	HRESULT				Render();

public:
	_float3 Get_Point() { return m_vPoint; }

	//�ڱ� �ڽ��� �����ϴ� ���� �� ���Ƽ� ����Ʈ�� �ٲ��ִ� �۾� <- �ش� ���� abc�� üũ? �װ� ��. ����
	void RepairPoint(_float3 _vPoint);

	//�������� �־��ִ� �۾�
	void Add_Cell(CCell* _pCell, CCell::POINT _ePoint);

	//�����Ҷ� �ڱ� �ڽ��� ����ϴ� ������ �����ϴ� �۾�<-�׺���̼� �޾ƿͼ� ó��
	// -> �׶� �ڽ��� ����ϴ� �̿��������� �̿��� ������ִ� �۾�? �ƴϸ� �� ���̹� �ϸ� �׶� �ٽ� �̿��� ���� ����?
	// ���� �ε����� �� -1�� ���������
	void Delete_Point(class CNavigation* _pNavi);
	void Delete_Cell(CCell* _pCell);

	_bool Picking();

	void Set_Index(_int _iIndex)	{ m_iIndex = _iIndex; }
	_int Get_Index() { return m_iIndex; }
	
private:
	//������ �����ϴ� ������ �ε�����...
	//�� ������ ����Ʈ�� �׺���̼ǿ��� ����ִ´�? 
	vector<pair<class CCell*, CCell::POINT>>		m_CellIs;
	//�ڱ� �ڽ��� ������
	_float3											m_vPoint;
	//�ڽ��� �浹ü
	class CCollider*								m_pCollider = nullptr;

	_int											m_iIndex = -1;

public:
	static CPointInCell* Create(_float3	_vPoint);
	virtual void Free();
};

END