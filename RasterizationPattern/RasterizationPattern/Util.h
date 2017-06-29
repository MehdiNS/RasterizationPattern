#pragma once

namespace OGL
{

#define PI (3.14159265358979f)
#define TWOPI (2.f*PI)
#define FOURPI (4.f*PI)
#define NB_ELEMS(x)  (sizeof(x) / sizeof((x)[0]))

	enum
	{
		USE_GEOM = 1,
		USE_TESS
	};

	// GL util 
	void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar *message, void *userParam);
	void clearBindings();
	void drawQuad();

	// C++ util
	bool isValidFile(const fs::path& p, fs::file_status s = fs::file_status{});
}
