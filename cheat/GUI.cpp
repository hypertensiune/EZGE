#include "GUI.h"

#include <iostream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam))
		return true;

	switch (message)
	{
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU)
				return 0;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_LBUTTONDOWN:
			GUI::position = MAKEPOINTS(lParam);
			return 0;

		case WM_MOUSEMOVE:
			if (wParam == MK_LBUTTON)
			{
				const auto points = MAKEPOINTS(lParam);
				auto rect = RECT{ };

				GetWindowRect(GUI::window, &rect);

				rect.left += points.x - GUI::position.x;
				rect.top += points.y - GUI::position.y;

				if (GUI::position.x >= 0 && GUI::position.x <= GUI::WIDTH && GUI::position.y >= 0 && GUI::position.y <= 19)
					SetWindowPos(GUI::window, HWND_TOPMOST, rect.left, rect.top, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER);
			}
			return 0;
	}

	return DefWindowProc(window, message, wParam, lParam);
}

void GUI::CreateHWindow(const char* windowName, const char* className)
{
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = className;
	windowClass.hIconSm = 0;

	RegisterClassExA(&windowClass);

	window = CreateWindowA(className, windowName, WS_POPUP, 100, 100, WIDTH, HEIGHT, 0, 0, windowClass.hInstance, 0);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);

}

void GUI::DestroyHWindow()
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool GUI::CreateDevice()
{
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d9)
		return false;

	ZeroMemory(&params, sizeof(params));

	params.Windowed = TRUE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.EnableAutoDepthStencil = TRUE;
	params.AutoDepthStencilFormat = D3DFMT_D16;
	params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, &device) < 0)
		return false;
	
	return true;
}

void GUI::ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&params);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void GUI::DestroyDevice()
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}
	if (d3d9)
	{
		d3d9->Release();
		d3d9 = nullptr;
	}
}

void GUI::CreateImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = NULL;
	io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 14);

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void GUI::DestroyImGui()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GUI::BeginRender()
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void GUI::EndRender()
{
	ImGui::EndFrame();
	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

void GUI::Render()
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin("Cheat", &isRunning, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::BeginTabBar("#tabs", ImGuiTabBarFlags_None);

	if (ImGui::BeginTabItem("Cheats"))
	{
		ImGui::Dummy(ImVec2(0, 10.f));

		ImGui::Checkbox("Aimbot", &Settings::AIMBOT);
		ImGui::SameLine();
		ImGui::SliderFloat("Smoothness", &Settings::SMOOTHNESS, 1.f, 5.f, "%.1f");

		ImGui::Dummy(ImVec2(0, 10.f));

		ImGui::Checkbox("Glow", &Settings::GLOW);
		ImGui::ColorEdit4("Spotted", Settings::spottedGlowColor);
		ImGui::ColorEdit4("Not spotted", Settings::notSpottedGlowColor);

		ImGui::Dummy(ImVec2(0, 10.f));

		ImGui::Checkbox("Radar", &Settings::RADAR);
		ImGui::Checkbox("BunnyHop", &Settings::BHOP);
		ImGui::Checkbox("No flashbang", &Settings::NOFLASH);

		ImGui::EndTabItem();
	}
	if (ImGui::BeginTabItem("Skin Changer"))
	{
		ImGui::Dummy(ImVec2(0, 10.f));

		ImGui::Checkbox("Skin Changer", &Settings::SKINCHANGER);

		for (auto& w : SKINS::weapons)
		{
			if (ImGui::BeginCombo(w.name, w.selectedSkin.name))
			{
				for (auto& skin : w.availableSkins)
				{
					bool is_selected = (w.selectedSkin.id == skin.id);
					if (ImGui::Selectable(skin.name, is_selected))
						w.selectedSkin = skin;
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}

		ImGui::EndTabItem();
	}
	
	ImGui::EndTabBar();

	ImGui::End();
}