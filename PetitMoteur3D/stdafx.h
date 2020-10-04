// stdafx.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets qui sont utilisés fréquemment,
// et sont rarement modifiés
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclure les en-têtes Windows rarement utilisés
// Fichiers d'en-tête Windows :

#include <windows.h>

// Fichiers d'en-tête C RunTime
#include <cassert>
#include <cstdint>
#include <tchar.h>
#include "CImg.h"
#include <iomanip> 
#include "json.hpp"
using namespace cimg_library;
using json = nlohmann::json;

// Fichiers d'en-tête C++ RunTime
#include <exception>
#include <vector>
#include <memory>
#include <d3dcompiler.h>
using namespace std;

// TODO : faites référence ici aux en-têtes supplémentaires nécessaires au programme
#define _XM_NO_INTRINSICS_
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
