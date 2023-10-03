#pragma once
#include <memory>

struct Vector3
{
	float x, y, z;

	Vector3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f)
		: x(x), y(y), z(z) {};

	const Vector3& operator-(const Vector3& other) noexcept
	{
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}

	const bool IsZero() const noexcept
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}
};


struct Rect
{
	float x, y, w, h;
};


struct Vector2
{
	float x, y;
};

struct IntVector3
{
	int32_t z; //0x0000
	int32_t x; //0x0004
	int32_t y; //0x0008
};


struct Collider
{
	float height; //0x0000
	float crouch; //0x0004
	float stand; //0x0008
	float width; //0x000C
};

Vector3 CalculateAngle(const Vector3& localPosition, const Vector3& enemyPosition);
float GetDistance(const Vector3& localPosition, const Vector3& enemyPosition);
bool WorldToScreen(const Vector3& position, Vector2& screen, float viewMatrix[16], int windowWidth, int windowHeight);