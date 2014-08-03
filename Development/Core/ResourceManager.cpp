#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "ResourceManager"

	ResourceManager ResourceManager::Instance;

	SuperSmartPointer<Texture> ResourceManager::InvalidTexture;

#if USE_GRAPHICS
	SuperSmartPointer<sf::Font> ResourceManager::InvalidFont;
#endif

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

#if USE_GRAPHICS
		InvalidFont.Reset(new sf::Font());
#endif

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

#if USE_GRAPHICS
		while(Fonts.begin() != Fonts.end())
		{
			if(Fonts.begin()->second.Get())
				Fonts.begin()->second.Dispose();

			Fonts.erase(Fonts.begin());
		};

		RenderTextUtils::DefaultFont.Dispose();
#endif
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
	SuperSmartPointer<sf::Font> ResourceManager::GetFont(const std::string &FileName)
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr(TAG, "This Subsystem has not yet been initialized!");

			return SuperSmartPointer<sf::Font>();
		};

		StringID RealName = MakeStringID(FileName);
		FontMap::iterator it = Fonts.find(RealName);

		if(it == Fonts.end() || it->second.Get() == NULL)
		{
			Log::Instance.LogDebug(TAG, "Loading a font '%s' (H: 0x%08x)", FileName.c_str(), RealName);

			SuperSmartPointer<FontInfo> TheFont(new FontInfo());

			SuperSmartPointer<FileStream> TheStream(new FileStream());

			if(!TheStream->Open(FileName, StreamFlags::Read))
			{
				Log::Instance.LogErr(TAG, "Failed to load a font '%s' (H: 0x%08x)", FileName.c_str(), RealName);

				return SuperSmartPointer<sf::Font>();
			};

			TheFont->Data.resize((uint32)TheStream->Length());

			TheStream->AsBuffer(&TheFont->Data[0], (uint32)TheStream->Length());

			if(!TheFont->ActualFont->loadFromMemory(&TheFont->Data[0], TheFont->Data.size()))
			{
				Log::Instance.LogErr(TAG, "Failed to load a font '%s' (H: 0x%08x)", FileName.c_str(), RealName);

				return SuperSmartPointer<sf::Font>();
			};

			Fonts[RealName] = TheFont;

			return TheFont->ActualFont;
		};

		return it->second->ActualFont;
	};

	SuperSmartPointer<sf::Font> ResourceManager::GetFontFromPackage(const std::string &Directory, const std::string &FileName)
	{
		if(!WasStarted)
		{
			Log::Instance.LogErr(TAG, "This Subsystem has not yet been initialized!");

			return SuperSmartPointer<sf::Font>();
		};

		StringID RealName = MakeStringID("PACKAGE:" + Directory + "_" + FileName);
		FontMap::iterator it = Fonts.find(RealName);

		if(it == Fonts.end() || it->second.Get() == NULL)
		{
			Log::Instance.LogDebug(TAG, "Loading a font '%s%s' (H: 0x%08x)", Directory.c_str(), FileName.c_str(), RealName);

			SuperSmartPointer<FontInfo> TheFont(new FontInfo());

			SuperSmartPointer<Stream> TheStream = PackageFileSystemManager::Instance.GetFile(MakeStringID(Directory), MakeStringID(FileName));

			if(!TheStream.Get())
			{
				Log::Instance.LogErr(TAG, "Failed to load a font '%s%s' (H: 0x%08x)", Directory.c_str(), FileName.c_str(), RealName);

				return SuperSmartPointer<sf::Font>();
			};

			TheFont->Data.resize((uint32)TheStream->Length());

			TheStream->AsBuffer(&TheFont->Data[0], (uint32)TheStream->Length());

			if(!TheFont->ActualFont->loadFromMemory(&TheFont->Data[0], TheFont->Data.size()))
			{
				Log::Instance.LogErr(TAG, "Failed to load a font '%s%s' (H: 0x%08x)", Directory.c_str(), FileName.c_str(), RealName);

				return SuperSmartPointer<sf::Font>();
			};

			Fonts[RealName] = TheFont;

			return TheFont->ActualFont;
		};

		return it->second->ActualFont;
	};
#endif

	void ResourceManager::PrepareResourceReload()
	{
		for(TextureMap::iterator it = Textures.begin(); it != Textures.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.ObserverCount() == 1)
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
			while(it->second.Get() == NULL || it->second.ObserverCount() == 1)
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
			while(it->second.Get() == NULL || it->second.ObserverCount() == 1)
			{
				Textures.erase(it);
				it = Textures.begin();

				if(it == Textures.end())
					break;
			};

			if(it == Textures.end())
				break;
		};

#if USE_GRAPHICS
		for(FontMap::iterator it = Fonts.begin(); it != Fonts.end(); it++)
		{
			while(it->second.Get() == NULL || it->second->ActualFont.ObserverCount() == 1)
			{
				Fonts.erase(it);
				it = Fonts.begin();

				if(it == Fonts.end())
					break;
			};

			if(it == Fonts.end())
				break;
		};
#endif

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
