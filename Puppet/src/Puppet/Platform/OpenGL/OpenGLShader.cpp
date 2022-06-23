#include "PPpch.h"
#include "OpenGLShader.h"
#include "Puppet/Renderer/Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include "Puppet/Core/Timer.h"
namespace Puppet
{
#define UNIFORM_LOGGING 0
#if UNIFORM_LOGGING
#define PP_LOG_UNIFORM(...) PP_CORE_WARN(__VA_ARGS__)
#else
#define PP_LOG_UNIFORM
#endif

	namespace Utils {
		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;
			if (type == "compute")
				return GL_COMPUTE_SHADER;
			PP_CORE_ASSERT(false, "Unknown shader type!");
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			PP_CORE_ASSERT(false);
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			PP_CORE_ASSERT(false);
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/opengl";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			PP_CORE_ASSERT(false);
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			PP_CORE_ASSERT(false);
			return "";
		}

		// Parsing helper functions
		static const char* FindToken(const char* str, const std::string& token)
		{
			const char* t = str;
			while (t = strstr(t, token.c_str()))
			{
				bool left = str == t || isspace(t[-1]);
				bool right = !t[token.size()] || isspace(t[token.size()]);
				if (left && right)
					return t;

				t += token.size();
			}
			return nullptr;
		}

		static const char* FindToken(const std::string& string, const std::string& token)
		{
			return FindToken(string.c_str(), token);
		}

		static std::vector<std::string> SplitString(const std::string& string, const std::string& delimiters)
		{
			size_t start = 0;
			size_t end = string.find_first_of(delimiters);

			std::vector<std::string> result;

			while (end <= std::string::npos)
			{
				std::string token = string.substr(start, end - start);
				if (!token.empty())
					result.push_back(token);

				if (end == std::string::npos)
					break;

				start = end + 1;
				end = string.find_first_of(delimiters, start);
			}

			return result;
		}

		static std::vector<std::string> SplitString(const std::string& string, const char delimiter)
		{
			return SplitString(string, std::string(1, delimiter));
		}

		static std::vector<std::string> Tokenize(const std::string& string)
		{
			return SplitString(string, " \t\n\r");
		}

		static std::vector<std::string> GetLines(const std::string& string)
		{
			return SplitString(string, "\n");
		}

		static std::string GetBlock(const char* str, const char** outPosition)
		{
			const char* end = strstr(str, "}");
			if (!end)
				return str;

			if (outPosition)
				*outPosition = end;
			uint32_t length = end - str + 1;
			return std::string(str, length);
		}

		static std::string GetStatement(const char* str, const char** outPosition)
		{
			const char* end = strstr(str, ";");
			if (!end)
				return str;

			if (outPosition)
				*outPosition = end;
			uint32_t length = end - str + 1;
			return std::string(str, length);
		}

		static bool StartsWith(const std::string& string, const std::string& start)
		{
			return string.find(start) == 0;
		}

		static bool IsTypeStringResource(const std::string& type)
		{
			if (type == "sampler1D")		return true;
			if (type == "sampler2D")		return true;
			if (type == "sampler2DMS")		return true;
			if (type == "samplerCube")		return true;
			if (type == "sampler2DShadow")	return true;
			return false;
		}

	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: m_FilePath(filepath)
	{
		
		std::filesystem::path path = m_FilePath;
		m_Name = path.stem().string();
		
		Reload();
	}

	void OpenGLShader::Reload()
	{
		std::string source = ReadFile(m_FilePath);
		PreProcess(source);
		if (!m_IsCompute)
			Parse();

		Renderer::Submit([instance=Ref<OpenGLShader>(this)]() mutable{
			if (instance->m_RendererID)
				glDeleteProgram(instance->m_RendererID);

			instance->CompileAndUploadShader();
			if (!instance->m_IsCompute)
			{
				instance->ResolveUniforms();
				instance->ValidateUniforms();
			}

			if (instance->m_Loaded)
			{
				for (auto& callback : instance->m_ShaderReloadedCallbacks)
					callback();
			}

			instance->m_Loaded = true;
		});
	}

	OpenGLShader::~OpenGLShader()
	{
		RendererID rid = m_RendererID;
		Renderer::Submit([rid]() {
			glDeleteProgram(rid);
		});
	}

	void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback& callback)
	{
		m_ShaderReloadedCallbacks.push_back(callback);
	}
	void OpenGLShader::Bind() const
	{

		Renderer::Submit([instance=Ref<const OpenGLShader>(this)](){
			glUseProgram(instance->m_RendererID);
		});
	}
	void OpenGLShader::UnBind() const
	{

		Renderer::Submit([]() {
			glUseProgram(0);
		});
	}

	void OpenGLShader::SetVSMaterialUniformBuffer(Buffer buffer)
	{

		Renderer::Submit([instance = Ref <OpenGLShader>(this),buffer]()mutable {
			glUseProgram(instance->m_RendererID);
			instance->ResolveAndSetUniforms(instance->m_VSMaterialUniformBuffer, buffer);
		});
	}

	void OpenGLShader::SetPSMaterialUniformBuffer(Buffer buffer)
	{

		Renderer::Submit([instance = Ref <OpenGLShader>(this), buffer]()mutable {
			glUseProgram(instance->m_RendererID);
			instance->ResolveAndSetUniforms(instance->m_PSMaterialUniformBuffer, buffer);
		});
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([instance = Ref <OpenGLShader>(this), name,value]()mutable {
			instance->UploadUniformInt(name, value);
		});
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		PP_PROFILE_FUNCTION();
		int* copy = new int[count];
		memcpy(copy, values, sizeof(int) * count);
		Renderer::Submit([instance= Ref <OpenGLShader>(this),name, copy,count]()mutable {
			instance->UploadUniformIntArray(name, copy, count);
			delete[] copy;
		});
	}

	void OpenGLShader::SetBool(const std::string& name, bool value)
	{
		Renderer::Submit([instance = Ref <OpenGLShader>(this),name,value]()mutable {
			instance->UploadUniformInt(name, value);
		});
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([instance = Ref <OpenGLShader>(this),name,value]()mutable {
			instance->UploadUniformFloat(name, value);
		});
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([instance = Ref <OpenGLShader>(this), name, value]()mutable {
			instance->UploadUniformFloat2(name, value);
		});
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([instance = Ref <OpenGLShader>(this), name, value]()mutable {
			instance->UploadUniformFloat3(name, value);
			});
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		PP_PROFILE_FUNCTION();


		Renderer::Submit([instance = Ref <OpenGLShader>(this), name, value]()mutable {
			instance->UploadUniformFloat4(name, value);
			});
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		PP_PROFILE_FUNCTION();
		
		Renderer::Submit([instance = Ref <OpenGLShader>(this), name, value]()mutable {
			instance->UploadUniformMat4(name, value);
			});

	}

	void OpenGLShader::SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind)
	{
		UploadUniformMat4(name, value);
	}
	 
	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		glUseProgram(m_RendererID);
		GLint location = GetUniformLocation(name);
		if(location!=-1)
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		glUseProgram(m_RendererID);
		GLint location = GetUniformLocation(name);
		if (location != -1)
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		glUseProgram(m_RendererID);
		GLint location = GetUniformLocation(name);
		if (location != -1)
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		glUseProgram(m_RendererID);
		GLint location = GetUniformLocation(name);
		if(location!=-1)
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		glUseProgram(m_RendererID);
		GLint location = GetUniformLocation(name);
		if (location != -1)
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		glUseProgram(m_RendererID);
		GLint location = GetUniformLocation(name);
		if (location != -1)
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		glUseProgram(m_RendererID);
		GLint location = GetUniformLocation(name);
		if (location != -1)
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		glUseProgram(m_RendererID);
		GLint location = GetUniformLocation(name);
		if (location != -1)
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}


	std::string OpenGLShader::ReadFile(const std::string& glslpath)
	{
		std::string result;
		std::ifstream in(glslpath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
				in.close();
			}
			else
			{
				PP_CORE_ERROR("Could not read from file '{0}'", glslpath);
			}
		}
		else
		{
			PP_CORE_ERROR("Could not open file '{0}'", glslpath);
		}
		return result;
	}

	void OpenGLShader::PreProcess(const std::string& source)
	{
		m_SourceCode.clear();
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			PP_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			PP_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			PP_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line
			
			auto shaderType = Utils::ShaderTypeFromString(type);
			m_SourceCode[shaderType] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		
			// Compute shaders cannot contain other types
			if (shaderType == GL_COMPUTE_SHADER)
			{
				m_IsCompute = true;
				break;
			}
		}
	}

	void OpenGLShader::Parse()
	{
		const char* token;
		const char* vstr;
		const char* fstr;

		m_Resources.clear();
		m_Structs.clear();
		m_VSMaterialUniformBuffer.Reset();
		m_PSMaterialUniformBuffer.Reset();

		auto& vertexSource = m_SourceCode[GL_VERTEX_SHADER];
		auto& fragmentSource = m_SourceCode[GL_FRAGMENT_SHADER];

		// Vertex Shader
		vstr = vertexSource.c_str();
		while (token = Utils::FindToken(vstr, "struct"))
			ParseUniformStruct(Utils::GetBlock(token, &vstr), ShaderDomain::Vertex);

		vstr = vertexSource.c_str();
		while (token = Utils::FindToken(vstr, "uniform"))
			ParseUniform(Utils::GetStatement(token, &vstr), ShaderDomain::Vertex);

		// Fragment Shader
		fstr = fragmentSource.c_str();
		while (token = Utils::FindToken(fstr, "struct"))
			ParseUniformStruct(Utils::GetBlock(token, &fstr), ShaderDomain::Pixel);

		fstr = fragmentSource.c_str();
		while (token = Utils::FindToken(fstr, "uniform"))
			ParseUniform(Utils::GetStatement(token, &fstr), ShaderDomain::Pixel);
	}

	void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain)
	{
		std::vector<std::string> tokens = Utils::Tokenize(statement);
		uint32_t index = 0;

		index++; // "uniform"
		std::string typeString = tokens[index++];
		std::string name = tokens[index++];
		// Strip ; from name if present
		if (const char* s = strstr(name.c_str(), ";"))
			name = std::string(name.c_str(), s - name.c_str());

		std::string n(name);
		int32_t count = 1;
		const char* namestr = n.c_str();
		if (const char* s = strstr(namestr, "["))
		{
			name = std::string(namestr, s - namestr);

			const char* end = strstr(namestr, "]");
			std::string c(s + 1, end - s);
			count = atoi(c.c_str());
		}

		if (Utils::IsTypeStringResource(typeString))
		{
			ShaderResourceDeclaration* declaration = new OpenGLShaderResourceDeclaration(OpenGLShaderResourceDeclaration::StringToType(typeString), name, count);
			m_Resources.push_back(declaration);
		}
		else
		{
			OpenGLShaderUniformDeclaration::Type t = OpenGLShaderUniformDeclaration::StringToType(typeString);
			OpenGLShaderUniformDeclaration* declaration = nullptr;

			if (t == OpenGLShaderUniformDeclaration::Type::NONE)
			{
				// Find struct
				ShaderStruct* s = FindStruct(typeString);
				PP_CORE_ASSERT(s, "");
				declaration = new OpenGLShaderUniformDeclaration(domain, s, name, count);
			}
			else
			{
				declaration = new OpenGLShaderUniformDeclaration(domain, t, name, count);
			}

			if (Utils::StartsWith(name, "r_"))
			{
				if (domain == ShaderDomain::Vertex)
					((OpenGLShaderUniformBufferDeclaration*)m_VSRendererUniformBuffers.front())->PushUniform(declaration);
				else if (domain == ShaderDomain::Pixel)
					((OpenGLShaderUniformBufferDeclaration*)m_PSRendererUniformBuffers.front())->PushUniform(declaration);
			}
			else
			{
				if (domain == ShaderDomain::Vertex)
				{
					if (!m_VSMaterialUniformBuffer)
						m_VSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDeclaration("", domain));

					m_VSMaterialUniformBuffer->PushUniform(declaration);
				}
				else if (domain == ShaderDomain::Pixel)
				{
					if (!m_PSMaterialUniformBuffer)
						m_PSMaterialUniformBuffer.Reset(new OpenGLShaderUniformBufferDeclaration("", domain));

					m_PSMaterialUniformBuffer->PushUniform(declaration);
				}
			}
		}
	}

	void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain)
	{
		std::vector<std::string> tokens = Utils::Tokenize(block);

		uint32_t index = 0;
		index++; // struct
		std::string name = tokens[index++];
		ShaderStruct* uniformStruct = new ShaderStruct(name);
		index++; // {
		while (index < tokens.size())
		{
			if (tokens[index] == "}")
				break;

			std::string type = tokens[index++];
			std::string name = tokens[index++];

			// Strip ; from name if present
			if (const char* s = strstr(name.c_str(), ";"))
				name = std::string(name.c_str(), s - name.c_str());

			uint32_t count = 1;
			const char* namestr = name.c_str();
			if (const char* s = strstr(namestr, "["))
			{
				name = std::string(namestr, s - namestr);

				const char* end = strstr(namestr, "]");
				std::string c(s + 1, end - s);
				count = atoi(c.c_str());
			}
			ShaderUniformDeclaration* field = new OpenGLShaderUniformDeclaration(domain, OpenGLShaderUniformDeclaration::StringToType(type), name, count);
			uniformStruct->AddField(field);
		}
		m_Structs.push_back(uniformStruct);
	}

	ShaderStruct* OpenGLShader::FindStruct(const std::string& name)
	{
		for (ShaderStruct* s : m_Structs)
		{
			if (s->GetName() == name)
				return s;
		}
		return nullptr;
	}

	int32_t OpenGLShader::GetUniformLocation(const std::string& name) const
	{
		int32_t result = glGetUniformLocation(m_RendererID, name.c_str());
		if (result == -1)
			PP_LOG_UNIFORM("Could not find uniform '{0}' in shader{1}", name,m_FilePath);
		return result;
	}

	void OpenGLShader::CompileAndUploadShader()
	{
		std::vector<GLuint> shaderRendererIDs;

		GLuint program = glCreateProgram();
		for (auto& kv : m_SourceCode)
		{
			GLenum type = kv.first;
			std::string& source = kv.second;

			GLuint shaderRendererID = glCreateShader(type);
			const GLchar* sourceCstr = (const GLchar*)source.c_str();
			glShaderSource(shaderRendererID, 1, &sourceCstr, 0);

			glCompileShader(shaderRendererID);

			GLint isCompiled = 0;
			glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderRendererID, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

				PP_CORE_ERROR("Shader compilation failed ({0}):\n{1}", m_FilePath, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shaderRendererID);

				PP_CORE_ASSERT(false, "Failed");
			}

			shaderRendererIDs.push_back(shaderRendererID);
			glAttachShader(program, shaderRendererID);
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			PP_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : shaderRendererIDs)
				glDeleteShader(id);
		}

		// Always detach shaders after a successful link.
		for (auto id : shaderRendererIDs)
			glDetachShader(program, id);

		m_RendererID = program;
	}
	void OpenGLShader::ResolveUniforms()
	{
		glUseProgram(m_RendererID);

		for (size_t i = 0; i < m_VSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDeclaration* decl = (OpenGLShaderUniformBufferDeclaration*)m_VSRendererUniformBuffers[i];
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
				if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
						field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->m_Name);
				}
			}
		}

		for (size_t i = 0; i < m_PSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDeclaration* decl = (OpenGLShaderUniformBufferDeclaration*)m_PSRendererUniformBuffers[i];
			const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
			for (size_t j = 0; j < uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
				if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& s = uniform->GetShaderUniformStruct();
					const auto& fields = s.GetFields();
					for (size_t k = 0; k < fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
						field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
					}
				}
				else
				{
					uniform->m_Location = GetUniformLocation(uniform->m_Name);
				}
			}
		}

		{
			const auto& decl = m_VSMaterialUniformBuffer;
			if (decl)
			{
				const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
				for (size_t j = 0; j < uniforms.size(); j++)
				{
					OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
					if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
					{
						const ShaderStruct& s = uniform->GetShaderUniformStruct();
						const auto& fields = s.GetFields();
						for (size_t k = 0; k < fields.size(); k++)
						{
							OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
							field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
						}
					}
					else
					{
						uniform->m_Location = GetUniformLocation(uniform->m_Name);
					}
				}
			}
		}

		{
			const auto& decl = m_PSMaterialUniformBuffer;
			if (decl)
			{
				const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
				for (size_t j = 0; j < uniforms.size(); j++)
				{
					OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[j];
					if (uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
					{
						const ShaderStruct& s = uniform->GetShaderUniformStruct();
						const auto& fields = s.GetFields();
						for (size_t k = 0; k < fields.size(); k++)
						{
							OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
							field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
						}
					}
					else
					{
						uniform->m_Location = GetUniformLocation(uniform->m_Name);
					}
				}
			}
		}

		uint32_t sampler = 0;
		for (size_t i = 0; i < m_Resources.size(); i++)
		{
			OpenGLShaderResourceDeclaration* resource = (OpenGLShaderResourceDeclaration*)m_Resources[i];
			int32_t location = GetUniformLocation(resource->m_Name);

			if (resource->GetCount() == 1)
			{
				resource->m_Register = sampler;
				if (location != -1)
					UploadUniformInt(location, sampler);

				sampler++;
			}
			else if (resource->GetCount() > 1)
			{
				resource->m_Register = sampler;
				uint32_t count = resource->GetCount();
				int* samplers = new int[count];
				for (uint32_t s = 0; s < count; s++)
					samplers[s] = sampler++;
				UploadUniformIntArray(resource->GetName(), samplers, count);
				delete[] samplers;
			}
		}
	}
	void OpenGLShader::ValidateUniforms()
	{
	}
	void OpenGLShader::ResolveAndSetUniforms(const Ref<OpenGLShaderUniformBufferDeclaration>& decl, Buffer buffer)
	{
		const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
		for (size_t i = 0; i < uniforms.size(); i++)
		{
			OpenGLShaderUniformDeclaration* uniform = (OpenGLShaderUniformDeclaration*)uniforms[i];
			if (uniform->IsArray())
				ResolveAndSetUniformArray(uniform, buffer);
			else
				ResolveAndSetUniform(uniform, buffer);
		}
	}
	void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
	{
		if (uniform->GetLocation() == -1)
			return;

		PP_CORE_ASSERT(uniform->GetLocation() != -1, "Uniform has invalid location!");

		uint32_t offset = uniform->GetOffset();
		switch (uniform->GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::BOOL:
			UploadUniformFloat(uniform->GetLocation(), *(bool*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(uniform->GetLocation(), *(glm::vec2*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(uniform->GetLocation(), *(glm::vec3*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(uniform->GetLocation(), *(glm::vec4*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(uniform->GetLocation(), *(glm::mat3*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4(uniform->GetLocation(), *(glm::mat4*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::STRUCT:
			UploadUniformStruct(uniform, buffer.Data, offset);
			break;
		default:
			PP_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}
	void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* uniform, Buffer buffer)
	{
		uint32_t offset = uniform->GetOffset();
		switch (uniform->GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::BOOL:
			UploadUniformFloat(uniform->GetLocation(), *(bool*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(uniform->GetLocation(), *(float*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(uniform->GetLocation(), *(int32_t*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(uniform->GetLocation(), *(glm::vec2*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(uniform->GetLocation(), *(glm::vec3*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(uniform->GetLocation(), *(glm::vec4*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(uniform->GetLocation(), *(glm::mat3*)&buffer.Data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4Array(uniform->GetLocation(), *(glm::mat4*)&buffer.Data[offset], uniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::STRUCT:
			UploadUniformStruct(uniform, buffer.Data, offset);
			break;
		default:
			PP_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}
	void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& field, byte* data, int32_t offset)
	{
		switch (field.GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::BOOL:
			UploadUniformFloat(field.GetLocation(), *(bool*)&data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(field.GetLocation(), *(float*)&data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(field.GetLocation(), *(int32_t*)&data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(field.GetLocation(), *(glm::vec2*)&data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(field.GetLocation(), *(glm::vec3*)&data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(field.GetLocation(), *(glm::vec4*)&data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(field.GetLocation(), *(glm::mat3*)&data[offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4(field.GetLocation(), *(glm::mat4*)&data[offset]);
			break;
		default:
			PP_CORE_ASSERT(false, "Unknown uniform type!");
		}
	}
	void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration* uniform, byte* buffer, uint32_t offset)
	{
		const ShaderStruct& s = uniform->GetShaderUniformStruct();
		const auto& fields = s.GetFields();
		for (size_t k = 0; k < fields.size(); k++)
		{
			OpenGLShaderUniformDeclaration* field = (OpenGLShaderUniformDeclaration*)fields[k];
			ResolveAndSetUniformField(*field, buffer, offset);
			offset += field->m_Size;
		}
	}
	void OpenGLShader::UploadUniformInt(uint32_t location, int32_t value)
	{
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformIntArray(uint32_t location, int32_t* values, int32_t count)
	{
		glUniform1iv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloat(uint32_t location, float value)
	{
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(uint32_t location, const glm::vec2& value)
	{
		glUniform2f(location, value.x, value.y);
	}
	void OpenGLShader::UploadUniformFloat3(uint32_t location, const glm::vec3& value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformFloat4(uint32_t location, const glm::vec4& value)
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadUniformMat3(uint32_t location, const glm::mat3& values)
	{
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformMat4(uint32_t location, const glm::mat4& values)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(values));
	}
	void OpenGLShader::UploadUniformMat4Array(uint32_t location, const glm::mat4& values, uint32_t count)
	{
		glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(values));
	}
}