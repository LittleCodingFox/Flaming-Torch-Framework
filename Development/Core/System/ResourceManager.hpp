#pragma once

class ResourceManager : public SubSystem
{
private:
#if USE_GRAPHICS

	typedef std::map<StringID, FontHandle> FontMap;
#endif

	typedef std::map<StringID, DisposablePointer<Texture> > TextureMap;
	
	TextureMap Textures;

	typedef std::map<StringID, DisposablePointer<TexturePacker> > TexturePackerMap;

	TexturePackerMap TexturePackers;

#if USE_GRAPHICS
	FontMap Fonts;
#endif

	ResourceManager() : SubSystem(RESOURCEMANAGER_PRIORITY) {};

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
public:
	static ResourceManager Instance;

	static bool IsSameTexture(Texture *Self, Texture *Other);
	static DisposablePointer<Texture> InvalidTexture;

	DisposablePointer<Texture> GetTexture(const std::string &FileName);
	DisposablePointer<Texture> GetTexture(const Path &ThePath);
	DisposablePointer<Texture> GetTextureFromPackage(const std::string &Directory, const std::string &FileName);
	DisposablePointer<Texture> GetTextureFromPackage(const Path &ThePath);
	DisposablePointer<TexturePacker> GetTexturePack(const std::string &FileName, GenericConfig *Config);
	DisposablePointer<TexturePacker> GetTexturePack(const Path &ThePath, GenericConfig *Config);
	DisposablePointer<TexturePacker> GetTexturePackFromPackage(const std::string &Directory, const std::string &FileName, GenericConfig *Config);
	DisposablePointer<TexturePacker> GetTexturePackFromPackage(const Path &ThePath, GenericConfig *Config);

#if USE_GRAPHICS
	FontHandle GetFont(Renderer *TheRenderer, const std::string &FileName);
	FontHandle GetFont(Renderer *TheRenderer, const Path &ThePath);
	FontHandle GetFontFromPackage(Renderer *TheRenderer, const std::string &Directory, const std::string &FileName);
	FontHandle GetFontFromPackage(Renderer *TheRenderer, const Path &ThePath);

	static void DisposeFont(Renderer *TheRenderer, FontHandle Handle);
#endif

	template<typename type>
	inline static void DisposeResource(DisposablePointer<type> Resource)
	{
		Resource.Dispose();
	};

	void PrepareResourceReload();
	void ReloadResources();
	void Cleanup();
};
