#pragma once

class CDIManipulateur
{
	IDirectInput8* pDirectInput; 
	IDirectInputDevice8* pClavier; 
	IDirectInputDevice8* pSouris; 
	IDirectInputDevice8* pJoystick; 
	static bool bDejaInit;
	char tamponClavier[256];

public:
	CDIManipulateur();
	bool Init(HINSTANCE hInstance, HWND hWnd);
	void StatutClavier();
	bool ToucheAppuyee(UINT touche);
	~CDIManipulateur();
};

