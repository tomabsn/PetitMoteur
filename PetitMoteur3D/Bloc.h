#pragma once
#include "Objet3D.h"
#include "DispositifD3D11.h"

namespace PM3D {
	class CBloc : public CObjet3D {
	public:
		CBloc(const float dx, const float dy, const float dz, CDispositifD3D11* pDispositif_);
		virtual ~CBloc();
		virtual void Draw();
		virtual void Anime(float tempsEcoule);
	private:
		float rotation;
		CDispositifD3D11* pDispositif;
		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		void InitShaders();
		ID3D11VertexShader* pVertexShader;
		ID3D11InputLayout* pVertexLayout;
		ID3D11Buffer* pConstantBuffer;
		ID3D11PixelShader* pPixelShader;

		XMMATRIX matWorld;
	};
}