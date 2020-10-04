// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclure les en-t�tes Windows rarement utilis�s
// Fichiers d'en-t�te Windows�:

#include <windows.h>

// Fichiers d'en-t�te C RunTime
#include <cassert>
#include <cstdint>
#include <tchar.h>
#include "CImg.h"
#include <iomanip> 
#include "json.hpp"
using namespace cimg_library;
using json = nlohmann::json;

// Fichiers d'en-t�te C++ RunTime
#include <exception>
#include <vector>
#include <memory>
#include <d3dcompiler.h>
using namespace std;

// TODO�: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
#define _XM_NO_INTRINSICS_
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
