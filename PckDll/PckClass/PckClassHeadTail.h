#pragma once
#include "PckClassBaseFeatures.h"
#include "PckClassZlib.h"

class CPckClassHeadTail :
	public virtual CPckClassBaseFeatures
{
public:
	CPckClassHeadTail();
	~CPckClassHeadTail();

	//���ø�����Ϣ
	const char*	GetAdditionalInfo();
	BOOL	SetAdditionalInfo(const char* lpszAdditionalInfo);

private:

};
