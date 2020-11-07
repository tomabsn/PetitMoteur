#include "stdafx.h"
#include "BlocEffet1.h"
#include "sommetbloc.h"
#include "MoteurWindows.h"
#include "util.h"
#include "resource.h"
#include "Bloc.h"

//structure pour communiquer entre VS et PS
//tampon de constante 
struct ShadersParams {
	XMMATRIX matWorldViewProj; // la matrice totale 
	XMMATRIX matWorld; // matrice de transformation dans le monde 
	XMVECTOR vLumiere; // la position de la source d��clairage (Point) 
	XMVECTOR vCamera; // la position de la cam�ra 
	XMVECTOR vAEcl; // la valeur ambiante de l��clairage 
	XMVECTOR vAMat; // la valeur ambiante du mat�riau 
	XMVECTOR vDEcl; // la valeur diffuse de l��clairage 
	XMVECTOR vDMat; // la valeur diffuse du mat�riau 
};

namespace PM3D {

	CBlocEffet1::CBlocEffet1(const float dx, const float dy, const float dz, CDispositifD3D11* pDispositif_) : pDispositif(pDispositif_) {
		pTextureD3D = 0;
		pSampleState = 0;
		
		rotation = 0.0f;
		XMFLOAT3 point[8] = {
			XMFLOAT3(-dx / 2, dy / 2, -dz / 2),
			XMFLOAT3(dx / 2, dy / 2, -dz / 2),
			XMFLOAT3(dx / 2, -dy / 2, -dz / 2),
			XMFLOAT3(-dx / 2, -dy / 2, -dz / 2),
			XMFLOAT3(-dx / 2, dy / 2, dz / 2),
			XMFLOAT3(-dx / 2, -dy / 2, dz / 2),
			XMFLOAT3(dx / 2, -dy / 2, dz / 2),
			XMFLOAT3(dx / 2, dy / 2, dz / 2)
		};

		// Calculer les normales
		XMFLOAT3 n0(0.0f, 0.0f, -1.0f); // devant
		XMFLOAT3 n1(0.0f, 0.0f, 1.0f); // arri�re
		XMFLOAT3 n2(0.0f, -1.0f, 0.0f); // dessous
		XMFLOAT3 n3(0.0f, 1.0f, 0.0f); // dessus
		XMFLOAT3 n4(-1.0f, 0.0f, 0.0f); // face gauche
		XMFLOAT3 n5(1.0f, 0.0f, 0.0f); // face droite


		CSommetBloc sommets[24];

		// Le devant du bloc 
		sommets[0] = CSommetBloc(point[0], n0, XMFLOAT2(0.0f, 0.0f));
		sommets[1] = CSommetBloc(point[1], n0, XMFLOAT2(1.0f, 0.0f));
		sommets[2] = CSommetBloc(point[2], n0, XMFLOAT2(1.0f, 1.0f));
		sommets[3] = CSommetBloc(point[3], n0, XMFLOAT2(0.0f, 1.0f));

		// L�arri�re du bloc 
		sommets[4] = CSommetBloc(point[4], n1, XMFLOAT2(0.0f, 1.0f));
		sommets[5] = CSommetBloc(point[5], n1, XMFLOAT2(0.0f, 0.0f));
		sommets[6] = CSommetBloc(point[6], n1, XMFLOAT2(1.0f, 0.0f));
		sommets[7] = CSommetBloc(point[7], n1, XMFLOAT2(1.0f, 1.0f));

		// Le dessous du bloc 
		sommets[8] = CSommetBloc(point[3], n2, XMFLOAT2(0.0f, 0.0f));
		sommets[9] = CSommetBloc(point[2], n2, XMFLOAT2(1.0f, 0.0f));
		sommets[10] = CSommetBloc(point[6], n2, XMFLOAT2(1.0f, 1.0f));
		sommets[11] = CSommetBloc(point[5], n2, XMFLOAT2(0.0f, 1.0f));

		// Le dessus du bloc 
		sommets[12] = CSommetBloc(point[0], n3, XMFLOAT2(0.0f, 1.0f));
		sommets[13] = CSommetBloc(point[4], n3, XMFLOAT2(0.0f, 0.0f));
		sommets[14] = CSommetBloc(point[7], n3, XMFLOAT2(1.0f, 1.0f));
		sommets[15] = CSommetBloc(point[1], n3, XMFLOAT2(1.0f, 0.0f));

		// La face gauche 
		sommets[16] = CSommetBloc(point[0], n4, XMFLOAT2(0.0f, 0.0f));
		sommets[17] = CSommetBloc(point[3], n4, XMFLOAT2(1.0f, 0.0f));
		sommets[18] = CSommetBloc(point[5], n4, XMFLOAT2(1.0f, 1.0f));
		sommets[19] = CSommetBloc(point[4], n4, XMFLOAT2(0.0f, 1.0f));

		// La face droite 
		sommets[20] = CSommetBloc(point[1], n5, XMFLOAT2(0.0f, 0.0f));
		sommets[21] = CSommetBloc(point[7], n5, XMFLOAT2(1.0f, 0.0f));
		sommets[22] = CSommetBloc(point[6], n5, XMFLOAT2(1.0f, 1.0f));
		sommets[23] = CSommetBloc(point[2], n5, XMFLOAT2(0.0f, 1.0f));

		// Cr�ation du vertex buffer et copie des sommets 
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();
		D3D11_BUFFER_DESC bd; ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(sommets);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0; D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = sommets;
		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);

		// Cr�ation de l�index buffer et copie des indices 
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(index_bloc);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = index_bloc;
		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer), DXE_CREATIONINDEXBUFFER);

		matWorld = XMMatrixIdentity();

		rotation = 0.0f;

		InitEffet();
	}

	void CBlocEffet1::Draw() {
		// Obtenir le contexte 
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext(); 
		// Choisir la topologie des primitives 
		pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ); 
		// Source des sommets 
		const UINT stride = sizeof( CSommetBloc ); 
		const UINT offset = 0; 
		pImmediateContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset ); 
		// Source des index 
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0); 
		// input layout des sommets 
		pImmediateContext->IASetInputLayout( pVertexLayout ); 
		// Initialiser et s�lectionner les � constantes � de l�effet 
		ShadersParams sp; 
		XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();

		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj); 
		sp.matWorld = XMMatrixTranspose(matWorld); 
		sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f); 
		sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f); 
		sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f); 
		sp.vAMat = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f); 
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f); 
		sp.vDMat = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f); 
		
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

		// Nous n�avons qu�un seul CBuffer 
		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param"); 
		pCB->SetConstantBuffer(pConstantBuffer); 
		

		// Activation de la texture 
		ID3DX11EffectShaderResourceVariable* variableTexture;

		variableTexture = pEffet->GetVariableByName("textureEntree")->AsShaderResource();
		variableTexture->SetResource(pTextureD3D);

		// Le sampler state 
		ID3DX11EffectSamplerVariable* variableSampler; 
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler(); 
		variableSampler->SetSampler(0, pSampleState);

		// **** Rendu de l�objet 
		pPasse->Apply(0, pImmediateContext);

		// **** Rendu de l�objet 
		pImmediateContext->DrawIndexed(ARRAYSIZE(index_bloc), 0, 0);
	}

	void CBlocEffet1::InitEffet() {
		//comilation et chargemnt du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		//creation d'un taampon pour les constantes du VS
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
		bd.CPUAccessFlags = 0; 
		pD3DDevice->CreateBuffer(&bd, NULL, &pConstantBuffer);

		//pour l'effet
		ID3DBlob* pFXBlob = nullptr;

		DXEssayer(D3DCompileFromFile(L"MiniPhong.fx", 0, 0, 0, "fx_5_0", 0, 0, &pFXBlob, 0), DXE_ERREURCREATION_FX);
		
		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);
		
		pFXBlob->Release();

		pTechnique = pEffet->GetTechniqueByIndex(0); 
		pPasse = pTechnique->GetPassByIndex(0);

		// Cr�er l�organisation des sommets pour le VS de notre effet
		D3DX11_PASS_SHADER_DESC effectVSDesc; 
		pPasse->GetVertexShaderDesc(&effectVSDesc);

		D3DX11_EFFECT_SHADER_DESC effectVSDesc2; 
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

		const void* vsCodePtr = effectVSDesc2.pBytecode; 
		unsigned vsCodeLen = effectVSDesc2.BytecodeLength;
	
		pVertexLayout = NULL;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout, CSommetBloc::numElements, vsCodePtr, vsCodeLen, &pVertexLayout), DXE_CREATIONLAYOUT);
	
		// Initialisation des param�tres de sampling de la texture 
		D3D11_SAMPLER_DESC samplerDesc;
	
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; 
		samplerDesc.MipLODBias = 0.0f; 
		samplerDesc.MaxAnisotropy = 4;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; 
		samplerDesc.BorderColor[0] = 0; 
		samplerDesc.BorderColor[1] = 0; 
		samplerDesc.BorderColor[2] = 0; 
		samplerDesc.BorderColor[3] = 0; 
		samplerDesc.MinLOD = 0; 
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		//Cr�ation de l'�tat de sampling 
		pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
	}

	void CBlocEffet1::Anime(float tempsEcoule) {
		rotation = rotation + ((XM_PI * 2.0f) / 3.0f * tempsEcoule); // modifier la matrice de l�objet bloc 
		matWorld = XMMatrixRotationX(rotation);
	}

	void CBlocEffet1::setTexture(CTexture* pTexture) {
		pTextureD3D = pTexture->GetD3DTexture();
	}

	CBlocEffet1::~CBlocEffet1() {
		DXRelacher(pConstantBuffer);
		DXRelacher(pVertexLayout);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}
}