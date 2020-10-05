#include "stdafx.h"
#include "Terrain.h"
#include "MoteurWindows.h"
#include "util.h"
#include "resource.h"
#include <fstream>
using namespace std;

namespace PM3D {

	CTerrain::CTerrain([[maybe_unused]]const float dxy, [[maybe_unused]] const float dz, const char* image)  {
		const CImg<float> img(image);
		width = img.width();
		height = img.height();
		nbrPolygones = (height - 1) * (width - 1) * 2;
		nbrSommets = width * height;
		sommets = new CSommetTerrain[nbrSommets];

		for (int y = 0; y != height; ++y) {
			for (int x = 0; x != width; ++x) {
				sommets[height * y + x] = CSommetTerrain(XMFLOAT3(x / dxy, y / dxy, img.atXY(x, y) / dz) , XMFLOAT3(0.0f, 0.0f, 1.0f));
			}
		}
	}

	void CTerrain::CalculerNormale() {
		for (int y = 0; y != height; ++y) {
			for (int x = 0; x != width; ++x) {
				XMVECTOR n1;
				XMVECTOR n2;
				XMVECTOR n3;
				XMVECTOR n4;

				XMVECTOR v1;
				XMVECTOR v2;
				XMVECTOR v3;
				XMVECTOR v4;

				XMFLOAT3 p0 = ObtenirPosition(x,y);
				XMFLOAT3 p1;
				XMFLOAT3 p2;
				XMFLOAT3 p3;
				XMFLOAT3 p4;

				v1 = v2 = v3 = v4 = XMVectorSet(0, 0, 0, 0);
				n1 = n2 = n3 = n4 = XMVectorSet(0, 0, -1, 0);
				if (y < height - 1) {
					p1 = ObtenirPosition(x, y + 1);
					v1 = XMVectorSet(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z, 0);
				}
				if (x < width - 1) {
					p2 = ObtenirPosition(x + 1, y);
					v2 = XMVectorSet(p2.x - p0.x, p2.y - p0.y, p2.z - p0.z, 0);
				}
				if (y > 0) {
					p3 = ObtenirPosition(x, y - 1);
					v3 = XMVectorSet(p3.x - p0.x, p3.y - p0.y, p3.z - p0.z, 0);
				}

				if (x > 0) {
					p4 = ObtenirPosition(x - 1, y);
					v4 = XMVectorSet(p4.x - p0.x, p4.y - p0.y, p4.z - p0.z, 0);
				}


				if (y < height - 1 && x < width - 1) n1 = XMVector3Cross(v2, v1);
				if (y > 0 && x < width - 1) n2 = XMVector3Cross(v3, v2);
				if (y > 0 && x > 0) n3 = XMVector3Cross(v4, v3);
				if (y < height - 1 && x > 0) n4 = XMVector3Cross(v1, v4);

				n1 = n2 + n3 + n3 + n1;

				XMFLOAT3 resultat;
				XMStoreFloat3(&resultat, n1);

				sommets[height * y + x].m_Normal = resultat;
			}
		}
	}

	void CTerrain::ConstruireIndex() {
		pIndices = new unsigned int[nbrPolygones * 3];
		int k = 0;
		for (int y = 0; y != height - 1; ++y) {
			for (int x = 0; x != width - 1; ++x) {
				pIndices[k++] = y * height + x;
				pIndices[k++] = (y + 1) * height + (x + 1);
				pIndices[k++] = y * height + (x + 1);
				pIndices[k++] = y * height + x;
				pIndices[k++] = (y + 1) * height + x;
				pIndices[k++] = (y + 1) * height + (x + 1);
			}
		}

	}

	void CTerrain::Sauver(char* name) {
		ofstream o(name);
		json j1;
		j1["index_size"] = nbrPolygones * 3;
		j1["nombre_de_sommets"] = nbrSommets;

		int k = 1;
		for (int i = 0; i != nbrPolygones * 3; i+=3) {
			j1["index"]["triangle " + to_string(k++)] = { pIndices[i], pIndices[i + 1], pIndices[i + 2] };
		}

		for (int j = 0; j != nbrSommets; ++j) {
			j1["sommet " + to_string(j)] = { 
				{ "coordonnee",{{"x", sommets[j].m_Position.x}, {"y", sommets[j].m_Position.y},{"z", sommets[j].m_Position.z} }},
				{ "normal",{{"x", sommets[j].m_Normal.x}, {"y", sommets[j].m_Normal.y},{"z", sommets[j].m_Normal.z} }}
			};
		}

		o << setw(4) << j1 << std::endl;
	}

	CTerrain::~CTerrain(){

	}

	XMFLOAT3 CTerrain::ObtenirPosition(int x, int y) {
		return sommets[height * y + x].m_Position;
	}
}