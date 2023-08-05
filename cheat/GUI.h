#pragma once

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

#include "skins.h"
#include "Settings.h"

#include <d3d9.h>

namespace GUI
{
	const int WIDTH = 500;
	const int HEIGHT = 300;

	inline bool isRunning = true;

	inline HWND window = nullptr;
	inline WNDCLASSEXA windowClass = { };
	
	inline POINTS position;

	inline int activeTab = 0;

	inline PDIRECT3D9 d3d9 = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS params = { };

	void CreateHWindow(const char* windowName, const char* className);
	void DestroyHWindow();
	
	bool CreateDevice();
	void ResetDevice();
	void DestroyDevice();

	void CreateImGui();
	void DestroyImGui();

	void BeginRender();
	void EndRender();
	void Render();
};
