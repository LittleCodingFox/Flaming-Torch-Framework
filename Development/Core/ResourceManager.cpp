#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "ResourceManager"

	ResourceManager ResourceManager::Instance;

	SuperSmartPointer<Texture> ResourceManager::InvalidTexture;

	bool ResourceManager::IsSameTexture(Texture *Self, Texture *Other)
	{
		return Self == Other;
	};

	void ResourceManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "Starting Resource Manager Subsystem");

		InvalidTexture.Reset(new Texture());
	};

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
		};
	};

	void ResourceManager::Update(uint32 Priority)
	{
		if(Priority != RESOURCEMANAGER_PRIORITY)
			return;

		SUBSYSTEM_PRIORITY_CHECK();

		Cleanup();
	};

	SuperSmartPointer<Texture> ResourceManager::GetTexture(const std::string &FileName)
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr(TAG, "This Subsystem has not yet been initialized!");

			return SuperSmartPointer<Texture>();
		};

		StringID RealName = MakeStringID(FileName);
		TextureMap::iterator it =
			Textures.find(RealName);

		if(it == Textures.end() || it->second.Get() == NULL)
		{
			Log::Instance.LogDebug(TAG, "Loading Texture '%s' (H: '0x%08x').", FileName.c_str(),
				RealName);

			SuperSmartPointer<Texture> _Texture(new Texture());

			if(!_Texture->FromFile(FileName.c_str()))
			{
				Log::Instance.LogErr(TAG, "Failed to load texture '%s' (H: '0x%08x').", FileName.c_str(), RealName);

				return SuperSmartPointer<Texture>();
			};

			Textures[RealName] = _Texture;

			return _Texture;
		};

		return it->second;
	};

	SuperSmartPointer<Texture> ResourceManager::GetTextureFromPackage(const std::string &Directory, const std::string &FileName)
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr(TAG, "This Subsystem has not yet been initialized!");

			return SuperSmartPointer<Texture>();
		};

		StringID RealName = MakeStringID("PACKAGE:" + Directory + "_" + FileName);
		TextureMap::iterator it =
			Textures.find(RealName);

		if(it == Textures.end() || it->second.Get() == NULL)
		{
			Log::Instance.LogDebug(TAG, "Loading Texture from '%s%s' (H: '0x%08x').", Directory.c_str(),
				FileName.c_str(), RealName);

			SuperSmartPointer<Texture> _Texture(new Texture());

			if(!_Texture->FromPackage(Directory, FileName))
			{
				Log::Instance.LogErr(TAG, "Failed to load texture '%s%s' (H: '0x%08x').", Directory.c_str(),
					FileName.c_str(), RealName);

				return SuperSmartPointer<Texture>();
			};

			Textures[RealName] = _Texture;

			return _Texture;
		};

		return it->second;
	};

#if USE_GRAPHICS
	FontHandle ResourceManager::GetFont(Renderer *TheRenderer, const std::string &FileName)
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr(TAG, "This Subsystem has not yet been initialized!");

			return 0;
		};

		StringID RealName = MakeStringID(FileName);
		FontMap::iterator it = Fonts.find(RealName);

		if(it == Fonts.end() || it->second == 0)
		{
			Log::Instance.LogDebug(TAG, "Loading a font '%s' (H: 0x%08x)", FileName.c_str(), RealName);

			SuperSmartPointer<FileStream> TheStream(new FileStream());

			if(!TheStream->Open(FileName, StreamFlags::Read))
			{
				Log::Instance.LogErr(TAG, "Failed to load a font '%s' (H: 0x%08x)", FileName.c_str(), RealName);

				return 0;
			};

			FontHandle Out = TheRenderer->CreateFont(TheStream);

			if(Out == 0)
			{
				Log::Instance.LogErr(TAG, "Failed to load a font '%s' (H: 0x%08x)", FileName.c_str(), RealName);

				return 0;
			};

			Fonts[RealName] = Out;

			return Out;
		};

		return it->second;
	};

	FontHandle ResourceManager::GetFontFromPackage(Renderer *TheRenderer, const std::string &Directory, const std::string &FileName)
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr(TAG, "This Subsystem has not yet been initialized!");

			return 0;
		};

		StringID RealName = MakeStringID("PACKAGE:" + Directory + "_" + FileName);
		FontMap::iterator it = Fonts.find(RealName);

		if(it == Fonts.end() || it->second == 0)
		{
			Log::Instance.LogDebug(TAG, "Loading a font '%s%s' (H: 0x%08x)", Directory.c_str(), FileName.c_str(), RealName);

			SuperSmartPointer<Stream> TheStream = PackageFileSystemManager::Instance.GetFile(MakeStringID(Directory), MakeStringID(FileName));

			if(!TheStream.Get())
			{
				Log::Instance.LogErr(TAG, "Failed to load a font '%s%s' (H: 0x%08x)", Directory.c_str(), FileName.c_str(), RealName);

				return 0;
			};

			FontHandle Out = TheRenderer->CreateFont(TheStream);

			if(Out == 0)
			{
				Log::Instance.LogErr(TAG, "Failed to load a font '%s%s' (H: 0x%08x)", Directory.c_str(), FileName.c_str(), RealName);

				return 0;
			};

			Fonts[RealName] = Out;

			return Out;
		};

		return it->second;
	};
#endif

	void ResourceManager::PrepareResourceReload()
	{
		for(TextureMap::iterator it = Textures.begin(); it != Textures.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.use_count() == 1)
			{
				Textures.erase(it);
				it = Textures.begin();

				if(it == Textures.end())
					break;
			};

			it->second->Destroy();
		};
	};

	void ResourceManager::ReloadResources()
	{
		for(TextureMap::iterator it = Textures.begin(); it != Textures.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.use_count() == 1)
			{
				Textures.erase(it);
				it = Textures.begin();

				if(it == Textures.end())
					break;

				continue;
			};

			uint32 TextureFilter = it->second->FilterMode();

			if(it->second->GetData().Get() && it->second->GetData()->Width() != 0 && it->second->GetData()->Height() != 0)
			{
				SuperSmartPointer<TextureBuffer> Buffer = it->second->GetData();

				it->second->FromData(&Buffer->Data[0], it->second->Width(), it->second->Height());
			}
			else
			{
				it->second->CreateEmptyTexture(it->second->Width(), it->second->Height(),
					it->second->ColorType() == ColorType::RGBA8);
			};

			it->second->SetTextureFiltering(TextureFilter);
		};
	};

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
			while(it->second.Get() == NULL || it->second.use_count() == 1)
			{
				Textures.erase(it);
				it = Textures.begin();

				if(it == Textures.end())
					break;
			};

			if(it == Textures.end())
				break;
		};

		uint32 CurrentObjects = Textures.size() +
#if USE_GRAPHICS
			Fonts.size();
#else
			0;
#endif

		if(CurrentObjects != TotalObjects)
		{
			Log::Instance.LogDebug(TAG, "Cleared %d objects (Prev: %d/Now: %d)", TotalObjects - CurrentObjects, TotalObjects, CurrentObjects);
		};
	};
};
