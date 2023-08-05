#pragma once

namespace Settings
{
	inline bool AIMBOT;
	inline bool GLOW;
	inline bool BHOP;
	inline bool RADAR;
	inline bool NOFLASH;

	inline float SMOOTHNESS;

	inline bool SKINCHANGER;

	inline float spottedGlowColor[4] = { 0.f, 1.f, 0.f, 1.f };
	inline float notSpottedGlowColor[4] = { 1.f, 0.f, 0.f, 1.f };
}