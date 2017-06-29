#include "stdafx.h"
#include "Texture.h"
#include "Util.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace OGL
{
	TextureParams ASSET_TEXTURE2D_MIP_ON{ GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NO_SRGB, USE_MIPMAP };
	TextureParams ASSET_TEXTURE2D_MIP_OFF{ GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NO_SRGB, NO_MIPMAP };
	TextureParams FBO_TEXTURE2D_RGBA{ GL_TEXTURE_2D, 0, GL_RGBA8, 640, 480, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, NO_SRGB, NO_MIPMAP };
	TextureParams FBO_TEXTURE2D_R_U32{ GL_TEXTURE_2D, 0, GL_R32UI, 640, 480, 0, 0,  GL_RED_INTEGER, GL_UNSIGNED_INT, NO_SRGB, NO_MIPMAP };

	Texture::Texture(TextureParams& params, const std::string& filename, VerticalFlipUse flip, i32 id)
	{
		m_id = id;
		m_params = params;
		m_filename = filename;

		// TODO : test on strings are bad. Not done each frame but still : Sad !
		if (filename.empty())
			loadFramebufferTexture();
		else
			loadTextureFromFile(flip);

		glBindTexture(m_params.m_target, m_textureObj);
	}

	Texture::~Texture()
	{
		// call kill() to delete the texture properly
	}

	void Texture::kill()
	{
		glDeleteTextures(1, &m_textureObj);
	}

	void Texture::loadTextureFromFile(VerticalFlipUse flip)
	{
		// Image loading
		i32 x, y, n;
		i32 force_channels = 4;

		if (flip)
			stbi_set_flip_vertically_on_load(true);
		else
			stbi_set_flip_vertically_on_load(false);

		u8* image_data = stbi_load(m_filename.c_str(), &x, &y, &n, force_channels);
		if (!image_data) 
		{
			std::cerr << "ERROR : could not load : " << m_filename << std::endl;
			return;
		}

		m_params.m_width = x;
		m_params.m_height = y;

		// Power of 2 check, may screw the mipmap if not
		if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0)
		{
			std::cerr << "WARNING : " << m_filename << " is not a power of 2" << std::endl;
		}

		glGenTextures(1, &m_textureObj);
		glBindTexture(m_params.m_target, m_textureObj);
		glTexImage2D(m_params.m_target,
			m_params.m_level,
			m_params.m_internalFormat,
			m_params.m_width,
			m_params.m_height,
			m_params.m_border,
			m_params.m_format,
			m_params.m_type,
			image_data);

		if (m_params.m_mips)
			glGenerateMipmap(m_params.m_target);

		// Trilinear filtering.
		glTexParameteri(m_params.m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_params.m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(m_params.m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(m_params.m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// set the maximum anisotropic filtering level allowed (perf?)
		f32 max_aniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
		glTexParameterf(m_params.m_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

		// no longer need the loaded image 
		stbi_image_free(image_data);
	}
	
	void Texture::loadFramebufferTexture()
	{
		glGenTextures(1, &m_textureObj);
		glBindTexture(m_params.m_target, m_textureObj);

		if (m_params.m_target == GL_TEXTURE_2D)
		{
			glTexImage2D(m_params.m_target,
				m_params.m_level,
				m_params.m_internalFormat,
				m_params.m_width,
				m_params.m_height,
				m_params.m_border,
				m_params.m_format,
				m_params.m_type,
				nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (m_params.m_internalFormat == GL_DEPTH_COMPONENT)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			}

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			if (m_params.m_mips == USE_MIPMAP)
				glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	void Texture::bind(GLenum textureUnit)
	{
		glActiveTexture(textureUnit);
		glBindTexture(m_params.m_target, m_textureObj);
	}
}