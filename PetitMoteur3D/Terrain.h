#pragma once
#include "Objet3D.h"
#include "DispositifD3D11.h"
#include "SommetTerrain.h"

namespace PM3D {
	class CTerrain {
	public:
		CTerrain(const float dxy, const float dz, const char* image);
		void CalculerNormale();
		void Sauver(char* name);
		void ConstruireIndex();
		XMFLOAT3 ObtenirPosition(int x, int y);
		~CTerrain();
	private:
		int width;
		int height;
		int nbrSommets;
		int nbrPolygones;
		CSommetTerrain* sommets;
		unsigned int* pIndices;
	};
}