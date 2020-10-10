#include "StdAfx.h"
#include "resource.h"
#include "DispositifD3D11.h"
#include "Util.h"
#include "InfoDispositif.h"

namespace PM3D
{
CDispositifD3D11::~CDispositifD3D11()
{
	pSwapChain->SetFullscreenState(FALSE, NULL); // passer en mode fenêtré

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
		D3D_FEATURE_LEVEL_11_0, 
		D3D_FEATURE_LEVEL_10_1, 
		D3D_FEATURE_LEVEL_10_0,
	};
	const UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	pImmediateContext = nullptr;
	pSwapChain = nullptr;
	pRenderTargetView = nullptr;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	// Obtenir les informations de l’adaptateur courant 
	CInfoDispositif Dispo0( ADAPTATEUR_COURANT );

	width = 1024;
	height = 768;

	switch (cdsMode)
	{
	case CDS_FENETRE:
		sd.Windowed = TRUE;

		break;

	case CDS_PLEIN_ECRAN:

		sd.Windowed = FALSE;

		break;

	}

	DXGI_MODE_DESC desc; 
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
	desc.Height = height; 
	desc.Width = width; 
	desc.RefreshRate.Numerator = 60; 
	desc.RefreshRate.Denominator = 1; 
	desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; 
	desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; 
	CInfoDispositif DispoVoulu(desc); 
	DispoVoulu.GetDesc(desc); 

	width = desc.Width;
	height = desc.Height; 

	largeurEcran = width;
	hauteurEcran = height;

	sd.BufferCount = 1; 
	sd.BufferDesc.Width = desc.Width; 
	sd.BufferDesc.Height = desc.Height; 
	sd.BufferDesc.Format = desc.Format; 
	sd.BufferDesc.RefreshRate.Numerator = desc.RefreshRate.Numerator; 
	sd.BufferDesc.RefreshRate.Denominator = desc.RefreshRate.Denominator; 
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 
	sd.OutputWindow = hWnd; 
	sd.SampleDesc.Count = 1; 
	sd.SampleDesc.Quality = 0;

	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	// régler le problème no 1 du passage en mode fenêtré RECT rcClient,
	POINT ptDiff; 
	RECT rcClient;
	RECT rcWindow;
	GetClientRect(hWnd, &rcClient); 
	GetWindowRect(hWnd, &rcWindow); 
	ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
	ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom; 
	MoveWindow(hWnd, rcWindow.left, rcWindow.top, width + ptDiff.x, height + ptDiff.y, TRUE);
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
