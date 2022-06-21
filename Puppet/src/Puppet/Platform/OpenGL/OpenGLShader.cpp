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
	namespace Utils {

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

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
	}

	static GLenum ShaderTypeFromString(std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;

		if (type == "fragment"|| type == "pixel")
			return GL_FRAGMENT_SHADER;

		PP_CORE_ASSERT(false,"Unknow Shader Type!");

		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& glslpath, bool isNative)
		: m_FilePath(glslpath)
	{
		PP_PROFILE_FUNCTION();
		if (isNative)
			CreateNativeShader();
		else
			CreateCrossShader();
		//Utils::CreateCacheDirectoryIfNeeded();

		//std::filesystem::path path = glslpath;
		//m_Name =path.stem().string();
		//std::string source=ReadFile(glslpath);
		//auto shaderSources = PreProcess(source);
		//{
		//	Timer timer;
		//	CompileOrGetVulkanBinaries(shaderSources);
		//	CompileOrGetOpenGLBinaries();
		//	CreateProgram();
		//	PP_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		//}
	}
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		:m_Name(name)
	{
		PP_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources{
			{GL_VERTEX_SHADER,vertexSrc},
			{GL_FRAGMENT_SHADER,fragmentSrc}
		};
		CompileOrGetVulkanBinaries();
		CompileOrGetOpenGLBinaries();
		CreateProgram();
	}
	
	
	OpenGLShader::~OpenGLShader()
	{
		PP_PROFILE_FUNCTION();

		RendererID rid = m_RendererID;
		Renderer::Submit([rid]() {
			glDeleteProgram(rid);
		});
	}
	void OpenGLShader::Bind() const
	{
		PP_PROFILE_FUNCTION();
		Ref<const OpenGLShader> instance = this;
		Renderer::Submit([instance]()mutable {
			glUseProgram(instance->m_RendererID);
		});
	}
	void OpenGLShader::UnBind() const
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([]() {
			glUseProgram(0);
		});
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		PP_PROFILE_FUNCTION();
		Renderer::Submit([=](){
			UploadUniformInt(name, value);
		});
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		PP_PROFILE_FUNCTION();
		//TODO: =copy have problem?
		Renderer::Submit([=]() {
			UploadUniformIntArray(name, values, count);
		});
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([=]() {
			UploadUniformFloat(name, value);
			});
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([=]() {
			UploadUniformFloat2(name, value);
			});
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		PP_PROFILE_FUNCTION();

		Renderer::Submit([=]() {
			UploadUniformFloat3(name, value);
			});
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		PP_PROFILE_FUNCTION();


		Renderer::Submit([=]() {
			UploadUniformFloat4(name, value);
			});
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		PP_PROFILE_FUNCTION();
		
		Renderer::Submit([=]() {
			UploadUniformMat4(name, value);
			});

	}
	 
	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
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
			PP_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			PP_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			m_SourceCode[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}
	}

	void OpenGLShader::CreateNativeShader()
	{
		Utils::CreateCacheDirectoryIfNeeded();
		std::filesystem::path path = m_FilePath;
		m_Name = path.stem().string();
		std::string source = ReadFile(m_FilePath);
		PreProcess(source);
		Compile();
		//{
		//	Timer timer;
		//	//CompileOrGetVulkanBinaries(shaderSources);
		//	CompileOrGetOpenGLBinaries();
		//	CreateProgram();
		//	PP_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		//}
	}

	void OpenGLShader::CreateCrossShader()
	{
		Utils::CreateCacheDirectoryIfNeeded();
		std::filesystem::path path = m_FilePath;
		m_Name = path.stem().string();
		std::string source = ReadFile(m_FilePath);
		PreProcess(source);
		{
			Timer timer;
			CompileOrGetVulkanBinaries();
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			PP_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}
	}

	void OpenGLShader::Compile()
	{
		GLuint program = glCreateProgram();
		std::vector<GLuint> glShaderIDs;
		glShaderIDs.reserve(m_SourceCode.size());
		for (auto&& [type, source] : m_SourceCode)
		{
			// Create an empty vertex shader handle
			GLuint shader = glCreateShader(type);

			// Send the vertex shader source code to GL
			// Note that std::string's .c_str is NULL character terminated.
			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			// Compile the vertex shader
			glCompileShader(shader);

			// ¶¥µã×ÅÉ«Æ÷±àÒë×´Ì¬
			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore
				glDeleteShader(shader);

				PP_CORE_ERROR("{0}", infoLog.data());
				PP_CORE_ASSERT(false, "shader compilation failure!");
				break;
			}
			glShaderIDs.emplace_back(shader);
			glAttachShader(program, shader);
		}

		m_RendererID=program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*
		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_RendererID);
			// Don't leak shaders either.
			for(auto id:glShaderIDs)
				glDeleteShader(id);

			PP_CORE_ERROR("{0}", infoLog.data());
			PP_CORE_ASSERT(false, "Shader link failure!");
			return;
		}
		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
			glDetachShader(m_RendererID, id);
	}
	
	void OpenGLShader::CompileOrGetVulkanBinaries()
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		m_VulkanSPIRV.clear();
		for (auto&& [stage, source] : m_SourceCode)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = m_VulkanSPIRV[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);	
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					PP_CORE_ERROR(module.GetErrorMessage());
					PP_CORE_ASSERT(false);
				}

				m_VulkanSPIRV[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = m_VulkanSPIRV[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : m_VulkanSPIRV)
			Reflect(stage, data);
	}
	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		m_OpenGLSPIRV.clear();
		m_SourceCode.clear();
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = m_OpenGLSPIRV[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_SourceCode[stage] = glslCompiler.compile();
				auto& source = m_SourceCode[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					PP_CORE_ERROR(module.GetErrorMessage());
					PP_CORE_ASSERT(false);
				}

				m_OpenGLSPIRV[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = m_OpenGLSPIRV[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}
	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			PP_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		m_RendererID = program;
	}
	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		PP_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		PP_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		PP_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		PP_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			PP_CORE_TRACE("  {0}", resource.name);
			PP_CORE_TRACE("    Size = {0}", bufferSize);
			PP_CORE_TRACE("    Binding = {0}", binding);
			PP_CORE_TRACE("    Members = {0}", memberCount);
		}
	}
}