#pragma once

#include "Memory.h"
#include "offsets.h"
#include "vec3.h"
#include "GUI.h"
#include "Settings.h"

#include <thread>
#include <map>
#include <vector>
#include <array>

struct color
{
	float r, g, b, a;
	color(float* v) { r = v[0], g = v[1], b = v[2], a = v[3]; }
};

class Hacks
{
private:
	void Run();

	std::pair<float, vec3> Aimbot(const int& player, const vec3& eyePosition, const vec3& viewAngles, const vec3& aimPunch);
	void Glow(const int& player, const uintptr_t& glowManager);
	void Bunnyhop(const uintptr_t& localPlayer);
	void Radar(const int& player);
	void NoFlash(const uintptr_t& localPlayer);

public:
	const Memory& memory;
	const uintptr_t& client;
	const uintptr_t& engine;

	Hacks(const Memory& _memory, const uintptr_t& _client, const uintptr_t& _engine);

	std::thread spawnThread();
};