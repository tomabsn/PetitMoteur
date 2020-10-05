#pragma once
using namespace DirectX;

namespace PM3D {
	class CSommetBloc {
	public:
		CSommetBloc() = default;
		CSommetBloc(const XMFLOAT3& position, const XMFLOAT3& normal);
	public:
		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];
	private:
		XMFLOAT3 m_Position;
		XMFLOAT3 m_Normal;
	};
}