#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "ResourceManager"

	ResourceManager ResourceManager::Instance;

	DisposablePointer<Texture> ResourceManager::InvalidTexture;

	bool ResourceManager::IsSameTexture(Texture *Self, Texture *Other)
	{
		return Self == Other;
	}

	void ResourceManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Starting Resource Manager Subsystem");

		InvalidTexture.Reset(new Texture());
	}

	void ResourceManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Terminating Resource Manager Subsystem");

		SubSystem::Shutdown(Priority);

		while(Textures.begin() != Textures.end())
		{
			if(Textures.begin()->second.Get())
				Textures.begin()->second.Dispose();

			Textures.erase(Textures.begin());
		}
	}

	void ResourceManager::Update(uint32 Priority)
	{
		if(Priority != RESOURCEMANAGER_PRIORITY)
			return;

		SUBSYSTEM_PRIORITY_CHECK();

		Cleanup();
	}

	DisposablePointer<Texture> ResourceManager::GetTexture(const Path &ThePath)
	{
		if (!WasStarted)
		{
			Log::Instance.LogErr(TAG, "This Subsystem has not yet been initialized!");

			return DisposablePointer<Texture>();
		}

		std::string FileName = ThePath.FullPath();
		StringID RealName = MakeStringID(FileName);
		TextureMap::iterator it = Textures.find(RealName);

		if (it == Textures.end() || it->second.Get() == NULL)
		{
			Log::Instance.LogDebug(TAG, "Loading Texture '%s' (H: '0x%08x').", FileName.c_str(),
				RealName);

			DisposablePointer<Texture> _Texture = Texture::CreateFromStream(PackageFileSystemManager::Instance.GetFile(ThePath));

			if (!_Texture)
			{
				_Texture = Texture::CreateFromStream(FileStream::FromPath(ThePath, StreamFlags::Read));

				if (!_Texture)
				{
					Log::Instance.LogErr(TAG, "Failed to load texture '%s' (H: '0x%08x').", FileName.c_str(), RealName);

					return DisposablePointer<Texture>();
				}
			}

			Textures[RealName] = _Texture;

			return _Texture;
		}

		return it->second;
	}

	DisposablePointer<TexturePacker> ResourceManager::GetTexturePack(const Path &ThePath, GenericConfig *Config)
	{
		if (!WasStarted)
		{
			Log::Instance.LogErr(TAG, "This Subsystem has not yet been initialized!");

			return DisposablePointer<TexturePacker>();
		}

		if (!Config)
			return DisposablePointer<TexturePacker>();

		StringID RealName = MakeStringID(ThePath.FullPath() + "_" + StringUtils::MakeIntString(Config->CRC()));
		TexturePackerMap::iterator it = TexturePackers.find(RealName);

		if (it == TexturePackers.end() || it->second.Get() == NULL)
		{
			Log::Instance.LogDebug(TAG, "Loading Texture Packer '%s' (H: '0x%08x').", ThePath.FullPath().c_str(),
				RealName);

			DisposablePointer<Texture> In = GetTexture(ThePath);

			if (!In)
				return DisposablePointer<TexturePacker>();

			DisposablePointer<TexturePacker> Packer = TexturePacker::FromConfig(In, *Config);

			if (!Packer.Get())
			{
				Log::Instance.LogErr(TAG, "Failed to load texture packer '%s' (H: '0x%08x').", ThePath.FullPath().c_str(), RealName);

				return DisposablePointer<TexturePacker>();
			}

			TexturePackers[RealName] = Packer;

			return Packer;
		}

		return it->second;
	}

#if USE_GRAPHICS
	DisposablePointer<Font> ResourceManager::GetFont(Renderer *TheRenderer, const Path &ThePath)
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr(TAG, "This Subsystem has not yet been initialized!");

			return DisposablePointer<Font>();
		}

		StringID RealName = MakeStringID(ThePath.FullPath());
		FontMap::iterator it = Fonts.find(RealName);

		if(it == Fonts.end() || it->second.Get() == NULL)
		{
			Log::Instance.LogDebug(TAG, "Loading a font '%s' (H: 0x%08x)", ThePath.FullPath().c_str(), RealName);

			DisposablePointer<FileStream> TheStream = PackageFileSystemManager::Instance.GetFile(ThePath);

			if (!TheStream.Get())
			{
				TheStream = FileStream::FromPath(ThePath, StreamFlags::Read);

				if (!TheStream.Get())
				{
					Log::Instance.LogErr(TAG, "Failed to load a font '%s' (H: 0x%08x)", ThePath.FullPath().c_str(), RealName);

					return DisposablePointer<Font>();
				}
			}

			DisposablePointer<Font> Out(new Font());

			if(!Out->FromStream(TheStream))
			{
				Log::Instance.LogErr(TAG, "Failed to load a font '%s' (H: 0x%08x)", ThePath.FullPath().c_str(), RealName);

				return DisposablePointer<Font>();
			}

			Fonts[RealName] = Out;

			return Out;
		}

		return it->second;
	}
#endif

	void ResourceManager::PrepareResourceReload()
	{
		for(TextureMap::iterator it = Textures.begin(); it != Textures.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.UseCount() == 1)
			{
				Textures.erase(it);
				it = Textures.begin();

				if(it == Textures.end())
					return;
			}

			it->second->Destroy();
		}
	}

	void ResourceManager::ReloadResources()
	{
		for(TextureMap::iterator it = Textures.begin(); it != Textures.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.UseCount() == 1)
			{
				Textures.erase(it);
				it = Textures.begin();

				if(it == Textures.end())
					return;
			}

			uint32 TextureFilter = it->second->FilterMode();

			if(it->second->GetData().Get() && it->second->GetData()->Width() != 0 && it->second->GetData()->Height() != 0)
			{
				DisposablePointer<TextureBuffer> Buffer = it->second->GetData();

				it->second->FromData(&Buffer->Data[0], it->second->Width(), it->second->Height());
			}
			else
			{
				it->second->CreateEmptyTexture(it->second->Width(), it->second->Height(),
					it->second->ColorType() == ColorType::RGBA8);
			}

			it->second->SetTextureFiltering(TextureFilter);
		}

#if USE_GRAPHICS
		Renderer *TheRenderer = RendererManager::Instance.ActiveRenderer();

		TheRenderer->OnResourcesReloaded(TheRenderer);
#endif
	}

	void ResourceManager::Cleanup()
	{
		uint32 TotalObjects = Textures.size() +
#if USE_GRAPHICS
			Fonts.size();
#else
			0;
#endif

		for(TextureMap::iterator it = Textures.begin(); it != Textures.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.UseCount() == 1)
			{
				Textures.erase(it);
				it = Textures.begin();

				if(it == Textures.end())
					break;
			}

			if (it == Textures.end())
				break;
		}

		for (TexturePackerMap::iterator it = TexturePackers.begin(); it != TexturePackers.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.UseCount() == 1)
			{
				TexturePackers.erase(it);
				it = TexturePackers.begin();

				if(it == TexturePackers.end())
					break;
			}

			if (it == TexturePackers.end())
				break;
		}

		uint32 CurrentObjects = Textures.size() + TexturePackers.size() + Fonts.size();

		if(CurrentObjects != TotalObjects)
		{
			Log::Instance.LogDebug(TAG, "Cleared %d objects (Prev: %d/Now: %d)", TotalObjects - CurrentObjects, TotalObjects, CurrentObjects);
		}
	}
}
