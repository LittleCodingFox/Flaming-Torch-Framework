#pragma once

class ResourceManager : public SubSystem
{
private:
#if USE_GRAPHICS
	class FontInfo
	{
	public:
		SuperSmartPointer<sf::Font> ActualFont;
		std::vector<uint8> Data;

		FontInfo() : ActualFont(new sf::Font()) {};
		~FontInfo() { ActualFont.Dispose(); };
	};

	typedef std::map<StringID, SuperSmartPointer<FontInfo> > FontMap;
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
#if USE_GRAPHICS
	static SuperSmartPointer<sf::Font> InvalidFont;
#endif

	SuperSmartPointer<Texture> GetTexture(const std::string &FileName);
	SuperSmartPointer<Texture> GetTextureFromPackage(const std::string &Directory, const std::string &FileName);

#if USE_GRAPHICS
	SuperSmartPointer<sf::Font> GetFont(const std::string &FileName);
	SuperSmartPointer<sf::Font> GetFontFromPackage(const std::string &Directory, const std::string &FileName);
#endif

	void PrepareResourceReload();
	void ReloadResources();
	void Cleanup();
};
