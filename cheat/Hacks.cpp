#include "Hacks.h"
 
Hacks::Hacks(const Memory& _memory, const uintptr_t& _client, const uintptr_t& _engine) : memory(_memory), client(_client), engine(_engine)
{
	Settings::SMOOTHNESS = 3.0f;
}

std::thread Hacks::spawnThread()
{
	return std::thread([this] {
		Run();
	});
}

void Hacks::Run()
{
	while (GUI::isRunning)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));

		const auto localPlayer = memory.read<uintptr_t>(client + offsets::dwLocalPlayer);
	
		if (!localPlayer)
			continue;

		const auto localTeam = memory.read<int>(localPlayer + offsets::m_iTeamNum);

		float FOV = 9.0f;
		vec3 ANGLE = { };


		const auto glowManager = memory.read<uintptr_t>(client + offsets::dwGlowObjectManager);

		const auto eyePosition = memory.read<vec3>(localPlayer + offsets::m_vecOrigin) + memory.read<vec3>(localPlayer + offsets::m_vecViewOffset);
		const auto clientState = memory.read<uintptr_t>(engine + offsets::dwClientState);
		const auto viewAngles = memory.read<vec3>(clientState + offsets::dwClientState_ViewAngles);
		const auto aimPunch = memory.read<vec3>(localPlayer + offsets::m_aimPunchAngle) * 2;


		for (int i = 0; i < 64; i++)
		{
			const auto player = memory.read<int>(client + offsets::dwEntityList + i * 0x10);

			if (!player)
				continue;

			if (memory.read<int>(player + offsets::m_iTeamNum) == localTeam)
				continue;

			if (!memory.read<int>(player + offsets::m_iHealth))
				continue;


			// ======= AIMBOT =======
			if (Settings::AIMBOT && GetAsyncKeyState(VK_LBUTTON) && !memory.read<bool>(player + offsets::m_bDormant) && memory.read<bool>(player + offsets::m_bSpottedByMask))
			{
				auto res = Aimbot(player, eyePosition, viewAngles, aimPunch);
				if (res.first < FOV)
					FOV = res.first, ANGLE = res.second;
			}

			// ======= GLOW =======
			if (Settings::GLOW && glowManager)
				Glow(player, glowManager);

			// ======= RADAR =======
			if (Settings::RADAR)
				Radar(player);
		}

		// ======= BUNNYHOP =======
		if (Settings::BHOP && GetAsyncKeyState(VK_SPACE))
			Bunnyhop(localPlayer);

		// ======= NO FLASHBANG =======
		if (Settings::NOFLASH)
			NoFlash(localPlayer);


		if(!ANGLE.isZero())
			memory.write<vec3>(clientState + offsets::dwClientState_ViewAngles, viewAngles + ANGLE / Settings::SMOOTHNESS);
	}
}

std::pair<float, vec3> Hacks::Aimbot(const int& player, const vec3& eyePosition, const vec3& viewAngles, const vec3& aimPunch)
{
	const auto boneMatrix = memory.read<uintptr_t>(player + offsets::m_dwBoneMatrix);

	const vec3 playerHeadPosition(
		memory.read<float>(boneMatrix + 0x30 * 8 + 0x0C),
		memory.read<float>(boneMatrix + 0x30 * 8 + 0x1C),
		memory.read<float>(boneMatrix + 0x30 * 8 + 0x2C)
	);

	const vec3 angle = CalculateAngle(eyePosition, playerHeadPosition, viewAngles + aimPunch);

	const auto fov = std::hypot(angle.x, angle.y);

	return { fov, angle };
}

void Hacks::Glow(const int& player, const uintptr_t& glowManager)
{
	const auto glowIndex = memory.read<int>(player + offsets::m_iGlowIndex);

	if (memory.read<bool>(player + offsets::m_bSpottedByMask))
		memory.write(glowManager + (glowIndex * 0x38) + 0x8, color(Settings::spottedGlowColor));
	else
		memory.write(glowManager + (glowIndex * 0x38) + 0x8, color(Settings::notSpottedGlowColor));

	memory.write(glowManager + (glowIndex * 0x38) + 0x28, true);
	memory.write(glowManager + (glowIndex * 0x38) + 0x29, false);
}

void Hacks::Bunnyhop(const uintptr_t& localPlayer)
{
	const auto health = memory.read<int>(localPlayer + offsets::m_iHealth);

	if (!health)
		return;

	const auto flags = memory.read<int>(localPlayer + offsets::m_fFlags);

	if (!flags)
		return;

	if (flags & (1 << 0))
		memory.write(client + offsets::dwForceJump, 6);
	else
		memory.write(client + offsets::dwForceJump, 4);
}

void Hacks::Radar(const int& player)
{
	memory.write(player + offsets::m_bSpotted, true);
}

void Hacks::NoFlash(const uintptr_t& localPlayer)
{
	memory.write<int>(localPlayer + offsets::m_flFlashDuration, 0);
}