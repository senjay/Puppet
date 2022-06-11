#pragma once
#include "Puppet/Renderer/Framebuffer.h"
namespace Puppet
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();
		
		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		

		virtual void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const override;

		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }

		virtual RendererID GetRendererID() const override { return m_RendererID; }
		virtual RendererID GetColorAttachmentRendererID(int index = 0) const override { return m_ColorAttachments[index]; }
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;
		virtual RendererID GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }
		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
		virtual void CopyFromOther(const Ref<Framebuffer>& readFrambuffer)override;
	private:
		FramebufferSpecification m_Specification;
		RendererID m_RendererID = 0;

		std::vector<RendererID> m_ColorAttachments;
		RendererID m_DepthAttachment;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;
	};
}

