#include "glText.h"
#include <string>
#include <iostream>

void GL::Font::Build(int height, int width)
{
	this->height = height;
	this->width = width;

	hdc = wglGetCurrentDC();
	base = glGenLists(96);
	HFONT hFont = CreateFontA(-height, 0, 0, 0, FW_MEDIUM, FALSE,
		FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Hack");

	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
	wglUseFontBitmaps(hdc, 32, 96, base);
	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);

	built = true;
}

void GL::Font::Print(float x, float y, const unsigned char color[3], const char* format, ...)
{
	glColor3ub(color[0], color[1], color[2]);
	glRasterPos2f(x, y);

	char text[100];
	va_list args;

	va_start(args, format);
	vsprintf_s(text, 100, format, args);
	va_end(args);

	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

Vector2 GL::Font::centerText(float x, float y, float width, float height, float textWidth, float textHeight)
{
	Vector2 text;
	text.x = x + (width - textWidth) / 2;
	text.y = y + textHeight;
	return text;
}

float GL::Font::centerText(float x, float width, float textWidth)
{
	if (width > textWidth)
	{
		float diff = width - textWidth;
		return x + (diff / 2);
	}
	else
	{
		float diff = textWidth - width;
		return x - (diff / 2);
	}
}

float GL::Font::GetTextWidth(const char* text)
{
	if (textWidthCache.find(text) != textWidthCache.end()) {
		return textWidthCache[text];
	}

	HFONT hFont = CreateFontA(-height, 0, 0, 0, FW_MEDIUM, FALSE,
		FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, FF_DONTCARE | DEFAULT_PITCH, "Hack");

	HFONT originalFont = (HFONT)SelectObject(hdc, hFont);
	SIZE textSize;
	GetTextExtentPoint32A(hdc, text, strlen(text), &textSize);
	SelectObject(hdc, originalFont);
	DeleteObject(hFont);

	textWidthCache[text] = static_cast<float>(textSize.cx);
	return textWidthCache[text];

}