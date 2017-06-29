#include "stdafx.h"
#include "FrameBuffer.h"
#include "Util.h"

namespace OGL
{
	FrameBuffer::FrameBuffer() :
		m_fbo{ 0 },
		m_width{ 0 },
		m_heigth{ 0 }
	{
	}

	FrameBuffer::FrameBuffer(TextureManager& tm, u32 width, u32 heigth, bool useStencil) :
		m_fbo{ 0 },
		m_width{ width },
		m_heigth{ heigth }
	{
		load(tm);
	}

	FrameBuffer::~FrameBuffer()
	{
		glDeleteFramebuffers(1, &m_fbo);
	}

	void FrameBuffer::load(TextureManager& tm)
	{
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		// Atomic counters texture (first pass)
		m_atomicCountersId = tm.createTexture(FBO_TEXTURE2D_R_U32);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tm.getTextureHandle(m_atomicCountersId), 0);

		// Normal rendering texture
		m_normalRenderingId = tm.createTexture(FBO_TEXTURE2D_RGBA);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tm.getTextureHandle(m_normalRenderingId), 0);

		// Final texture (second pass)
		m_finalTextureId = tm.createTexture(FBO_TEXTURE2D_RGBA);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, tm.getTextureHandle(m_finalTextureId), 0);

		// Check FBO state
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteFramebuffers(1, &m_fbo);
		}

		// Unlock Frame Buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FrameBuffer::startFrame(TextureManager& tm)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		clearBindings();
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(2, drawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
	}

	void FrameBuffer::bindForFirstPass(TextureManager& tm)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, drawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
	}

	void FrameBuffer::bindForSecondPass(TextureManager& tm)
	{
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(1, drawBuffers);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.f, 0.f, 0.f, 1.0f);
	}

	u32 FrameBuffer::getFBO() const
	{
		return m_fbo;
	}

	u32 FrameBuffer::getWidth() const
	{
		return m_width;
	}

	u32 FrameBuffer::getHeigth() const
	{
		return m_heigth;
	}
}