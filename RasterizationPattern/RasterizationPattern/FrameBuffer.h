#pragma once
#include "TextureManager.h"

namespace OGL
{
	class FrameBuffer
	{
	private:
		void load(TextureManager& tm);
	public:
		FrameBuffer();
		FrameBuffer(TextureManager& tm, u32 width, u32 heigth, bool useStencil = false);
		~FrameBuffer();

		void startFrame(TextureManager& tm);
		void bindForFirstPass(TextureManager& tm);
		void bindForSecondPass(TextureManager& tm);

		GLuint getFBO() const;
		u32 getWidth() const;
		u32 getHeigth() const;

		u32 m_width;
		u32 m_heigth;

		u32 m_fbo;
		i32 m_atomicCountersId;
		i32 m_normalRenderingId;
		i32 m_finalTextureId;
	};
}