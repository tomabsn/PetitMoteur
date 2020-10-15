#include "stdafx.h"
#include "Bloc.h"
#include "sommetbloc.h"
#include "MoteurWindows.h"
#include "util.h"
#include "resource.h"

//structure pour communiquer entre VS et PS
//tampon de constante 
struct ShadersParams {
	XMMATRIX matWorldViewProj; // la matrice totale 
	XMMATRIX matWorld; // matrice de transformation dans le monde 
	XMVECTOR vLumiere; // la position de la source d’éclairage (Point) 
	XMVECTOR vCamera; // la position de la caméra 
	XMVECTOR vAEcl; // la valeur ambiante de l’éclairage 
	XMVECTOR vAMat; // la valeur ambiante du matériau 
	XMVECTOR vDEcl; // la valeur diffuse de l’éclairage 
	XMVECTOR vDMat; // la valeur diffuse du matériau 
};

namespace PM3D {

	CBloc::CBloc(const float dx, const float dy, const float dz, CDispositifD3D11* pDispositif_) : pDispositif(pDispositif_) {
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
		XMFLOAT3 n1(0.0f, 0.0f, 1.0f); // arrière
		XMFLOAT3 n2(0.0f, -1.0f, 0.0f); // dessous
		XMFLOAT3 n3(0.0f, 1.0f, 0.0f); // dessus
		XMFLOAT3 n4(-1.0f, 0.0f, 0.0f); // face gauche
		XMFLOAT3 n5(1.0f, 0.0f, 0.0f); // face droite


		CSommetBloc sommets[24];

		// Le devant du bloc 
		sommets[0] = CSommetBloc(point[0], n0);
		sommets[1] = CSommetBloc(point[1], n0);
		sommets[2] = CSommetBloc(point[2], n0);
		sommets[3] = CSommetBloc(point[3], n0);

		// L’arrière du bloc 
		sommets[4] = CSommetBloc(point[4], n1);
		sommets[5] = CSommetBloc(point[5], n1);
		sommets[6] = CSommetBloc(point[6], n1);
		sommets[7] = CSommetBloc(point[7], n1);

		// Le dessous du bloc 
		sommets[8] = CSommetBloc(point[3], n2);
		sommets[9] = CSommetBloc(point[2], n2);
		sommets[10] = CSommetBloc(point[6], n2);
		sommets[11] = CSommetBloc(point[5], n2);

		// Le dessus du bloc 
		sommets[12] = CSommetBloc(point[0], n3);
		sommets[13] = CSommetBloc(point[4], n3);
		sommets[14] = CSommetBloc(point[7], n3);
		sommets[15] = CSommetBloc(point[1], n3);

		// La face gauche 
		sommets[16] = CSommetBloc(point[0], n4);
		sommets[17] = CSommetBloc(point[3], n4);
		sommets[18] = CSommetBloc(point[5], n4);
		sommets[19] = CSommetBloc(point[4], n4);

		// La face droite 
		sommets[20] = CSommetBloc(point[1], n5);
		sommets[21] = CSommetBloc(point[7], n5);
		sommets[22] = CSommetBloc(point[6], n5);
		sommets[23] = CSommetBloc(point[2], n5);

		// Création du vertex buffer et copie des sommets 
		ID3D11Device* pD3DDevice = pDispositif-> GetD3DDevice(); 
		D3D11_BUFFER_DESC bd; ZeroMemory(&bd, sizeof(bd)); 
		bd. Usage = D3D11_USAGE_IMMUTABLE; 
		bd. ByteWidth = sizeof(sommets); 
		bd. BindFlags = D3D11_BIND_VERTEX_BUFFER; 
		bd. CPUAccessFlags = 0; D3D11_SUBRESOURCE_DATA InitData; 
		ZeroMemory(&InitData, sizeof(InitData)); 
		InitData.pSysMem = sommets; 
		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);


		// Création de l’index buffer et copie des indices 
		ZeroMemory(&bd, sizeof(bd)); 
		bd. Usage = D3D11_USAGE_IMMUTABLE; 
		bd. ByteWidth = sizeof(index_bloc); 
		bd. BindFlags = D3D11_BIND_INDEX_BUFFER; 
		bd. CPUAccessFlags = 0; 
		ZeroMemory(&InitData, sizeof(InitData)); 
		InitData.pSysMem = index_bloc; 
		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer), DXE_CREATIONINDEXBUFFER);

		matWorld = XMMatrixIdentity();

		rotation = 0.0f;

		InitShaders();

	}

	void CBloc::Draw() {
		// Obtenir le contexte 
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext(); 
		// Choisir la topologie des primitives 
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); 
		// Source des sommets 
		const UINT stride = sizeof(CSommetBloc); 
		const UINT offset = 0; 
		pImmediateContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset ); 
		// Source des index 
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0); 
		// input layout des sommets 
		pImmediateContext->IASetInputLayout(pVertexLayout); 
		// Activer le VS 
		pImmediateContext->VSSetShader(pVertexShader, nullptr, 0); 

		// Initialiser et sélectionner les « constantes » du VS 
		ShadersParams sp; 
		XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj(); 

		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj); 
		sp.matWorld = XMMatrixTranspose(matWorld); 
		sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -10.0f, 1.0f); 
		sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f); 
		sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f); 
		sp.vAMat = XMVectorSet(0.5f, 0.0f, 0.0f, 1.0f); //changer la couleur de notre cube 
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f); 
		sp.vDMat = XMVectorSet(0.5f, 0.0f, 0.0f, 1.0f); //changer la couleur de notre cube 

		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0); 
		
		//recupere les param de sortie du VS et les places dans pConstantBuffer
		pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer); 
		
		// Pas de Geometry Shader 
		pImmediateContext->GSSetShader(nullptr, nullptr,0); 
		
		// Activer le PS 
		pImmediateContext->PSSetShader( pPixelShader, nullptr, 0 ); 
		pImmediateContext->PSSetConstantBuffers( 0, 1, &pConstantBuffer );

		// **** Rendu de l’objet 
		pImmediateContext->DrawIndexed(ARRAYSIZE(index_bloc), 0, 0);
	}

	void CBloc::InitShaders() { // Compilation et chargement du vertex shader 
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice(); 
		ID3DBlob* pVSBlob = nullptr; 
		DXEssayer(D3DCompileFromFile(L"MiniPhong.vhl", nullptr, nullptr, "MiniPhongVS", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVSBlob, nullptr), DXE_FICHIER_VS);
		DXEssayer(pD3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &pVertexShader), DXE_CREATION_VS);

		// Créer l’organisation des sommets 
		pVertexLayout = nullptr; 
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout, CSommetBloc::numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pVertexLayout), DXE_CREATIONLAYOUT); 
		pVSBlob->Release(); // On n’a plus besoin du blob


		// Création d’un tampon pour les constantes du VS 
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd)); 
		bd. Usage = D3D11_USAGE_DEFAULT; 
		bd.ByteWidth = sizeof(ShadersParams);
		bd. BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
		bd. CPUAccessFlags = 0; pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		// Compilation et chargement du pixel shader 
		ID3DBlob* pPSBlob = nullptr; 
		DXEssayer(D3DCompileFromFile(L"MiniPhong.phl", nullptr, nullptr, "MiniPhongPS", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, nullptr), DXE_FICHIER_PS); 
		DXEssayer(pD3DDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pPixelShader), DXE_CREATION_PS); 
		pPSBlob->Release(); // On n’a plus besoin du blob
	}

	void CBloc::Anime(float tempsEcoule) {
		rotation = rotation + ((XM_PI * 2.0f) / 3.0f * tempsEcoule); // modifier la matrice de l’objet bloc 
		matWorld = XMMatrixRotationX(rotation); 
	}

	CBloc::~CBloc() {
		DXRelacher(pPixelShader); 
		DXRelacher(pConstantBuffer); 
		DXRelacher(pVertexLayout); 
		DXRelacher(pVertexShader); 
		DXRelacher(pIndexBuffer); 
		DXRelacher(pVertexBuffer);
	}
}