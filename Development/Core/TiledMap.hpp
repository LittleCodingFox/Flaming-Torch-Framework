#pragma once

/*!
*	Tiled Map Init Options class
*/
class FLAMING_API TiledMapInitOptions
{
public:
	std::vector<std::string> PackageDirectoriesValue;
	bool FromPackageValue;

	TiledMapInitOptions() : FromPackageValue(false) {};
	TiledMapInitOptions &FromPackage(bool value) { FromPackageValue = value; return *this; };
	TiledMapInitOptions &AdditionalPackageDirectory(const std::string &Directory)
	{ 
		PackageDirectoriesValue.push_back(Directory); return *this;
	};
};

/*!
*	Tiled Map class
*/
class FLAMING_API TiledMap
{
private:
	bool ResourcesInitedValue;
public:
	class Layer
	{
	public:
		/*!
		*	Whether this layer is visible
		*/
		bool Visible;
		/*!
		*	Layer Tranparency
		*/
		f32 Alpha;

		struct TileInfo
		{
			//Tile ID
			uint8 ID;
			//Tile Flip Flag
			uint8 FlipFlag;
			//Position of the tile
			Vector2 Position;
		};

		std::vector<TileInfo> Tiles;

#if USE_GRAPHICS
		std::vector<Vector2> Vertices, TexCoords;
		Signal3<TiledMap *, uint32, RendererManager::Renderer *> OnLayerDraw;
#endif
	};

	std::vector<SuperSmartPointer<Layer> > Layers;

	class UniqueTilesetInfo
	{
	public:
		typedef std::map<uint8, std::map<std::string, std::string> > PropertyMap;
		PropertyMap Properties;

#if USE_GRAPHICS
		std::string UniqueTilesetTextureName;
		SuperSmartPointer<Texture> UniqueTilesetTexture;
#endif
	}TileSet;

	class MapObject
	{
	public:
		Vector2 Position, Size;
		std::string Name, Type;
		std::vector<Vector2> PolygonData;
		uint8 IsPolyLine;
		std::map<std::string, std::string> Properties;
	};

	std::vector<MapObject> Objects;
	
	Vector2 MapTileSize, MapPixelSize, MapTileCount, Translation;
	Vector3 Color;

	/*!
	*	Scaling is applied before translation
	*/
	Vector2 Scale;

	TiledMap();

	bool DeSerialize(Stream *In);
#if USE_GRAPHICS
	bool InitResources(const TiledMapInitOptions &Options = TiledMapInitOptions());
	bool ResourcesInited();
	void Draw(uint32 Layer, RendererManager::Renderer *Renderer);
	void UpdateGeometry();
#endif
};
