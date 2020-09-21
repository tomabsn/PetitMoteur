#include "stdafx.h"
#include "SommetBloc.h"

// Definir l’organisation de notre sommet 
D3D11_INPUT_ELEMENT_DESC PM3D::CSommetBloc::layout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

UINT PM3D::CSommetBloc::numElements = ARRAYSIZE(CSommetBloc::layout);

PM3D::CSommetBloc::CSommetBloc(const XMFLOAT3& _position, const XMFLOAT3& _normal) {
	m_Position = _position; m_Normal = _normal;
}