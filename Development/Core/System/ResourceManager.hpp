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

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
public:

	ResourceManager() : SubSystem(RESOURCEMANAGER_PRIORITY) {}

	DisposablePointer<Texture> GetTexture(const Path &ThePath);
	DisposablePointer<TexturePacker> GetTexturePack(const Path &ThePath, GenericConfig *Config);

#if USE_GRAPHICS
	DisposablePointer<Font> GetFont(const Path &ThePath);
#endif

	void PrepareResourceReload();
	void ReloadResources();
	void Cleanup();
};

extern ResourceManager g_ResourceManager;

