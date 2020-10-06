#include "stdafx.h"
#include "Terrain.h"
#include "MoteurWindows.h"
#include "util.h"
#include "resource.h"
#include <fstream>
using namespace std;

namespace PM3D {

	CTerrain::CTerrain(const float dxy,const float dz, const char* image)  {
		const CImg<float> img(image);
		width = img.width();
		height = img.height();



		nbrSommets = width * height;
		nbrPolygones = (width - 1) * (height - 1) * 2;
		sommets = new CSommetTerrain[nbrSommets];

		for (int y = 0; y != height; ++y) {
			for (int x = 0; x != width; ++x) {
				sommets[width * y + x] = CSommetTerrain(XMFLOAT3(x / dxy, y / dxy, img.atXY(x, y) / dz));
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
				XMVECTOR n6;
				XMVECTOR n8;

				XMVECTOR v1;
				XMVECTOR v2;
				XMVECTOR v3;
				XMVECTOR v4;
				XMVECTOR v6;
				XMVECTOR v8;

				XMFLOAT3 p0 = ObtenirPosition(x,y);
				XMFLOAT3 p1;
				XMFLOAT3 p2;
				XMFLOAT3 p3;
				XMFLOAT3 p4;
				XMFLOAT3 p6;
				XMFLOAT3 p8;

				v1 = v2 = v3 = v4 = v6 = v8 = XMVectorSet(0, 0, 0, 0);
				n1 = n2 = n3 = n4 = n6 = n8 = XMVectorSet(0, 0, -1, 0);
				
				//calcul de vecteur !
				if (y < height - 1) {
					p1 = ObtenirPosition(x, y + 1);
					v1 = XMVectorSet(abs(p1.x - p0.x), abs(p1.y - p0.y), abs(p1.z - p0.z), 0);
				}
				if (x < width - 1) {
					p2 = ObtenirPosition(x + 1, y);
					v2 = XMVectorSet(abs(p2.x - p0.x), abs(p2.y - p0.y), abs(p2.z - p0.z), 0);
				}
				if (y > 0) {
					p3 = ObtenirPosition(x, y - 1);
					v3 = XMVectorSet(abs(p3.x - p0.x), abs(p3.y - p0.y), abs(p3.z - p0.z), 0);
				}
				if (x > 0) {
					p4 = ObtenirPosition(x - 1, y);
					v4 = XMVectorSet(abs(p4.x - p0.x), abs(p4.y - p0.y), abs(p4.z - p0.z), 0);
				}
				if (y < height - 1 && x < width - 1) {
					p6 = ObtenirPosition(x + 1, y + 1);
					v6 = XMVectorSet(abs(p6.x - p0.x), abs(p6.y - p0.y), abs(p6.z - p0.z), 0);
				}
				if (y > 0 && x > 0) {
					p8 = ObtenirPosition(x - 1, y - 1);
					v8 = XMVectorSet(abs(p8.x - p0.x), abs(p8.y - p0.y), abs(p8.z - p0.z), 0);
				}

				//calcul de produit scalaire !
				if (y < height - 1 && x < width - 1) {
					n1 = XMVector3Cross(v1, v6);
					n6 = XMVector3Cross(v6, v2);
				}
				if (y > 0 && x < width - 1){
					n2 = XMVector3Cross(v2, v3);
				}
				if (y > 0 && x > 0) { 
					n3 = XMVector3Cross(v3, v8);
					n8 = XMVector3Cross(v8, v4);
				}
				if (y < height - 1 && x > 0) {
					n4 = XMVector3Cross(v4, v1);
				}

				n1 = n2 + n3 + n3 + n6 + n8 +n1;

				//n1 = XMVector3Normalize(n1);
				XMFLOAT3 resultat;
				XMStoreFloat3(&resultat, n1);

				sommets[width * y + x].m_Normal = resultat;
			}
		}
	}

	void CTerrain::ConstruireIndex() {
		pIndices = new unsigned int[nbrPolygones * 3];
		int k = 0;
		for (int y = 0; y != height-1; ++y) {
			for (int x = 0; x != width-1; ++x) {
				pIndices[k++] = y * width + x;
				pIndices[k++] = (y + 1) * width + (x + 1);
				pIndices[k++] = y * width + (x + 1);
				pIndices[k++] = y * width + x;
				pIndices[k++] = (y + 1) * width + x;
				pIndices[k++] = (y + 1) * width + (x + 1);
			}
		}
	}

	void CTerrain::Sauver(char* name) {
		ofstream file;
		file.open(name);
		if (file.is_open()) {
			for (int i = 0; i != nbrSommets; ++i) {
				file << "v " << sommets[i].m_Position.x << " " << sommets[i].m_Position.y << " " << sommets[i].m_Position.z << "\n";
			}
			
			for (int i = 0; i != nbrSommets; ++i) {
				file << "vn " << sommets[i].m_Normal.x << " " << sommets[i].m_Normal.y << " " << sommets[i].m_Normal.z << "\n";
			}
			for (int j = 0; j < nbrPolygones*3 - 3; j += 3) {
				//f v1 / vt1 / vn1 v2 / vt2 / vn2 v3 / vt3 / vn3
				file << "f " << pIndices[j] + 1 << "//" << pIndices[j] + 1 << " ";
				file << pIndices[j + 1] +1<< "//" << pIndices[j+1] + 1 << " ";
				file << pIndices[j + 2] +1<< "//" << pIndices[j+2] + 1 << " ";
				file << '\n';
			}
		}
		file.close();
		/*json j1;
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

		o << setw(4) << j1 << std::endl;*/
	}

	CTerrain::~CTerrain(){

	}

	XMFLOAT3 CTerrain::ObtenirPosition(int x, int y) {
		return sommets[height * y + x].m_Position;
	}
}