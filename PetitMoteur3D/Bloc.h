#pragma once
#include "Objet3D.h"
#include "DispositifD3D11.h"


const uint16_t index_bloc[36] = {
	0,1,2, // devant 
	0,2,3, // devant 
	5,6,7, // arrière 
	5,7,4, // arrière 
	8,9,10, // dessous 
	8,10,11, // dessous 
	13,14,15, // dessus 
	13,15,12, // dessus 
	19,16,17, // gauche 
	19,17,18, // gauche 
	20,21,22, // droite 
	20,22,23 // droite };
};


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