#pragma once

#include <cmath>
#include <numbers>

struct vec3
{
	float x, y, z;

	vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) { }

	const vec3 operator+(const vec3 &other) const noexcept
	{
		return vec3(x + other.x, y + other.y, z + other.z);
	}

	const vec3 operator-(const vec3 &other) const noexcept
	{
		return vec3(x - other.x, y - other.y, z - other.z);
	}

	const vec3 operator*(float multiply) const noexcept
	{
		return vec3(x * multiply, y * multiply, z * multiply);
	}

	const vec3 operator/(float divide) const noexcept
	{
		return vec3(x / divide, y / divide, z / divide);
	}

	const vec3 toAngle() const noexcept
	{
		return vec3(std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>), std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>), 0.0f);
	}

	const bool isZero() const noexcept
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}
};

inline const vec3 CalculateAngle(const vec3& pos1, const vec3& pos2, const vec3& viewAngles)
{
	return ((pos2 - pos1).toAngle() - viewAngles);
}
