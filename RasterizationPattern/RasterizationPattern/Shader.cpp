#include "stdafx.h"
#include "Shader.h"
#include "Util.h"

namespace OGL
{
	std::tuple<GLenum, const std::string> Shader::shaderInfo[] =
	{
		std::make_tuple(GL_VERTEX_SHADER,".vert"),
		std::make_tuple(GL_FRAGMENT_SHADER,".frag"),
		std::make_tuple(GL_GEOMETRY_SHADER,".geom"),
		//std::make_tuple(GL_TESS_CONTROL_SHADER,".tc"),
		//std::make_tuple(GL_TESS_EVALUATION_SHADER,".te") 
	};

	void Shader::printShaderInfoLog(u32 shaderID)
	{
		i32 max_length = 2048;
		i32 actual_length = 0;
		i8 log[2048];
		glGetShaderInfoLog(shaderID, max_length, &actual_length, log);
		std::cerr << "Shader info log for GL index " << shaderID << " : \n" << log << std::endl;
	}

	u32 Shader::loadShaderFromFile(std::string path, GLenum shaderType)
	{
		//Open file 
		u32 shaderID = 0;
		std::string shaderString;
		std::ifstream sourceFile(path);

		//Source file loaded 
		if (sourceFile) {
			//Get shader source 
			shaderString.assign((std::istreambuf_iterator<i8>(sourceFile)), std::istreambuf_iterator<i8>());
			//Create shader ID 
			shaderID = glCreateShader(shaderType);
			//Set shader source 
			const i8* shaderSource = shaderString.c_str();
			glShaderSource(shaderID, 1, (const i8**)&shaderSource, NULL);
			//Compile shader source 
			glCompileShader(shaderID);
			//Check for compile errors
			i32 params = -1;
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &params);
			if (GL_TRUE != params)
			{
				std::cout << "ERROR : " << path.c_str() << " did not compile " << std::endl;
				printShaderInfoLog(shaderID);
				return false;
			}
		}
		return shaderID;
	}

	Shader::Shader(const std::string& fileName)
	{
		m_name = fileName;
		m_program = glCreateProgram();

		const std::string folderName = "Shaders/";

		for (u8 i = 0; i < SHADER_NUMBER; ++i)
		{
			auto& shaderName = folderName + fileName + std::get<1>(shaderInfo[i]);
			m_shaders[i] = isValidFile(shaderName) ? loadShaderFromFile(shaderName, std::get<0>(shaderInfo[i])) : 0;
			if (m_shaders[i])
				glAttachShader(m_program, m_shaders[i]);
		}
		glLinkProgram(m_program);
		glValidateProgram(m_program);
		retriveUniformLocation();
	}

	Shader::~Shader()
	{
		// Reminder that it works only because we deal with shared_ptr on shaders.
		// Ref count is cool when high perf is not an issue.
		for (u32 i = 0; i < SHADER_NUMBER; ++i)
		{
			if (m_shaders[i])
			{
				glDetachShader(m_program, m_shaders[i]);
				glDeleteShader(m_shaders[i]);
			}
		}
		glDeleteProgram(m_program);
	}

	void Shader::bind()
	{
		glUseProgram(m_program);
	}

	void Shader::unbind()
	{
		glUseProgram(0);
	}

	void Shader::update()
	{
		const std::string folderName = "Shaders/";
		for (u32 i = 0; i < SHADER_NUMBER; ++i)
		{
			if (m_shaders[i])
			{
				glDetachShader(m_program, m_shaders[i]);
				glDeleteShader(m_shaders[i]);
				m_shaders[i] = loadShaderFromFile(folderName + m_name + std::get<1>(shaderInfo[i]), std::get<0>(shaderInfo[i]));
				glAttachShader(m_program, m_shaders[i]);
				glLinkProgram(m_program);
				glValidateProgram(m_program);
			}
		}
		retriveUniformLocation();
	}
	void Shader::retriveUniformLocation()
	{
		i32 numUniforms = 0;
		glGetProgramInterfaceiv(m_program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
		GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

		for (int i = 0; i < numUniforms; ++i) {
			i32 results[4];
			glGetProgramResourceiv(m_program, GL_UNIFORM, i, 4, properties, 4, NULL, results);
			i32 nameBufSize = results[0] + 1;
			i8* name = new i8[nameBufSize];
			glGetProgramResourceName(m_program, GL_UNIFORM, i, nameBufSize, NULL, name);
			u32 location = glGetUniformLocation(m_program, name);
			m_uniformMap[name] = location;
			delete[] name;
		}

		i32 uniformBlocks;
		i32 uniformBlockMaxNameLength;
		i32 uniformMaxNameLength;

		glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlocks);
		glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &uniformBlockMaxNameLength);
		glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxNameLength);

		std::vector<i8> blockName(uniformBlockMaxNameLength);
		std::vector<i8> uniformName(uniformMaxNameLength);

		for (i32 i = 0; i < uniformBlocks; ++i)
		{
			GLsizei length;
			glGetActiveUniformBlockName(m_program, i, uniformBlockMaxNameLength, &length, blockName.data());
			m_uniformBlockList.emplace_back(blockName.data());
		}
	}

	u32 Shader::getUniformLocation(const std::string& s)
	{
		return m_uniformMap[s];
	}

	bool Shader::isValidUBO(const std::string& s)
	{
		return std::find(std::begin(m_uniformBlockList),
			std::end(m_uniformBlockList),
			s)
			!= std::end(m_uniformBlockList);
	}
}