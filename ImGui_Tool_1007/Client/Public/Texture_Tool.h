#pragma once
#include "Base.h"

//���� ��� : �ؽ��� �̸� �޾ƿ���
// �׷� �ؽ��İ� ��� ���ִ��� �� �� �ְ���
// �ؽ��� 1���� ���ִ� �ֵ� 0������ �ٲ� �� �ֵ���
// ��� OBJ������ pick�� ��������
// 


BEGIN(Engine)
class CTexture_Tool final :
	public CBase
{
	DECLARE_SINGLETON(CTexture_Tool)
private:
	CTexture_Tool();
	virtual ~CTexture_Tool();



	// CBase��(��) ���� ��ӵ�
public:
	virtual void Free() override;

};

END