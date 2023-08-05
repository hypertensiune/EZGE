#include "GUI.h"

#include "Hacks.h"
#include "Memory.h"
#include "SkinChanger.h"
#include "vec3.h"
#include "offsets.h"

#include <thread>

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE previousInstance, PWSTR arguments, int commandShow)
{
	GUI::CreateHWindow("CheatMenu", "CheatMenuClass");
	GUI::CreateDevice();
	GUI::CreateImGui();

	Memory memory("csgo.exe");

	const auto client = memory.GetModuleAddressByName("client.dll");
	const auto engine = memory.GetModuleAddressByName("engine.dll");

	Hacks hacks(memory, client, engine);

	auto thread1 = hacks.spawnThread();
	thread1.detach();

	std::thread(SKINCHANGER::Run, memory, client, engine).detach();

	while (GUI::isRunning)
	{
		GUI::BeginRender();
		GUI::Render();
		GUI::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	GUI::DestroyImGui();
	GUI::DestroyDevice();
	GUI::DestroyHWindow();

	return 0;
}
