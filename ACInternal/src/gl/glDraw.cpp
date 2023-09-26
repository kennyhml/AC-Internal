#include "glDraw.h"

void GL::SetupOrtho()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glViewport(0, 0, viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, viewport[2], viewport[3], 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
}

void GL::RestoreGL()
{
	glDisable(GL_DEPTH_TEST);
	glPopMatrix();
	glPopAttrib();
}

void GL::DrawFilledRect(Rect box, const GLubyte color[3])
{
	GL::DrawFilledRect(box.x, box.y, box.w, box.h, color);
}

void GL::DrawFilledRect(float x, float y, float width, float height, const GLubyte color[3])
{
	glColor3ub(color[0], color[1], color[2]);
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

void GL::DrawOutline(Rect box, float lineWidth, const GLubyte color[3])
{
	GL::DrawOutline(box.x, box.y, box.w, box.h, lineWidth, color);
}


void GL::DrawOutline(float x, float y, float width, float height, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glBegin(GL_LINE_STRIP);
	glColor3ub(color[0], color[1], color[2]);

	glVertex2f(x - 0.5f, y - 0.5f);
	glVertex2f(x + width + 0.5f, y - 0.5f);
	glVertex2f(x + width + 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y + height + 0.5f);
	glVertex2f(x - 0.5f, y - 0.5f);
	glEnd();
}

void GL::DrawLine(float x, float y, float x2, float y2, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	glColor3ub(color[0], color[1], color[2]);

	glVertex2f(x, y);
	glVertex2f(x2, y2);

	glEnd();
}

void GL::DrawCircle(float centerX, float centerY, float radius, int numSegments, float lineWidth, const GLubyte color[3])
{
	glLineWidth(lineWidth);
	glColor3ub(color[0], color[1], color[2]);

	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < numSegments; i++) {
		float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);

		float x = radius * cosf(theta);
		float y = radius * sinf(theta);

		glVertex2f(centerX + x, centerY + y);
	}

	glEnd();
}