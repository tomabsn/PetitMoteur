#include "stdafx.h" 
#include "strsafe.h" 
#include "dispositifd3d11.h" 
#include "Texture.h" 
#include "resource.h" 
#include "util.h" 
#include "DDSTextureLoader.h"

namespace PM3D {
	CTexture::CTexture(const std::wstring& filename, CDispositifD3D11* pDispositif) : m_Filename(filename), m_Texture(nullptr) {
		ID3D11Device* pDevice = pDispositif->GetD3DDevice();
		// Charger la texture en ressource 
		DXEssayer(CreateDDSTextureFromFile(pDevice, m_Filename.c_str(), nullptr, &m_Texture), DXE_FICHIERTEXTUREINTROUVABLE);
	}

	CTexture::~CTexture() { DXRelacher(m_Texture); }
}
