#pragma once

class ResourceManager : public SubSystem
{
private:
	typedef std::map<StringID, DisposablePointer<Font>> FontMap;

	typedef std::map<StringID, DisposablePointer<Texture> > TextureMap;
	
	TextureMap Textures;

	typedef std::map<StringID, DisposablePointer<TexturePacker> > TexturePackerMap;

	TexturePackerMap TexturePackers;

	FontMap Fonts;

	ResourceManager() : SubSystem(RESOURCEMANAGER_PRIORITY) {}

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
	DisposablePointer<Font> GetFont(Renderer *TheRenderer, const std::string &FileName);
	DisposablePointer<Font> GetFont(Renderer *TheRenderer, const Path &ThePath);
	DisposablePointer<Font> GetFontFromPackage(Renderer *TheRenderer, const std::string &Directory, const std::string &FileName);
	DisposablePointer<Font> GetFontFromPackage(Renderer *TheRenderer, const Path &ThePath);
#endif

	template<typename type>
	inline static void DisposeResource(DisposablePointer<type> Resource)
	{
		Resource.Dispose();
	}

	void PrepareResourceReload();
	void ReloadResources();
	void Cleanup();
};
