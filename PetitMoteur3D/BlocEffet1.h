#pragma once
#include "Objet3D.h"
#include "DispositifD3D11.h"
#include "d3dx11effect.h"
#include "Texture.h"

namespace PM3D {
	class CBlocEffet1 : public CObjet3D {
	public:
		CBlocEffet1(const float dx, const float dy, const float dz, CDispositifD3D11* pDispositif_);
		virtual ~CBlocEffet1();
		virtual void Draw();
		virtual void Anime(float tempsEcoule);
		void setTexture(CTexture* pTexture);
	private:
		float rotation;
		CDispositifD3D11* pDispositif;
		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		void InitEffet();
		ID3D11InputLayout* pVertexLayout;
		ID3D11Buffer* pConstantBuffer;

		XMMATRIX matWorld;

		//pour les effets 
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique; 
		ID3DX11EffectPass* pPasse;

		ID3D11ShaderResourceView* pTextureD3D;
		ID3D11SamplerState* pSampleState;
	};
}