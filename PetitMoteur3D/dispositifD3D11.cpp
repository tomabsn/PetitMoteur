#include "StdAfx.h"
#include "resource.h"
#include "DispositifD3D11.h"
#include "Util.h"

namespace PM3D
{
CDispositifD3D11::~CDispositifD3D11()
{
	if (pImmediateContext)
	{
		pImmediateContext->ClearState();
	}

	DXRelacher(pRenderTargetView);
	DXRelacher(pImmediateContext);
	DXRelacher(pSwapChain);
	DXRelacher(pD3DDevice);
}

//	FONCTION : CDispositifD3D11, constructeur paramètré 
//	BUT :	Constructeur de notre classe spécifique de dispositif 
//	PARAMÈTRES:		
//		cdsMode:	CDS_FENETRE application fenêtrée
//					CDS_PLEIN_ECRAN application plein écran
//
//		hWnd:	Handle sur la fenêtre Windows de l'application,
//				nécessaire pour la fonction de création du 
//				dispositif
CDispositifD3D11::CDispositifD3D11(
	const CDS_MODE cdsMode,
	const HWND hWnd)
{
	UINT width;
	UINT height;
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	const UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	pImmediateContext = nullptr;
	pSwapChain = nullptr;
	pRenderTargetView = nullptr;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	switch (cdsMode)
	{
	case CDS_FENETRE:
		RECT rc;
		GetClientRect(hWnd, &rc);
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;

		sd.BufferCount = 1;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		break;

	default:
		width = 0;
		height = 0;
		assert(false); // Pas encore implémenté.
	}

	DXEssayer(D3D11CreateDeviceAndSwapChain(
		0,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevels, numFeatureLevels,
		D3D11_SDK_VERSION,
		&sd,
		&pSwapChain,
		&pD3DDevice,
		nullptr,
		&pImmediateContext),
		DXE_ERREURCREATIONDEVICE);

	// Création d'un «render target view»
	ID3D11Texture2D *pBackBuffer;
	DXEssayer(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer), DXE_ERREUROBTENTIONBUFFER);

	DXEssayer(pD3DDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView), DXE_ERREURCREATIONRENDERTARGET);
	pBackBuffer->Release();

	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pImmediateContext->RSSetViewports(1, &vp);
}

void CDispositifD3D11::PresentSpecific()
{
	pSwapChain->Present(0, 0);
}

} // namespace PM3D
