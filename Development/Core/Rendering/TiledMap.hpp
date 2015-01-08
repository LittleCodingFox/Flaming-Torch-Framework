#pragma once

/*!
*	Tiled Map Orientation Modes
*/
namespace TiledMapOrientationMode
{
	enum TiledMapOrientationMode
	{
		Orthogonal, //!<Orthogonal Orientation
		Isometric //!<Isometric Orientation
	};
}

/*!
*	Tiled Map Order
*/
namespace TiledMapOrder
{
	enum
	{
		North = 0, //!<Default
		East, //!<Rotated 45 degrees to the right
		South, //!<Rotated 180 degrees
		West //!<Rotated 270 degrees
	};
}

/*!
*	Tiled Map Init Options class
*/
class TiledMapInitOptions
{
public:
	std::vector<std::string> PackageDirectoriesValue;
	bool FromPackageValue;

	TiledMapInitOptions() : FromPackageValue(false) {}
	TiledMapInitOptions &FromPackage(bool value) { FromPackageValue = value; return *this; }
	TiledMapInitOptions &AdditionalPackageDirectory(const std::string &Directory)
	{ 
		PackageDirectoriesValue.push_back(Directory); return *this;
	}
};

/*!
*	Tiled Map class
*/
class TiledMap
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
		SimpleDelegate::SimpleDelegate<TiledMap *, uint32, Renderer *> OnLayerDraw;
#endif
	};

	std::vector<DisposablePointer<Layer> > Layers;

	class UniqueTilesetInfo
	{
	public:
		typedef std::map<uint8, std::map<std::string, std::string> > PropertyMap;
		PropertyMap Properties;

#if USE_GRAPHICS
		std::string UniqueTilesetTextureName;
		DisposablePointer<Texture> UniqueTilesetTexture;
		DisposablePointer<TexturePacker> UniqueTilesetTexturePacker;
		GenericConfig UniqueTilesetConfig;
#endif
	}TileSet;

	class MapObject
	{
	public:
		Vector2 Position, Size;
		std::string Name, Type;
		std::vector<Vector2> PolygonData;
		uint8 IsPolyLine;
		f32 Rotation;
		std::map<std::string, std::string> Properties;
	};

	std::vector<MapObject> Objects;
	
	Vector2 MapTileSize, MapPixelSize, MapTileCount, Translation;
	Vector3 Color;

	/*!
	*	Scaling is applied before translation
	*/
	Vector2 Scale;

	/*!
	*	One of TiledMapOrientationMode::*
	*/
	uint32 Orientation;

	/*!
	*	Isometric Tile Ratio
	*/
	f32 TileRatio;

	/*!
	*	Tile Order (one of TiledMapOrder::*)
	*/
	uint32 TileOrder;

	TiledMap();

	bool DeSerialize(Stream *In);
#if USE_GRAPHICS
	bool InitResources(const TiledMapInitOptions &Options = TiledMapInitOptions());
	bool ResourcesInited();
	void Draw(uint32 Layer, Renderer *Renderer);
	void UpdateGeometry();
#endif

	/*!
	*	Converts an Ortho point to Iso
	*	\param Point the Ortho point
	*	\return the Iso point
	*/
	Vector2 ToIsometric(const Vector2 &Point);

	/*!
	*	Converts an Iso point to Ortho
	*	\param Point the Iso point
	*	\return the Ortho point
	*/
	Vector2 ToOrthogonal(const Vector2 &Point);
};
