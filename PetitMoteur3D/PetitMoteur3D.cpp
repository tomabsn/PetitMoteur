// PetitMoteur3D.cpp : définit le point d'entrée pour l'application.
//

#include "stdafx.h"
#include "PetitMoteur3D.h"
#include "MoteurWindows.h"
#include "Terrain.h"

using namespace PM3D;

int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	// Pour ne pas avoir d'avertissement
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	try
	{
		// Création de l'objet Moteur
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		// Spécifiques à une application Windows
		rMoteur.SetWindowsAppInstance(hInstance);

		CTerrain terrain(1.0f,1.0f,"hills.bmp");
		terrain.CalculerNormale();
		terrain.ConstruireIndex();
		//terrain.Sauver("save.json");
		// Initialisation du moteur
		rMoteur.Initialisations();

		// Boucle d'application
		rMoteur.Run();

		return (int)1;
	}

	catch (const std::exception& E)
	{
		const int BufferSize = 128;
		wchar_t message[BufferSize];

		size_t numCharacterConverted;
		mbstowcs_s(&numCharacterConverted, message, E.what(), BufferSize - 1);
		::MessageBox(nullptr, message, L"Erreur", MB_ICONWARNING);

		return (int)99;
	}

	catch (int codeErreur)
	{
		wchar_t szErrMsg[MAX_LOADSTRING];	// Un message d'erreur selon le code

		::LoadString(hInstance, codeErreur, szErrMsg, MAX_LOADSTRING);
		::MessageBox(nullptr, szErrMsg, L"Erreur", MB_ICONWARNING);

		return (int)99; // POURQUOI 99???
	}

}
