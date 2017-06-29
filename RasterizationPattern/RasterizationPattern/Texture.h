#pragma once
#include "Uncopiable.h"

namespace OGL
{
	enum MipMapUse : bool 
	{
		NO_MIPMAP = false,
		USE_MIPMAP = true,
	};

	enum SrgbUse : bool
	{
		NO_SRGB = false,
		USE_SRGB = true,
	};

	enum VerticalFlipUse : bool
	{
		NO_VFLIP = false,
		DO_VFLIP = true,
	};

	struct TextureParams
	{
		GLenum		m_target;
		GLint		m_level;
		GLint		m_internalFormat;
		GLsizei		m_width;
		GLsizei		m_height;
		GLsizei		m_depth;
		GLint		m_border;
		GLenum		m_format;
		GLenum		m_type;
		SrgbUse		m_sRGB;
		MipMapUse	m_mips;

		TextureParams() {};
		TextureParams(GLenum target, GLint level, GLint internalFormat, GLsizei width,
			GLsizei height, GLsizei depth, GLint border, GLenum format,
			GLenum type, SrgbUse sRGB, MipMapUse mips) :
			m_target{ target },
			m_level{ level },
			m_internalFormat{ internalFormat },
			m_width{ width },
			m_height{ height },
			m_depth{ depth },
			m_border{ border },
			m_format{ format },
			m_type{ type },
			m_sRGB{ sRGB },
			m_mips{ mips }
		{
		};
	};

	extern TextureParams ASSET_TEXTURE2D_MIP_ON;
	extern TextureParams ASSET_TEXTURE2D_MIP_OFF;
	extern TextureParams FBO_TEXTURE2D_RGBA;
	extern TextureParams FBO_TEXTURE2D_R_U32;

	class Texture
	{
	public:
		Texture(TextureParams& params, const std::string& filename = std::string(""), VerticalFlipUse flip = NO_VFLIP, i32 id = -1);
		Texture(const std::string& filename = std::string(""), i32 id = -1);

		// "Holy Trinity"
		Texture(const Texture& other) :
			m_id{ other.m_id },
			m_filename{ other.m_filename },
			m_params{ other.m_params },
			m_textureObj{ other.m_textureObj }
		{
		}
		void swap(Texture &other)
		{
			using std::swap;
			swap(m_id, other.m_id);
			swap(m_filename, other.m_filename);
			swap(m_params, other.m_params);
			swap(m_textureObj, other.m_textureObj);
		}
		Texture& operator=(const Texture&other)
		{
			Texture{ other }.swap(*this);
			return *this;
		}
		~Texture();

		void kill();
		void loadTextureFromFile(VerticalFlipUse flip);
		void loadFramebufferTexture();
	
		void bind(GLenum TextureUnit);
		const std::string& getName() { return m_filename; }

	public:
		std::string m_filename;
		TextureParams m_params;
		i32 m_id;
		u32 m_textureObj;
	};
}
