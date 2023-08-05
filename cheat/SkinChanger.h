#pragma once

#include "GUI.h"
#include "Settings.h"
#include "offsets.h"
#include "skins.h"
#include "Memory.h"

#include <thread>
#include <array>

namespace SKINCHANGER
{
	void Run(const Memory& memory, const uintptr_t& client, const uintptr_t& engine)
	{
		while (GUI::isRunning)
		{
			if (!Settings::SKINCHANGER)
				continue;

			const auto localPlayer = memory.read<uintptr_t>(client + offsets::dwLocalPlayer);
			const auto weapons = memory.read<std::array<unsigned long, 8>>(localPlayer + offsets::m_hMyWeapons);

			for (int i = 0; i < weapons.size(); i++)
			{
				const auto weapon = memory.read<uintptr_t>((client + offsets::dwEntityList + (weapons[i] & 0xFFF) * 0x10) - 0x10);

				if (!weapon)
					continue;

				const auto weaponIndex = memory.read<short>(weapon + offsets::m_iItemDefinitionIndex);
				const auto selectedSkin = SKINS::getById(weaponIndex).selectedSkin;

				if (selectedSkin.id == -1)
					continue;

				if (memory.read<int>(weapon + offsets::m_nFallbackPaintKit) == selectedSkin.id)
					continue;

				memory.write<int>(memory.read<int>(engine + offsets::dwClientState) + 0x174, -1);

				memory.write<int>(weapon + offsets::m_iItemIDHigh, -1);

				memory.write<int>(weapon + offsets::m_nFallbackPaintKit, selectedSkin.id);
				memory.write<float>(weapon + offsets::m_flFallbackWear, 0.1f);

				memory.write<int>(memory.read<int>(engine + offsets::dwClientState) + offsets::clientstate_delta_ticks, -1);
			}
		}
	}
};
