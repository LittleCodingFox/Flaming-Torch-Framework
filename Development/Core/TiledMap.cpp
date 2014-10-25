#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	define TAG "TiledMap"

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

	union TileInstanceInfo
	{
		struct Info
		{
			uint8 Index, X, Y, FlipFlag;
		}Info;

		uint32 U32;
	};

	struct MapHeader
	{
		uint32 ID; //'F' 'T' 'T' 'M'
		VersionType Version;
		uint32 LayerCount, ObjectCount, Orientation;
		Vector2 MapTileSize, MapPixelSize;
	};

	VertexBufferHandle TiledMapVertexBuffer = 0;

	TiledMap::TiledMap() : ResourcesInitedValue(false), Scale(1, 1), Color(1, 1, 1), Orientation(0), TileRatio(0), TileOrder(TiledMapOrder::North) {};

	bool TiledMap::DeSerialize(Stream *In)
	{
		PROFILE("TiledMap::DeSerialize", StatTypes::Game);

		ResourcesInitedValue = false;

		Layers.clear();
		Objects.clear();

		MapHeader Header;
		uint16 Length = 0;

		SFLASSERT(In->Read2<MapHeader>(&Header));

		if(Header.ID != CoreUtils::MakeIntFromBytes('F', 'T', 'T', 'M'))
		{
			Log::Instance.LogErr(TAG, "Unable to deserialize a Tiled Map: Invalid ID '%04x', should be '%04x'", Header.ID,
				CoreUtils::MakeIntFromBytes('F', 'T', 'T', 'M'));

			return false;
		};

		if(Header.Version != CoreUtils::MakeVersion(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR))
		{
			Log::Instance.LogErr(TAG, "Unable to deserialize a tiled map: Invalid Version '%08x': Should be '%08x'", Header.Version,
				CoreUtils::MakeVersion(FTSTD_VERSION_MAJOR, FTSTD_VERSION_MINOR));

			return false;
		};

		MapTileSize = Header.MapTileSize;
		MapPixelSize = Header.MapPixelSize;
		MapTileCount = MapPixelSize / MapTileSize;
		Orientation = Header.Orientation;
		TileRatio = MapTileSize.x / MapTileSize.y;

		SFLASSERT(In->Read2<uint16>(&Length));

		//TileSets
#if USE_GRAPHICS
		if(Length)
		{
			TileSet.UniqueTilesetTextureName.resize(Length);
			SFLASSERT(In->Read2<char>(&TileSet.UniqueTilesetTextureName[0], Length));
		};
#else
		SFLASSERT(In->Seek(In->Position() + Length));
#endif

		uint8 UniqueTilesetCount = 0;

		SFLASSERT(In->Read2<uint8>(&UniqueTilesetCount));

		TileSet.Properties.clear();

		for(uint8 i = 0; i < UniqueTilesetCount; i++)
		{
			uint8 PropertyCount = 0;

			SFLASSERT(In->Read2<uint8>(&PropertyCount));

			for(uint32 j = 0; j < PropertyCount; j++)
			{
				std::string Key, Value;

				SFLASSERT(In->Read2<uint16>(&Length));
				Key.resize(Length);

				if(Length)
				{
					SFLASSERT(In->Read2<char>(&Key[0]));
				};

				SFLASSERT(In->Read2<uint16>(&Length));
				Value.resize(Length);

				if(Length)
				{
					SFLASSERT(In->Read2<char>(&Value[0]));
				};

				TileSet.Properties[i][Key] = Value;
			};
		};

		//Layers
		uint32 LayerCount = Header.LayerCount;

		for(uint32 i = 0; i < LayerCount; i++)
		{
			Layers.push_back(SuperSmartPointer<Layer>(new Layer()));
			Layers[Layers.size() - 1]->Alpha = 1.f;

			SFLASSERT(In->Read2<bool>(&Layers[Layers.size() - 1]->Visible));

			uint8 TileCount = 0;
			SFLASSERT(In->Read2<uint8>(&TileCount));

			for(uint32 j = 0; j < TileCount; j++)
			{
				uint16 InstanceCount = 0;

				SFLASSERT(In->Read2<uint16>(&InstanceCount));

				TileInstanceInfo Info;

				for(uint32 k = 0; k < InstanceCount; k++)
				{
					SFLASSERT(In->Read2<uint32>(&Info.U32));

					Layer::TileInfo TInfo;
					TInfo.ID = Info.Info.Index;
					TInfo.Position = Vector2((f32)Info.Info.X, (f32)Info.Info.Y);
					TInfo.FlipFlag = Info.Info.FlipFlag;

					Layers[Layers.size() - 1]->Tiles.push_back(TInfo);
				};
			};
		};

		//Objects
		for(uint32 i = 0; i < Header.ObjectCount; i++)
		{
			MapObject Object;

			SFLASSERT(In->Read2<uint16>(&Length));
			Object.Name.resize(Length);

			if(Length)
			{
				SFLASSERT(In->Read2<char>(&Object.Name[0], Length));
			};

			SFLASSERT(In->Read2<uint16>(&Length));
			Object.Type.resize(Length);

			if(Length)
			{
				SFLASSERT(In->Read2<char>(&Object.Type[0], Length));
			};

			SFLASSERT(In->Read2<Vector2>(&Object.Position));
			SFLASSERT(In->Read2<Vector2>(&Object.Size));
			SFLASSERT(In->Read2<f32>(&Object.Rotation));
			SFLASSERT(In->Read2<uint8>(&Object.IsPolyLine));

			SFLASSERT(In->Read2<uint16>(&Length));

			if(Length)
			{
				Object.PolygonData.resize(Length);
				SFLASSERT(In->Read2<Vector2>(&Object.PolygonData[0], Length));
			};

			SFLASSERT(In->Read2<uint16>(&Length));

			uint32 PropCount = Length;

			std::string Name, Value;

			for(uint32 j = 0; j < PropCount; j++)
			{
				SFLASSERT(In->Read2<uint16>(&Length));
				Name.resize(Length);

				if(Length)
				{
					SFLASSERT(In->Read2<char>(&Name[0], Length));
				};

				SFLASSERT(In->Read2<uint16>(&Length));
				Value.resize(Length);

				if(Length)
				{
					SFLASSERT(In->Read2<char>(&Value[0], Length));
				};

				Object.Properties[Name] = Value;
			};

			Objects.push_back(Object);
		};

		return true;
	};

#if USE_GRAPHICS
	void TiledMap::UpdateGeometry()
	{
		Vector2 HalfTileSize = (MapTileSize / 2).Round();

		for(uint32 i = 0; i < Layers.size(); i++)
		{
			if(Layers[i]->Vertices.size() != Layers[i]->Tiles.size() * 6)
				Layers[i]->Vertices.resize(Layers[i]->Tiles.size() * 6);

			if(Layers[i]->TexCoords.size() != Layers[i]->Tiles.size() * 6)
				Layers[i]->TexCoords.resize(Layers[i]->Tiles.size() * 6);

			Vector2 OnePixel = Vector2(1, 1) / TileSet.UniqueTilesetTexture->Size();
			Vector2 DrawingOffset = Orientation == TiledMapOrientationMode::Isometric ? Vector2(MapTileCount.x * HalfTileSize.x, 0) : Vector2();
			uint32 index = 0;

			for(f32 y = 0; y < MapTileCount.y; y++)
			{
				for(f32 x = 0; x < MapTileCount.x; x++)
				{
					f32 ActualX = x, ActualY = y;

					uint32 ExtraSwapFlags = 0;

					switch(TileOrder)
					{
						case TiledMapOrder::North:
							//Do nothing

							break;

						case TiledMapOrder::East:
							std::swap(ActualX, ActualY);
							ActualX = MapTileCount.y - ActualX - 1;
							ExtraSwapFlags = TileFlipFlags::Diagonal;

							break;

						case TiledMapOrder::South:
							ActualY = MapTileCount.y - ActualY - 1;
							ExtraSwapFlags = TileFlipFlags::Vertical;

							break;

						case TiledMapOrder::West:
							std::swap(ActualX, ActualY);
							ExtraSwapFlags = TileFlipFlags::Diagonal | TileFlipFlags::Vertical | TileFlipFlags::Horizontal;

							break;

						default:
							Log::Instance.LogDebug(TAG, "Unknown Tiled Map Order '%d'", TileOrder);

							break;
					};

					int32 FoundLayerIndex = -1;

					for(uint32 j = 0; j < Layers[i]->Tiles.size(); j++)
					{
						if(Layers[i]->Tiles[j].Position.x == x && Layers[i]->Tiles[j].Position.y == y)
						{
							FoundLayerIndex = j;

							break;
						};
					};

					if(FoundLayerIndex == -1)
						continue;

					TexturePacker::SortedTexture &TextureInfo = TileSet.UniqueTilesetTexturePacker->Indices[TileSet.UniqueTilesetTexturePacker->GetTexture(Layers[i]->Tiles[FoundLayerIndex].ID)->GetIndex().Index];

					Vector2 FramePosition = Vector2((f32)TextureInfo.x, (f32)TextureInfo.y) / TileSet.UniqueTilesetTexture->Size();
					Vector2 FrameSize = Vector2((f32)TextureInfo.Width, (f32)TextureInfo.Height) / TileSet.UniqueTilesetTexture->Size();

					Vector2 ActualPosition = DrawingOffset + (Orientation == TiledMapOrientationMode::Isometric ? Vector2((ActualX - ActualY) * HalfTileSize.x, (ActualX + ActualY) * HalfTileSize.y) : Vector2(ActualX, ActualY) * MapTileSize);
					
					Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = ActualPosition;
					Layers[i]->Vertices[index + 1] = ActualPosition + Vector2(0, (f32)TextureInfo.Height);
					Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = ActualPosition + Vector2((f32)TextureInfo.Width, (f32)TextureInfo.Height);
					Layers[i]->Vertices[index + 4] = ActualPosition + Vector2((f32)TextureInfo.Width, 0);

					Layers[i]->TexCoords[index] = Layers[i]->TexCoords[index + 5] = FramePosition;
					Layers[i]->TexCoords[index + 1] = FramePosition + Vector2(0, FrameSize.y);
					Layers[i]->TexCoords[index + 2] = Layers[i]->TexCoords[index + 3] = FramePosition + FrameSize;
					Layers[i]->TexCoords[index + 4] = FramePosition + Vector2(FrameSize.x, 0);

					static Vector2 TempVertices[6];

					if(Layers[i]->Tiles[FoundLayerIndex].FlipFlag & TileFlipFlags::Diagonal)
					{
						memcpy(TempVertices, &Layers[i]->Vertices[index], sizeof(Vector2[6]));
						Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = TempVertices[4];
						Layers[i]->Vertices[index + 1] = TempVertices[0];
						Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = TempVertices[1];
						Layers[i]->Vertices[index + 4] = TempVertices[2];
					};

					if(Layers[i]->Tiles[FoundLayerIndex].FlipFlag & TileFlipFlags::Vertical)
					{
						memcpy(TempVertices, &Layers[i]->Vertices[index], sizeof(Vector2[6]));
						Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = TempVertices[2];
						Layers[i]->Vertices[index + 1] = TempVertices[4];
						Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = TempVertices[0];
						Layers[i]->Vertices[index + 4] = TempVertices[1];
					};

					if(Layers[i]->Tiles[FoundLayerIndex].FlipFlag & TileFlipFlags::Horizontal)
					{
						memcpy(TempVertices, &Layers[i]->Vertices[index], sizeof(Vector2[6]));
						Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = TempVertices[4];
						Layers[i]->Vertices[index + 1] = TempVertices[2];
						Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = TempVertices[1];
						Layers[i]->Vertices[index + 4] = TempVertices[0];
					};

					//Need to repeat here in case of rotating
					if(ExtraSwapFlags & TileFlipFlags::Diagonal)
					{
						memcpy(TempVertices, &Layers[i]->Vertices[index], sizeof(Vector2[6]));
						Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = TempVertices[4];
						Layers[i]->Vertices[index + 1] = TempVertices[0];
						Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = TempVertices[1];
						Layers[i]->Vertices[index + 4] = TempVertices[2];
					};
					
					if(ExtraSwapFlags & TileFlipFlags::Vertical)
					{
						memcpy(TempVertices, &Layers[i]->Vertices[index], sizeof(Vector2[6]));
						Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = TempVertices[2];
						Layers[i]->Vertices[index + 1] = TempVertices[4];
						Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = TempVertices[0];
						Layers[i]->Vertices[index + 4] = TempVertices[1];
					};
					
					if(ExtraSwapFlags & TileFlipFlags::Horizontal)
					{
						memcpy(TempVertices, &Layers[i]->Vertices[index], sizeof(Vector2[6]));
						Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = TempVertices[4];
						Layers[i]->Vertices[index + 1] = TempVertices[2];
						Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = TempVertices[1];
						Layers[i]->Vertices[index + 4] = TempVertices[0];
					};

					index += 6;
				};
			};
		};
	};

	bool TiledMap::InitResources(const TiledMapInitOptions &Options)
	{
		PROFILE("TiledMap::InitResources", StatTypes::Rendering);

		if(TileSet.UniqueTilesetTextureName.length() == 0)
		{
			ResourcesInitedValue = true;

			return true;
		};

		bool KeepData = Texture::KeepData;
		Texture::KeepData = false;

		if(Options.FromPackageValue)
		{
			bool Found = false;

			for(uint32 i = 0; i < Options.PackageDirectoriesValue.size(); i++)
			{
				TileSet.UniqueTilesetTexture = ResourceManager::Instance.GetTextureFromPackage(Options.PackageDirectoriesValue[i], TileSet.UniqueTilesetTextureName + ".png");

				if(TileSet.UniqueTilesetTexture.Get())
				{
					TileSet.UniqueTilesetTexture->SetTextureFiltering(TextureFiltering::Nearest);

					SuperSmartPointer<Stream> ConfigurationStream = PackageFileSystemManager::Instance.GetFile(MakeStringID(Options.PackageDirectoriesValue[i]), MakeStringID(TileSet.UniqueTilesetTextureName + ".cfg"));

					if(!ConfigurationStream.Get() || !TileSet.UniqueTilesetConfig.DeSerialize(ConfigurationStream))
					{
						Log::Instance.LogErr(TAG, "Unable to load Unique Tileset Config '%s.cfg'", TileSet.UniqueTilesetTextureName.c_str());

						TileSet.UniqueTilesetTexture.Dispose();

						continue;
					};

					Found = true;

					break;
				};
			};

			if(!Found)
			{
				TileSet.UniqueTilesetTexture = ResourceManager::Instance.GetTexture(TileSet.UniqueTilesetTextureName + ".png");

				if(!TileSet.UniqueTilesetTexture.Get())
				{
					Log::Instance.LogErr(TAG, "Unable to load Unique Tileset Texture '%s.png'", TileSet.UniqueTilesetTextureName.c_str());

					Texture::KeepData = KeepData;

					return false;
				};

				TileSet.UniqueTilesetTexture->SetTextureFiltering(TextureFiltering::Nearest);

				SuperSmartPointer<Stream> ConfigurationStream(new FileStream());

				if(!ConfigurationStream.AsDerived<FileStream>()->Open((TileSet.UniqueTilesetTextureName + ".cfg").c_str(), StreamFlags::Read | StreamFlags::Text) || !TileSet.UniqueTilesetConfig.DeSerialize(ConfigurationStream))
				{
					Log::Instance.LogErr(TAG, "Unable to load Unique Tileset Config '%s.cfg'", TileSet.UniqueTilesetTextureName.c_str());

					TileSet.UniqueTilesetTexture.Dispose();

					return false;
				};
			};
		}
		else
		{
			TileSet.UniqueTilesetTexture = ResourceManager::Instance.GetTexture(TileSet.UniqueTilesetTextureName + ".png");

			if(!TileSet.UniqueTilesetTexture.Get())
			{
				Log::Instance.LogErr(TAG, "Unable to load Unique Tileset Texture '%s.png'", TileSet.UniqueTilesetTextureName.c_str());

				Texture::KeepData = KeepData;

				return false;
			};

			TileSet.UniqueTilesetTexture->SetTextureFiltering(TextureFiltering::Nearest);

			SuperSmartPointer<Stream> ConfigurationStream(new FileStream());

			if(!ConfigurationStream.AsDerived<FileStream>()->Open((TileSet.UniqueTilesetTextureName + ".cfg").c_str(), StreamFlags::Read | StreamFlags::Text) || !TileSet.UniqueTilesetConfig.DeSerialize(ConfigurationStream))
			{
				Log::Instance.LogErr(TAG, "Unable to load Unique Tileset Config '%s.cfg'", TileSet.UniqueTilesetTextureName.c_str());

				TileSet.UniqueTilesetTexture.Dispose();

				return false;
			};
		};

		TileSet.UniqueTilesetTexturePacker = TexturePacker::FromConfig(TileSet.UniqueTilesetTexture, TileSet.UniqueTilesetConfig);

		if(!TileSet.UniqueTilesetTexturePacker.Get())
		{
			Log::Instance.LogErr(TAG, "Unable to load Unique Tileset Config '%s.cfg'", TileSet.UniqueTilesetTextureName.c_str());

			TileSet.UniqueTilesetTexture.Dispose();

			return false;
		};

		Texture::KeepData = KeepData;

		ResourceManager::Instance.Cleanup();

		ResourcesInitedValue = true;

		UpdateGeometry();

		return true;
	};

	void TiledMap::Draw(uint32 Layer, Renderer *Renderer)
	{
		PROFILE("TiledMap::Draw", StatTypes::Rendering);

		if(!ResourcesInitedValue || Layer >= Layers.size() || Layers[Layer]->Visible == false || Layers[Layer]->Vertices.size() == 0)
			return;

		SpriteCache::Instance.Flush(Renderer);

		bool DoTranslation = Translation != Vector2() || Scale != Vector2(1, 1);

		if(DoTranslation)
		{
			Renderer->PushMatrices();
			Renderer->SetWorldMatrix(Renderer->WorldMatrix() * Matrix4x4::Scale(Vector4(Scale, 1, 1)) * Matrix4x4::Translate(Vector4(Translation, 0, 1)));
		};

		if(!Renderer->IsVertexBufferHandleValid(TiledMapVertexBuffer))
		{
			TiledMapVertexBuffer = Renderer->CreateVertexBuffer();
		};

		//TODO: Optimize this
		static std::vector<SpriteVertex> Vertices;

		Vertices.resize(Layers[Layer]->Vertices.size());

		for(uint32 i = 0; i < Layers[Layer]->Vertices.size(); i++)
		{
			Vertices[i].Position = Layers[Layer]->Vertices[i];
			Vertices[i].Color = Vector4(Color, Layers[Layer]->Alpha);
			Vertices[i].TexCoord = Layers[Layer]->TexCoords[i];
		};

		Renderer->SetBlendingMode(BlendingMode::Alpha);
		Renderer->SetVertexBufferData(TiledMapVertexBuffer, VertexDetailsMode::Mixed, SpriteVertexDescriptor, sizeof(SpriteVertexDescriptor) / sizeof(SpriteVertexDescriptor[0]), &Vertices[0], sizeof(SpriteVertex) * Vertices.size());

		Renderer->BindTexture(TileSet.UniqueTilesetTexture.Get());

		Renderer->RenderVertices(VertexModes::Triangles, TiledMapVertexBuffer, 0, Vertices.size());

		Layers[Layer]->OnLayerDraw(this, Layer, Renderer);

		if(DoTranslation)
		{
			Renderer->PopMatrices();
		};
	};

	bool TiledMap::ResourcesInited()
	{
		return ResourcesInitedValue;
	};
#endif

	Vector2 TiledMap::ToIsometric(const Vector2 &Point)
	{
		if(Orientation == TiledMapOrientationMode::Isometric)
			return Point;

		return Vector2(Point.x - Point.y, Point.x / TileRatio + Point.y / TileRatio);
	};

	Vector2 TiledMap::ToOrthogonal(const Vector2 &Point)
	{
		if(Orientation == TiledMapOrientationMode::Orthogonal)
			return Point;

		return Vector2(Point.x / TileRatio + Point.y, Point.y - Point.x / TileRatio);
	};
};
