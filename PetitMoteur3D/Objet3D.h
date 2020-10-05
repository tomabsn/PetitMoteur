#pragma once

using namespace DirectX;

namespace PM3D {
	class CObjet3D {
	public:
		virtual ~CObjet3D() = default;

		virtual void Draw() = 0;
		virtual void Anime(float) =0;
	};
}