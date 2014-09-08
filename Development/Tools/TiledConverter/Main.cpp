#include "FlamingCore.hpp"
#include <Tmx.h>
#include <vector>
#include <map>
#include <sstream>
#include <stdio.h>
#include <SFML/System.hpp>

using namespace FlamingTorch;

#define VERSION_MAJOR 0
#define VERSION_MINOR 5
#define TAG "TiledConverter"

uint32 MapSplitSize = 2048;
bool SaveTexturesAsFTI = false;

std::string FLGameName()
{
	return "TiledConverter";
};

namespace TileFlipFlags
{
	enum
	{
		None = FLAGVALUE(0),
		Horizontal = FLAGVALUE(1),
		Vertical = FLAGVALUE(2),
		Diagonal = FLAGVALUE(3)
	};
};

class MapData
{
public:
	struct TileData
	{
		std::string FileName;
		Vector3 TransparentColor;
		SuperSmartPointer<TextureBuffer> Image;
		Vector2 TileSize;
		Vector2 FrameCount;
		void *Ptr;

		typedef std::map<uint32, std::map<std::string, std::string> > TilePropertyMap;
		TilePropertyMap Properties;
	};

	std::map<uint32, SuperSmartPointer<TileData> > TileSets;

	struct TileInfo
	{
		uint32 ID;
		uint32 FlipFlag;
		Vector2 Position;
	};
	
	//Tileset Index -> Tile ID
	typedef std::map<uint32, std::vector<TileInfo> > TileMap;

	struct LayerData
	{
		bool Visible;
		TileMap Tiles;
	};

	std::vector<SuperSmartPointer<LayerData> > Layers;

	struct MapObject
	{
		Vector2 Position, Size;
		f32 Rotation;
		std::string Name, Type;
		std::vector<Vector2> PolygonData;
		uint8 IsPolyLine;
		std::map<std::string, std::string> Properties;
	};

	std::vector<MapObject> MapObjects;

	struct MapHeader
	{
		uint32 ID; //'F' 'T' 'T' 'M'
		VersionType Version;
		uint32 LayerCount, ObjectCount;
		Vector2 MapTileSize;
		Vector2 MapPixelSize;
	};

	int32 GetTileSetIndex(uint32 Index)
	{
		int32 Count = 0;

		for(std::map<uint32, SuperSmartPointer<TileData> >::iterator it = TileSets.begin(); it != TileSets.end(); it++, Count++)
		{
			if(it->first == Index)
				return Count;
		};

		return 0xFFFFFFFF;
	};

	int32 GetTileSetFromCount(uint32 Count)
	{
		for(std::map<uint32, SuperSmartPointer<TileData> >::iterator it = TileSets.begin(); it != TileSets.end(); it++, Count--)
		{
			if(Count == 0)
				return it->first;
		};

		return 0xFFFFFF;
	};

	Vector2 MapTileSize, MapPixelSize;

	struct TileCacheInfo
	{
		Vector3 ID;
		TextureBuffer Data;
	};

	void Finalize(const char *OutFileName)
	{
		Log::Instance.LogInfo(TAG, "Finalizing!");

		FileStream Stream;

		if(!Stream.Open(OutFileName, StreamFlags::Write))
		{
			Log::Instance.LogErr(TAG, "Unable to open '%s' for writing!", OutFileName);

			return;
		};

		MapHeader Header;
		Header.ID = CoreUtils::MakeIntFromBytes('F', 'T', 'T', 'M');
		Header.Version = CoreUtils::MakeVersion(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR);
		Header.LayerCount = Layers.size();
		Header.MapPixelSize = MapPixelSize * MapTileSize;
		Header.MapTileSize = MapTileSize;
		Header.ObjectCount = MapObjects.size();

		SVOIDFLASSERT(Stream.Write2<MapHeader>(&Header));

		//1st: Image Filenames
		if(Layers.size())
		{
			//Unique Tileset generation
			std::vector<TextureBuffer> Textures(Layers.size());
			std::vector<TileCacheInfo> UniqueTiles;

			for(uint32 i = 0; i < Layers.size(); i++)
			{
				for(TileMap::iterator it = Layers[i]->Tiles.begin(); it != Layers[i]->Tiles.end(); it++)
				{
					TileData &TheTileSet = *TileSets[it->first].Get();
					TextureBuffer &TileSet = *TheTileSet.Image.Get();
					uint32 TileSetRowWidth = TileSet.Width() * 4;
					uint32 TileWidth = (uint32)TheTileSet.TileSize.x * 4;
					uint32 TileSetTileMultiplier = (uint32)TheTileSet.TileSize.y * TileSetRowWidth;

					for(uint32 j = 0; j < it->second.size(); j++)
					{
						Vector3 TileID = Vector3((f32)it->first, (f32)(it->second[j].ID % (uint32)TheTileSet.FrameCount.x),
							it->second[j].ID / TheTileSet.FrameCount.x);

						bool Found = false;

						for(uint32 k = 0; k < UniqueTiles.size(); k++)
						{
							if(UniqueTiles[k].ID == TileID)
							{
								Found = true;

								break;
							};
						};

						if(!Found)
						{
							UniqueTiles.resize(UniqueTiles.size() + 1);

							TileCacheInfo &Info = UniqueTiles[UniqueTiles.size() - 1];

							Info.ID = TileID;
							Info.Data.CreateEmpty((uint32)TheTileSet.TileSize.x, (uint32)TheTileSet.TileSize.y);
							Vector2 Position((f32)(it->second[j].ID % (uint32)TheTileSet.FrameCount.x), 
								(f32)(it->second[j].ID / (uint32)TheTileSet.FrameCount.x));
							uint32 TileSetStartIndex = (uint32)(Position.x * TileWidth + Position.y * TileSetTileMultiplier);

							for(uint32 y = 0, TileStartIndex = 0; y < (uint32)TheTileSet.TileSize.y; y++, TileSetStartIndex += TileSetRowWidth,
								TileStartIndex += TileWidth)
							{
								memcpy(&Info.Data.Data[TileStartIndex], &TileSet.Data[TileSetStartIndex], TileWidth);
							};
						};
					};
				};
			};

            if(UniqueTiles.size() != 0)
            {
                Vector2 MaxImageSize((f32)MapSplitSize, (f32)MapSplitSize);
                bool Done = false;
                
                TextureBuffer UniqueTilesBuffer;
                uint32 UniqueTileSideCount = (uint32)sqrt(UniqueTiles.size());
                
                while(UniqueTileSideCount * UniqueTileSideCount < UniqueTiles.size())
                {
                    UniqueTileSideCount++;
                };
                
                Vector2 ImageSize(MapTileSize * (f32)UniqueTileSideCount);
                
                if(ImageSize.x > MaxImageSize.x || ImageSize.y > MaxImageSize.y)
                {
                    Log::Instance.LogErr(TAG, "Unable to handle Unique Tiles Textures bigger than %dx%d, quitting...", MapSplitSize, MapSplitSize);
                    
                    return;
                };
                
                UniqueTilesBuffer.CreateEmpty((uint32)ImageSize.x, (uint32)ImageSize.y);
                
                TextureEncoderInfo TInfo;
                TInfo.Encoder = TextureEncoderType::PNG;
                TInfo.Quality = 100;
                TInfo.Lossless = true;
                
                for(uint32 i = 0; i < UniqueTiles.size(); i++)
                {
                    uint32 UniqueTilesRowWidth = UniqueTiles[i].Data.Width() * 4;
                    uint32 UniqueTilesBufferRowWidth = UniqueTilesBuffer.Width() * 4;
                    Vector2 StartXY = MapTileSize * Vector2((f32)(i % UniqueTileSideCount), (f32)(i / UniqueTileSideCount));
                    uint32 Start = (uint32)(StartXY.x * 4 + StartXY.y * UniqueTilesBufferRowWidth);
                    
                    for(uint32 y = 0, SourceIndex = 0; y < MapTileSize.y; y++, SourceIndex += UniqueTilesRowWidth, Start += UniqueTilesBufferRowWidth)
                    {
                        memcpy(&UniqueTilesBuffer.Data[Start], &UniqueTiles[i].Data.Data[SourceIndex], sizeof(uint8) * UniqueTilesRowWidth);
                    };
                };
                
                FileStream Out;
                
                std::stringstream str;
                str << OutFileName << "_UniqueTiles.png";
                
                if(!Out.Open(str.str(), StreamFlags::Write) || !UniqueTilesBuffer.Save(&Out, TInfo))
                {
                    Log::Instance.LogErr(TAG, "Unable to save Unique Tiles Texture");
                    
                    return;
                };
                
                std::string ActualFileName = str.str();
                
                int32 Index = ActualFileName.rfind('/');
                
                if(Index != std::string::npos)
                {
                    ActualFileName = ActualFileName.substr(Index + 1);
                };
                
                Index = ActualFileName.rfind('\\');
                
                if(Index != std::string::npos)
                {
                    ActualFileName = ActualFileName.substr(Index + 1);
                };
                
                uint16 Length = ActualFileName.length();
                
                SVOIDFLASSERT(Stream.Write2<uint16>(&Length));
                SVOIDFLASSERT(Stream.Write2<char>(ActualFileName.c_str(), Length));
            }
            else
            {
                uint16 Empty = 0;

                SVOIDFLASSERT(Stream.Write2<uint16>(&Empty));
            };
            
            uint16 Length = 0;

			uint8 UniqueTileCount = UniqueTiles.size();
			SVOIDFLASSERT(Stream.Write2<uint8>(&UniqueTileCount));

			//Tile Properties
			for(uint32 i = 0; i < UniqueTiles.size(); i++)
			{
				uint8 TileSetID = (uint8)UniqueTiles[i].ID.x;
				uint8 FrameID = (uint8)(UniqueTiles[i].ID.y + UniqueTiles[i].ID.z * TileSets[TileSetID]->FrameCount.x);

				uint8 PropertyCount = TileSets[TileSetID]->Properties[FrameID].size();
				SVOIDFLASSERT(Stream.Write2<uint8>(&PropertyCount));

				for(std::map<std::string, std::string>::iterator pit = TileSets[TileSetID]->Properties[FrameID].begin(); pit !=
					TileSets[TileSetID]->Properties[FrameID].end(); pit++)
				{
					Length = pit->first.length();

					SVOIDFLASSERT(Stream.Write2<uint16>(&Length));
					SVOIDFLASSERT(Stream.Write2<char>(pit->first.c_str(), Length));

					Length = pit->second.length();

					SVOIDFLASSERT(Stream.Write2<uint16>(&Length));
					SVOIDFLASSERT(Stream.Write2<char>(pit->second.c_str(), Length));
				};
			};

			//Layer Tiles
			for(uint32 i = 0; i < Layers.size(); i++)
			{
				SVOIDFLASSERT(Stream.Write2<bool>(&Layers[i]->Visible));
				uint8 TileCount = Layers[i]->Tiles.size();
				SVOIDFLASSERT(Stream.Write2<uint8>(&TileCount));

				for(TileMap::iterator it = Layers[i]->Tiles.begin(); it != Layers[i]->Tiles.end(); it++)
				{
					TileData &TheTileSet = *TileSets[it->first].Get();
					uint16 TinyCount = it->second.size();
					SVOIDFLASSERT(Stream.Write2<uint16>(&TinyCount));

					for(uint32 j = 0; j < it->second.size(); j++)
					{
						Vector3 TileID = Vector3((f32)it->first, (f32)(it->second[j].ID % (uint32)TheTileSet.FrameCount.x),
							(f32)(it->second[j].ID / TheTileSet.FrameCount.x));

						for(uint32 k = 0; k < UniqueTiles.size(); k++)
						{
							if(UniqueTiles[k].ID == TileID)
							{
								uint8 TinyID = (uint8)k;
								uint8 TinyCoords[2] = {
									(uint8)it->second[j].Position.x,
									(uint8)it->second[j].Position.y
								};

								uint8 TinyFlipFlags = (uint8)it->second[j].FlipFlag;

								SVOIDFLASSERT(Stream.Write2<uint8>(&TinyID));
								SVOIDFLASSERT(Stream.Write2<uint8>(TinyCoords, 2));
								SVOIDFLASSERT(Stream.Write2<uint8>(&TinyFlipFlags));

								break;
							};
						};
					};
				};
			};
		}
		else
		{
			uint16 Empty = 0;

			SVOIDFLASSERT(Stream.Write2<uint16>(&Empty));

			uint8 EmptyByte = 0;

			SVOIDFLASSERT(Stream.Write2<uint8>(&EmptyByte));
		};

		//Map Objects
		for(uint32 i = 0; i < MapObjects.size(); i++)
		{
			uint16 Length = MapObjects[i].Name.size();
			SVOIDFLASSERT(Stream.Write2<uint16>(&Length));

			if(Length)
			{
				SVOIDFLASSERT(Stream.Write2<char>(MapObjects[i].Name.c_str(), Length));
			};

			Length = MapObjects[i].Type.size();
			SVOIDFLASSERT(Stream.Write2<uint16>(&Length));

			if(Length)
			{
				SVOIDFLASSERT(Stream.Write2<char>(MapObjects[i].Type.c_str(), Length));
			};

			SVOIDFLASSERT(Stream.Write2<Vector2>(&MapObjects[i].Position));
			SVOIDFLASSERT(Stream.Write2<Vector2>(&MapObjects[i].Size));
			SVOIDFLASSERT(Stream.Write2<f32>(&MapObjects[i].Rotation));
			SVOIDFLASSERT(Stream.Write2<uint8>(&MapObjects[i].IsPolyLine));

			Length = MapObjects[i].PolygonData.size();
			SVOIDFLASSERT(Stream.Write2<uint16>(&Length));

			if(Length)
			{
				SVOIDFLASSERT(Stream.Write2<Vector2>(&MapObjects[i].PolygonData[0], Length));
			};

			Length = MapObjects[i].Properties.size();
			SVOIDFLASSERT(Stream.Write2<uint16>(&Length));

			for(std::map<std::string, std::string>::iterator it = MapObjects[i].Properties.begin();
				it != MapObjects[i].Properties.end(); it++)
			{
				Length = it->first.length();
				SVOIDFLASSERT(Stream.Write2<uint16>(&Length));

				if(Length)
				{
					SVOIDFLASSERT(Stream.Write2<char>(it->first.c_str(), Length));
				};

				Length = it->second.length();
				SVOIDFLASSERT(Stream.Write2<uint16>(&Length));

				if(Length)
				{
					SVOIDFLASSERT(Stream.Write2<char>(it->second.c_str(), Length));
				};
			};
		};

		Stream.Close();

		Log::Instance.LogInfo(TAG, "All OK!");
	};
}MapData;

int main(int argc, char **argv)
{
	Log::Instance.Register();

	Log::Instance.FolderName = FLGameName();

	if(argc == 1)
	{
		Log::Instance.LogInfo(TAG, "Usage: %s [-dir outdirectory] [-imagemode] filename", argv[0]);

		return 1;
	};

	std::string OutDirectory(".");

	std::string FileName, OutFileName;

	for(int32 i = 1; i < argc; i++)
	{
		if(std::string(argv[i]) == "-dir")
		{
			if(i + 1 < argc)
			{
				OutDirectory = argv[i + 1];

				i++;
			};
		}
		else if(std::string(argv[i]) == "-splitsize")
		{
			if(i + 1 < argc)
			{
				sscanf(argv[i + 1], "%u", &MapSplitSize);
				
				i++;
			};
		}
		else if(std::string(argv[i]) == "-fti")
		{
			SaveTexturesAsFTI = true;
		}
		else
		{
			FileName = argv[i];
		};
	};

	InitSubsystems();

	int32 Position = FileName.rfind('/');

	if(Position != -1)
	{
		OutFileName = OutDirectory + "/" + FileName.substr(Position + 1, FileName.rfind('.') - Position - 1) + ".fttm";
	}
	else
	{
		Position = FileName.rfind('\\');

		if(Position != -1)
		{
			OutFileName = OutDirectory + "/" + FileName.substr(Position + 1, FileName.rfind('.') - Position - 1) + ".fttm";
		}
		else
		{
			OutFileName = OutDirectory + "/" + FileName.substr(0, FileName.rfind('.')) + ".fttm";
		};
	};

	Log::Instance.LogInfo(TAG, "Starting version %d.%d", VERSION_MAJOR, VERSION_MINOR);
	Log::Instance.LogInfo(TAG, "Will store compiled data to '%s'", OutFileName.c_str());

	SuperSmartPointer<FileStream> Stream(new FileStream());

	if(!Stream->Open(FileName, StreamFlags::Read | StreamFlags::Text))
	{
		Log::Instance.LogInfo(TAG, "Unable to open '%s' for reading!", FileName.c_str());

		DeInitSubsystems();

		return 1;
	};

	Log::Instance.LogInfo(TAG, "Parsing '%s'", FileName.c_str());

	SuperSmartPointer<Tmx::Map> Map(new Tmx::Map());

	Map->ParseText(Stream->AsString());

	if(Map->HasError())
	{
		Log::Instance.LogInfo(TAG, "Error parsing '%s': %s.", argv[1], Map->GetErrorText().c_str());

		DeInitSubsystems();

		return 1;
	};

	Vector2 MapTileSize = Vector2((f32)Map->GetTileWidth(), (f32)Map->GetTileHeight());;
	Vector2 MapPixelSize = Vector2((f32)Map->GetWidth(), (f32)Map->GetHeight());

	MapData.MapTileSize = MapTileSize;
	MapData.MapPixelSize = MapPixelSize;

	const std::vector<Tmx::Tileset *> &Sets = Map->GetTilesets();

	Log::Instance.LogInfo(TAG, "Parsing %d Tilesets", Sets.size());

	for(uint32 i = 0; i < Sets.size(); i++)
	{
		SuperSmartPointer<MapData::TileData> TileSet(new MapData::TileData());
		TileSet->FileName = Sets[i]->GetImage()->GetSource();

		int32 Position = TileSet->FileName.rfind('/');

		if(Position != -1)
		{
			TileSet->FileName = TileSet->FileName.substr(Position + 1);
		}
		else
		{
			Position = TileSet->FileName.rfind('\\');

			if(Position != -1)
			{
				TileSet->FileName = TileSet->FileName.substr(Position + 1);
			};
		};

		TileSet->Image.Reset(new TextureBuffer());

		FileStream Stream;

		if(!Stream.Open(TileSet->FileName, StreamFlags::Read))
		{
			Log::Instance.LogErr(TAG, "Unable to open a tileset '%s'!", TileSet->FileName.c_str());

			DeInitSubsystems();

			return 1;
		};

		if(!TileSet->Image->FromStream(&Stream))
		{
			Log::Instance.LogErr(TAG, "Unable to load a tileset with image '%s'!", TileSet->FileName.c_str());

			DeInitSubsystems();

			return 1;
		};

		TileSet->Ptr = Sets[i];
		TileSet->TileSize = Vector2((f32)Sets[i]->GetTileWidth(), (f32)Sets[i]->GetTileHeight());
		TileSet->FrameCount = Vector2((f32)TileSet->Image->Width(), (f32)TileSet->Image->Height()) / TileSet->TileSize;

		std::string Color = Sets[i]->GetImage()->GetTransparentColor();

		if(Color.length())
		{
			TileSet->TransparentColor = Vector3((f32)StringUtils::HexToInt(Color.substr(0, 2)),
				(f32)StringUtils::HexToInt(Color.substr(2, 2)), (f32)StringUtils::HexToInt(Color.substr(4, 2)));

			Log::Instance.LogInfo(TAG, "Baking tileset '%s' with color '%s'", TileSet->FileName.c_str(), Color.c_str());

			uint8 *Pixels = &TileSet->Image->Data[0];

			FLASSERT(Pixels, "Needs pixels for processing image!");

			if(!Pixels)
			{
				Log::Instance.LogErr(TAG, "Unable to get the texture pixels! Silently quitting...");

				DeInitSubsystems();

				return 1;
			}

			uint32 Size = TileSet->Image->Width() * TileSet->Image->Height() * 4;

			for(uint32 j = 0; j < Size; j += 4)
			{
				if(Pixels[j] == TileSet->TransparentColor.x && Pixels[j + 1] == TileSet->TransparentColor.y &&
					Pixels[j + 2] == TileSet->TransparentColor.z)
				{
					Pixels[j + 3] = 0;
				};
			};

			std::string OutFileName = TileSet->FileName.substr(0, TileSet->FileName.rfind('.')) + "_BAKED_" + Color + ".png";
			SuperSmartPointer<FileStream> Out(new FileStream());

			TextureEncoderInfo TEInfo;
			TEInfo.Encoder = TextureEncoderType::PNG;
			TEInfo.Lossless = true;
			TEInfo.Quality = 100;

			if(!Out->Open(OutFileName, StreamFlags::Write) || !TileSet->Image->Save(Out, TEInfo))
			{
				Log::Instance.LogErr("Unable to save baked texture to '%s'!", OutFileName.c_str());

				DeInitSubsystems();

				return 1;
			};

			TileSet->FileName = OutFileName;
		};

		const std::vector<Tmx::Tile *> &Tiles = Sets[i]->GetTiles();

		Log::Instance.LogInfo(TAG, "Parsing %d Tiles", Tiles.size());

		for(uint32 j = 0; j < Tiles.size(); j++)
		{
			uint32 ID = Tiles[j]->GetId();

			TileSet->Properties[ID] = Tiles[j]->GetProperties().GetList();
		};

		MapData.TileSets[MapData.TileSets.size()] = TileSet;
	};

	const std::vector<Tmx::Layer *> &Layers = Map->GetLayers();
	
	Log::Instance.LogInfo(TAG, "Parsing %d Layers", Layers.size());

	for(int32 i = MapData.TileSets.size() - 1; i >= 0; i--)
	{
		bool Found = false;

		for(uint32 j = 0; j < Layers.size(); j++)
		{
			for(uint32 k = 0; k < (uint32)Layers[j]->GetHeight(); k++)
			{
				for(uint32 l = 0; l < (uint32)Layers[j]->GetWidth(); l++)
				{
					const Tmx::MapTile &Tile = Layers[j]->GetTile(l, k);

					if(Tile.tilesetId == -1)
						continue;

					uint32 TileSetIndex = Layers[j]->GetTileTilesetIndex(l, k);

					if(TileSetIndex == 0xFFFFFFFF || MapData.TileSets.find(TileSetIndex) == MapData.TileSets.end())
					{
						Log::Instance.LogErr(TAG, "Unable to find a Tileset, silently exiting...");

						DeInitSubsystems();

						return 1;
					};

					if(TileSetIndex == i)
					{
						Found = true;

						break;
					};
				};

				if(Found)
					break;
			};

			if(Found)
				break;
		};

		if(!Found)
			MapData.TileSets.erase(i);
	};

	for(uint32 i = 0; i < Layers.size(); i++)
	{
		SuperSmartPointer<MapData::LayerData> Layer(new MapData::LayerData);

		Layer->Visible = Layers[i]->IsVisible();

		for(uint32 j = 0; j < (uint32)Layers[i]->GetHeight(); j++)
		{
			for(uint32 k = 0; k < (uint32)Layers[i]->GetWidth(); k++)
			{
				const Tmx::MapTile &Tile = Layers[i]->GetTile(k, j);

				if(Tile.tilesetId == -1)
					continue;

				uint32 TileSetIndex = Layers[i]->GetTileTilesetIndex(k, j);

				if(TileSetIndex == 0xFFFFFFFF || MapData.TileSets.find(TileSetIndex) == MapData.TileSets.end())
				{
					Log::Instance.LogErr(TAG, "Unable to find a Tileset, silently exiting...");

					DeInitSubsystems();

					return 1;
				};

				MapData::TileInfo Info;
				Info.ID = Tile.id;
				Info.FlipFlag = 0;
				Info.Position = Vector2((f32)k, (f32)j);
				
				if(Tile.flippedHorizontally)
				{
					Info.FlipFlag |= TileFlipFlags::Horizontal;
				};

				if(Tile.flippedVertically)
				{
					Info.FlipFlag |= TileFlipFlags::Vertical;
				};

				if(Tile.flippedDiagonally)
				{
					Info.FlipFlag |= TileFlipFlags::Diagonal;
				};

				Layer->Tiles[TileSetIndex].push_back(Info);
			};
		};

		MapData.Layers.push_back(Layer);
	};

	const std::vector<Tmx::ObjectGroup *> &Objects = Map->GetObjectGroups();

	Log::Instance.LogInfo(TAG, "Parsing %d Object Groups", Objects.size());

	for(uint32 i = 0; i < Objects.size(); i++)
	{
		const std::vector<Tmx::Object *> &CurrentGroup = Objects[i]->GetObjects();

		for(uint32 j = 0; j < CurrentGroup.size(); j++)
		{
			MapData::MapObject Object;

			Object.Name = CurrentGroup[j]->GetName();
			Object.Type = CurrentGroup[j]->GetType();
			Object.Position = Vector2((f32)CurrentGroup[j]->GetX(), (f32)CurrentGroup[j]->GetY());
			Object.Size = Vector2((f32)CurrentGroup[j]->GetWidth(), (f32)CurrentGroup[j]->GetHeight());
			Object.Rotation = MathUtils::DegToRad((f32)CurrentGroup[j]->GetRotation());
			Object.IsPolyLine = CurrentGroup[j]->GetPolyline() != NULL;

			if(Object.IsPolyLine)
			{
				const Tmx::Polyline *Polyline = CurrentGroup[j]->GetPolyline();

				for(uint32 k = 0; k < (uint32)Polyline->GetNumPoints(); k++)
				{
					Object.PolygonData.push_back(Vector2((f32)Polyline->GetPoint(k).x, (f32)Polyline->GetPoint(k).y));
				};
			}
			else if(CurrentGroup[j]->GetPolygon())
			{
				const Tmx::Polygon *Polygon = CurrentGroup[j]->GetPolygon();

				for(uint32 k = 0; k < (uint32)Polygon->GetNumPoints(); k++)
				{
					Object.PolygonData.push_back(Vector2((f32)Polygon->GetPoint(k).x, (f32)Polygon->GetPoint(k).y));
				};
			};

			Object.Properties = CurrentGroup[j]->GetProperties().GetList();

			MapData.MapObjects.push_back(Object);
		};
	};

	MapData.Finalize(OutFileName.c_str());

	DeInitSubsystems();

	return 0;
};
