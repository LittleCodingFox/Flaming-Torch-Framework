#pragma once

class ResourceManager : public SubSystem
{
private:
#if USE_GRAPHICS

	typedef std::map<StringID, FontHandle> FontMap;
#endif

	typedef std::map<StringID, SuperSmartPointer<Texture> > TextureMap;
	
	TextureMap Textures;
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
	static SuperSmartPointer<Texture> InvalidTexture;

	SuperSmartPointer<Texture> GetTexture(const std::string &FileName);
	SuperSmartPointer<Texture> GetTextureFromPackage(const std::string &Directory, const std::string &FileName);

#if USE_GRAPHICS
	FontHandle GetFont(Renderer *TheRenderer, const std::string &FileName);
	FontHandle GetFontFromPackage(Renderer *TheRenderer, const std::string &Directory, const std::string &FileName);
#endif

	void PrepareResourceReload();
	void ReloadResources();
	void Cleanup();
};
