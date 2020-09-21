
#pragma once

#include "dispositif.h"

namespace PM3D
{
//
//  Classe CDispositifD3D11
//
//  BUT : 	Classe permettant d'implanter un dispositif de rendu 
// 			Direct3D
//
class CDispositifD3D11 final : public CDispositif
{
public:
	CDispositifD3D11(const CDS_MODE cdsMode, const HWND hWnd);

	virtual ~CDispositifD3D11();

	virtual void PresentSpecific() override;

	// Fonction d'accès aux membres protégés
	ID3D11Device* GetD3DDevice() { return pD3DDevice; }
	ID3D11DeviceContext*    GetImmediateContext() { return pImmediateContext; }
	IDXGISwapChain*         GetSwapChain() { return pSwapChain; }
	ID3D11RenderTargetView* GetRenderTargetView() { return pRenderTargetView; }

private:
	ID3D11Device * pD3DDevice;
	ID3D11DeviceContext*    pImmediateContext;
	IDXGISwapChain*         pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;
};

} // namespace PM3D
