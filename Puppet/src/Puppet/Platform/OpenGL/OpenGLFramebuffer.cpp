#include "PPpch.h"
#include "OpenGLFramebuffer.h"
#include "Puppet/Renderer/Renderer.h"
#include <glad/glad.h>
namespace Puppet {

	namespace Utils {

		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum internalFormat, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			case FramebufferTextureFormat::DEPTH32F:	 return true;
			}

			return false;
		}

		static GLenum PuppetFBTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
			case FramebufferTextureFormat::RGBA16F:       return GL_RGBA16F;
			case FramebufferTextureFormat::RGBA32F: return GL_RGBA32F;
			case FramebufferTextureFormat::RG32F:       return GL_RG32F;
			case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			case FramebufferTextureFormat::DEPTH32F:       return GL_DEPTH_COMPONENT32F;
			case FramebufferTextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
			}

			PP_CORE_ASSERT(false," Unknow FramebufferTextureFormat");
			return 0;
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{

		Renderer::Submit([instance = Ref<OpenGLFramebuffer>(this)]()mutable {
			glDeleteFramebuffers(1, &instance->m_RendererID);
			glDeleteTextures(instance->m_ColorAttachments.size(), instance->m_ColorAttachments.data());
			glDeleteTextures(1, &instance->m_DepthAttachment);
		});
	}

	void OpenGLFramebuffer::Invalidate()
	{

		Renderer::Submit([instance = Ref<OpenGLFramebuffer>(this)]() mutable {
			if (instance->m_RendererID)
			{
				glDeleteFramebuffers(1, &instance->m_RendererID);
				glDeleteTextures(instance->m_ColorAttachments.size(), instance->m_ColorAttachments.data());
				glDeleteTextures(1, &instance->m_DepthAttachment);

				instance->m_ColorAttachments.clear();
				instance->m_DepthAttachment = 0;
			}

			glCreateFramebuffers(1, &instance->m_RendererID);
			glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);

			bool multisample = instance->m_Specification.Samples > 1;

			// Attachments
			if (instance->m_ColorAttachmentSpecifications.size())
			{
				instance->m_ColorAttachments.resize(instance->m_ColorAttachmentSpecifications.size());
				Utils::CreateTextures(multisample, instance->m_ColorAttachments.data(), instance->m_ColorAttachments.size());

				for (size_t i = 0; i < instance->m_ColorAttachments.size(); i++)
				{
					Utils::BindTexture(multisample, instance->m_ColorAttachments[i]);
					switch (instance->m_ColorAttachmentSpecifications[i].TextureFormat)
					{
					case FramebufferTextureFormat::RGBA8:
						Utils::AttachColorTexture(instance->m_ColorAttachments[i], instance->m_Specification.Samples, GL_RGBA8, GL_RGBA, instance->m_Specification.Width, instance->m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						Utils::AttachColorTexture(instance->m_ColorAttachments[i], instance->m_Specification.Samples, GL_R32I, GL_RED_INTEGER, instance->m_Specification.Width, instance->m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RGBA16F:
						Utils::AttachColorTexture(instance->m_ColorAttachments[i], instance->m_Specification.Samples, GL_RGBA16F, GL_RGBA, instance->m_Specification.Width, instance->m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RGBA32F:
						Utils::AttachColorTexture(instance->m_ColorAttachments[i], instance->m_Specification.Samples, GL_RGBA32F, GL_RGBA, instance->m_Specification.Width, instance->m_Specification.Height, i);
						break;
					case FramebufferTextureFormat::RG32F:
						Utils::AttachColorTexture(instance->m_ColorAttachments[i], instance->m_Specification.Samples, GL_RG32F, GL_RGBA, instance->m_Specification.Width, instance->m_Specification.Height, i);
						break;
					}
				}
			}

			if (instance->m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
			{
				Utils::CreateTextures(multisample, &instance->m_DepthAttachment, 1);
				Utils::BindTexture(multisample, instance->m_DepthAttachment);
				switch (instance->m_DepthAttachmentSpecification.TextureFormat)
				{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(instance->m_DepthAttachment, instance->m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, instance->m_Specification.Width, instance->m_Specification.Height);
					break;
				case FramebufferTextureFormat::DEPTH32F:
					Utils::AttachDepthTexture(instance->m_DepthAttachment, instance->m_Specification.Samples, GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, instance->m_Specification.Width, instance->m_Specification.Height);
					break;
				}
			}

			if (instance->m_ColorAttachments.size() > 1)
			{
				PP_CORE_ASSERT(instance->m_ColorAttachments.size() <= 4, "m_ColorAttachments.size() > 4");
				GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
				glDrawBuffers(instance->m_ColorAttachments.size(), buffers);
			}
			else if (instance->m_ColorAttachments.empty())
			{
				// Only depth-pass
				glDrawBuffer(GL_NONE);
			}

			PP_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		});
	}

	void OpenGLFramebuffer::BindTexture(uint32_t attachmentIndex, uint32_t slot) const
	{
		Renderer::Submit([instance = Ref<const OpenGLFramebuffer>(this), attachmentIndex, slot]() {
			glBindTextureUnit(slot, instance->m_ColorAttachments[attachmentIndex]);
		});
	}

	void OpenGLFramebuffer::CopyFromOther(const Ref<Framebuffer>& readFrambuffer)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, readFrambuffer->GetRendererID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
		glBlitFramebuffer(0, 0, m_Specification.Width, m_Specification.Height, 0, 0,
			m_Specification.Width, m_Specification.Height, GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT|
			GL_STENCIL_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		Renderer::Submit([instance= Ref<const OpenGLFramebuffer>(this)]() {
			glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);
			glViewport(0, 0, instance->m_Specification.Width, instance->m_Specification.Height);
		});
	}

	void OpenGLFramebuffer::Unbind()
	{
		Renderer::Submit([]() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		});
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
	{	
		//not Recreate
		if (!forceRecreate && (m_Specification.Width == width && m_Specification.Height == height))
			return;
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		PP_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(),"attachmentIndex >= m_ColorAttachments.size()");
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return pixelData;

	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		PP_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(),"attachmentIndex > m_ColorAttachments.size()");
		
		Renderer::Submit([instance = Ref<OpenGLFramebuffer>(this), attachmentIndex,value]()mutable {
			auto& spec = instance->m_ColorAttachmentSpecifications[attachmentIndex];
			glClearTexImage(instance->m_ColorAttachments[attachmentIndex], 0,
				Utils::PuppetFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
		});	
	}

}