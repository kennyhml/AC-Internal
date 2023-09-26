#pragma once
#pragma comment(lib, "OpenGL32.lib")
#include <Windows.h>
#include <gl/GL.h>
#include "../tools/mem.h"
#include "../geometry.h"
#include <unordered_map>
#include <string>

namespace GL
{
	struct Font
	{
		bool built = false;
		HDC hdc = nullptr;

		unsigned int base;
		int height;
		int width;

		void Build(int height, int width);
		void Print(float x, float y, const unsigned char color[3], const char* format, ...);
		Vector2 centerText(float x, float y, float width, float height, float textWidth, float textHeight);
		float centerText(float x, float width, float textWidth);
		float GetTextWidth(const char* text);

	private:
		std::unordered_map<std::string, float> textWidthCache;

	};
}