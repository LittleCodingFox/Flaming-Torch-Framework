#include "FlamingCore.hpp"
namespace FlamingTorch
{
#if USE_GRAPHICS
	uint32 FLRBATOGLRBA[3] = {
		GL_COLOR_ATTACHMENT0,
		GL_DEPTH_ATTACHMENT,
		GL_STENCIL_ATTACHMENT
	};

	uint32 FLRBAFTOGLRBAF[3] = {
		GL_RGBA4,
		GL_DEPTH_COMPONENT,
		GL_STENCIL_INDEX8
	};

	RenderTarget::RenderTarget() : FBOHandle(0), WidthValue(0), HeightValue(0) {};

	RenderTarget::~RenderTarget()
	{
		Destroy();
	};

	bool RenderTarget::Initialize()
	{
		Destroy();
		glGenFramebuffers(1, (GLuint*)&FBOHandle);

		GLCHECK();

		//Force generation of FBO
		glBindFramebuffer(GL_FRAMEBUFFER, FBOHandle);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GLCHECK();

		return true;
	};

	void RenderTarget::Destroy()
	{
		if(glIsFramebuffer(FBOHandle))
		{
			glDeleteFramebuffers(1, (const GLuint*)&FBOHandle);
		};

		GLCHECK();

		FBOHandle = 0;

		for(uint32 i = 0; i < RenderBuffers.size(); i++)
		{
			glDeleteRenderbuffers(1, (const GLuint*)&RenderBuffers[i]);
		};

		GLCHECK();

		RenderBuffers.clear();
		Attachments.clear();
		WidthValue = HeightValue = 0;
	};

	void RenderTarget::AddRenderBuffer(uint32 Width, uint32 Height, uint32 Type)
	{
		SVOIDFLASSERT(glIsFramebuffer(FBOHandle));

		int32 RenderBuffer;
		glGenRenderbuffers(1, (GLuint*)&RenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBuffer);

		GLCHECK();
		
		glRenderbufferStorage(GL_RENDERBUFFER, FLRBAFTOGLRBAF[Type], Width, Height);

		GLCHECK();
		
		glBindFramebuffer(GL_FRAMEBUFFER, FBOHandle);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, FLRBATOGLRBA[Type], GL_RENDERBUFFER, RenderBuffer);

		GLCHECK();
		
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GLCHECK();

		WidthValue = Width;
		HeightValue = Height;

		RenderBuffers.push_back(RenderBuffer);
	};

	bool RenderTarget::AddColorBuffer(Texture *Texture, uint32 Attachment)
	{
		SFLASSERT(glIsFramebuffer(FBOHandle) && Texture && Texture->Width() > 0 && Texture->Height() > 0);
		glBindFramebuffer(GL_FRAMEBUFFER, FBOHandle);

		GLCHECK();

		switch(Attachment)
		{
		case ColorBufferAttachments::Depth:
			Attachment = GL_DEPTH_ATTACHMENT;
			break;
		case ColorBufferAttachments::Color:
			Attachment = GL_COLOR_ATTACHMENT0 + Attachments.size();
			break;
		case ColorBufferAttachments::Stencil:
			Attachment = GL_STENCIL_ATTACHMENT;
			break;
		};

		GLCHECK();

		glFramebufferTexture2D(GL_FRAMEBUFFER, Attachment, GL_TEXTURE_2D, Texture->ID(), 0);

		GLCHECK();

		uint32 _Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if(_Status == GL_FRAMEBUFFER_COMPLETE)
		{
			Attachments.push_back(Texture);

			return true;
		};

		GLCHECK();

		return false;
	};

	uint32 RenderTarget::Width()
	{
		return WidthValue;
	};

	uint32 RenderTarget::Height()
	{
		return HeightValue;
	};

	void RenderTarget::Activate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBOHandle);

		std::vector<GLenum> BufferIDs(Attachments.size());

		for(uint32 i = 0; i < Attachments.size(); i++)
		{
			BufferIDs[i] = GL_COLOR_ATTACHMENT0 + i;
		};

		glDrawBuffers(BufferIDs.size(), &BufferIDs[0]);
	};

	void RenderTarget::Deactivate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);

		for(uint32 i = 0; i < Attachments.size(); i++)
		{
			Attachments[i]->Bind();
			glGenerateMipmap(GL_TEXTURE_2D);
		};

		glBindTexture(GL_TEXTURE_2D, 0);
	};
#endif
};
