#include "stdafx.h"
#include "BlocMesh.h"
#include <string>	

#include "moteurWindows.h" 
#include "util.h" 
#include "resource.h"

using namespace UtilitairesDX;
namespace PM3D {

	// Definir l�organisation de notre sommet 
	D3D11_INPUT_ELEMENT_DESC CObjetMesh::CSommetMesh::layout[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT CObjetMesh::CSommetMesh::numElements;



	CObjetMesh::CObjetMesh(IChargeur& chargeur, CDispositifD3D11* _pDispositif)
		: pDispositif(_pDispositif) // prendre en note le dispositif 
	{
		// Placer l'objet sur la carte graphique 
		TransfertObjet(chargeur);
		// Initialisation de l'effet 
		InitEffet();
		matWorld = XMMatrixIdentity();
		rotation = 0.0f;
	}


	CObjetMesh::~CObjetMesh()
	{
		DXRelacher(pConstantBuffer);
		DXRelacher(pSampleState);
		DXRelacher(pEffet);
		DXRelacher(pVertexLayout);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);
	}


	void CObjetMesh::TransfertObjet(IChargeur& chargeur)
	{
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();
		// 1. SOMMETS a) Cr�ations des sommets dans un tableau temporaire 
		{
			const size_t nombreSommets = chargeur.GetNombreSommets();
			std::unique_ptr<CSommetMesh[]> ts(new CSommetMesh[nombreSommets]);
			for (uint32_t i = 0; i < nombreSommets; ++i)
			{
				ts[i].position = chargeur.GetPosition(i);
				ts[i].normal = chargeur.GetNormale(i);
				ts[i].coordTex = chargeur.GetCoordTex(i);
			}
			// 1. SOMMETS b) Cr�ation du vertex buffer et copie des sommets 
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = static_cast<uint32_t>(sizeof(CSommetMesh) * nombreSommets);
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = ts.get();
			pVertexBuffer = nullptr;
			DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);
		}
		// 2. INDEX - Cr�ation de l�index buffer et copie des indices 
		// Les indices �tant habituellement des entiers, j�ai 
		// pris directement ceux du chargeur, mais attention au 
		// format si vous avez autre chose que DXGI_FORMAT_R32_UINT 
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = static_cast<uint32_t>(sizeof(uint32_t) * chargeur.GetNombreIndex());
			2
				bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = chargeur.GetIndexData();
			pIndexBuffer = nullptr;
			DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
				DXE_CREATIONINDEXBUFFER);
		}
		3
			// 3. Les sous-objets 
			NombreSubset = chargeur.GetNombreSubset();
		// D�but de chaque sous-objet et un pour la fin 
		SubsetIndex.reserve(NombreSubset);
		chargeur.CopieSubsetIndex(SubsetIndex);
		// 4. MATERIAUX 
		4
			// 4a) Cr�er un mat�riau de d�faut en index 0 
			// Vous pourriez changer les valeurs, j�ai conserv� 
			// celles du constructeur 
			Material.reserve(chargeur.GetNombreMaterial() + 1);
		Material.emplace_back(CMaterial());
		// 4b) Copie des mat�riaux dans la version locale 
		for (int32_t i = 0; i < chargeur.GetNombreMaterial(); ++i)
		{
			CMaterial mat;
			chargeur.GetMaterial(i, mat.NomFichierTexture,
				mat.NomMateriau,
				5
				mat.Ambient, 217

				mat.Diffuse,
				mat.Specular,
				mat.Puissance);
			Material.push_back(mat);
		}
		// 4c) Trouver l�index du materiau pour chaque sous-ensemble 
		SubsetMaterialIndex.reserve(chargeur.GetNombreSubset());
		for (int32_t i = 0; i < chargeur.GetNombreSubset(); ++i)
		{
			int32_t index;
			for (index = 0; index < Material.size(); ++index)
			{
				if (Material[index].NomMateriau == chargeur.GetMaterialName(i)) break;
			}
			if (index >= Material.size()) index = 0; // valeur de d�faut 
			SubsetMaterialIndex.push_back(index);
		}
		// 4d) Chargement des textures 
		CGestionnaireDeTextures& TexturesManager = CMoteurWindows::GetInstance().GetTextureManager();
		for (uint32_t i = 0; i < Material.size(); ++i)
		{
			if (Material[i].NomFichierTexture.length() > 0)
			{
				const std::wstring ws(Material[i].NomFichierTexture.begin(), Material[i].NomFichierTexture.end());
				Material[i].pTextureD3D = TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();
			}
		}
	}

	void CObjetMesh::InitEffet()
	{
		// Compilation et chargement du vertex shader 
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();
		// Cr�ation d�un tampon pour les constantes du VS 
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		DXEssayer(pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer));
		// Pour l�effet 
		ID3DBlob* pFXBlob = nullptr;
		DXEssayer(D3DCompileFromFile(L"MiniPhong.fx", 0, 0, 0,
			"fx_5_0", 0, 0, &pFXBlob, 0),
			DXE_ERREURCREATION_FX);
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
		const uint32_t vsCodeLen = effectVSDesc2.BytecodeLength;
		pVertexLayout = nullptr;
		CSommetMesh::numElements = ARRAYSIZE(CSommetMesh::layout);
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetMesh::layout,
			CSommetMesh::numElements,
			vsCodePtr,
			vsCodeLen,
			&pVertexLayout),
			DXE_CREATIONLAYOUT);
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
		// Cr�ation de l��tat de sampling 
		pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
	}

	void CObjetMesh::Draw()
	{
		// Obtenir le contexte 
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();
		// Choisir la topologie des primitives 
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// input layout des sommets 
		pImmediateContext->IASetInputLayout(pVertexLayout);
		// Index buffer 
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		// Vertex buffer 
		UINT stride = sizeof(CSommetMesh);
		UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		// Initialiser et s�lectionner les � constantes � de l�effet 
		ShadersParams sp;
		XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();
		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);
		sp.vLumiere = XMVectorSet(-10.0f, 10.0f, -15.0f, 1.0f);
		sp.vCamera = XMVectorSet(0.0f, 3.0f, -5.0f, 1.0f);
		sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vSEcl = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);
		// Le sampler state 
		ID3DX11EffectSamplerVariable * variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);
		// Dessiner les sous-objets non-transparents 
		for (int i = 0; i < NombreSubmesh; ++i)
		{
			int indexStart = SubmeshIndex[i];
			int indexDrawAmount = SubmeshIndex[i + 1] - SubmeshIndex[i];
			if (indexDrawAmount)
			{
				sp.vAMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Ambient);
				sp.vDMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Diffuse);
				sp.vSMat = XMLoadFloat4(&Material[SubmeshMaterialIndex[i]].Specular);
				sp.puissance = Material[SubmeshMaterialIndex[i]].Puissance;
				// Activation de la texture ou non 
				if (Material[SubmeshMaterialIndex[i]].pTextureD3D != nullptr)
				{
					ID3DX11EffectShaderResourceVariable* variableTexture;
					variableTexture =
						pEffet->GetVariableByName("textureEntree")->AsShaderResource();
					variableTexture->SetResource(Material[SubmeshMaterialIndex[i]].pTextureD3D);
					sp.bTex = 1;
				}
				else
				{
					sp.bTex = 0;
				}
				// IMPORTANT pour ajuster les param. 
				pPasse->Apply(0, pImmediateContext);
				// Nous n�avons qu�un seul CBuffer 
				ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");
				pCB->SetConstantBuffer(pConstantBuffer);
				pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);
				pImmediateContext->DrawIndexed(indexDrawAmount, indexStart, 0);
			}
		}
	}

	void CObjetMesh::Anime(float tempsEcoule)
	{
		rotation = rotation + ((XM_PI * 2.0f) / 10.0f * tempsEcoule);
		// modifier la matrice de l�objet bloc 
		matWorld = XMMatrixRotationZ(rotation);
	}

}