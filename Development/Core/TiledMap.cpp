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
		uint32 LayerCount, ObjectCount;
		Vector2 MapTileSize, MapPixelSize;
	};

	TiledMap::TiledMap() : ResourcesInitedValue(false), Scale(1, 1), Color(1, 1, 1) {};

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

		SFLASSERT(In->Read2<uint16>(&Length));

		if(Length == 0)
		{
			Log::Instance.LogErr(TAG, "Unable to deserialize a tiled map: Invalid Length 0");

			return false;
		};

		//TileSets
#if USE_GRAPHICS
		TileSet.UniqueTilesetTextureName.resize(Length);
		SFLASSERT(In->Read2<char>(&TileSet.UniqueTilesetTextureName[0], Length));
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
		uint32 TileSetFrameWidth = (uint32)(TileSet.UniqueTilesetTexture->Width() / MapTileSize.x);
		Vector2 OneFrame = MapTileSize / TileSet.UniqueTilesetTexture->Size();

		for(uint32 i = 0; i < Layers.size(); i++)
		{
			if(Layers[i]->Vertices.size() != Layers[i]->Tiles.size() * 6)
				Layers[i]->Vertices.resize(Layers[i]->Tiles.size() * 6);

			if(Layers[i]->TexCoords.size() != Layers[i]->Tiles.size() * 6)
				Layers[i]->TexCoords.resize(Layers[i]->Tiles.size() * 6);

			for(uint32 j = 0, index = 0; j < Layers[i]->Tiles.size(); j++, index += 6)
			{
				Vector2 ActualPosition = Layers[i]->Tiles[j].Position * MapTileSize;

				Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = ActualPosition;
				Layers[i]->Vertices[index + 1] = ActualPosition + Vector2(0, MapTileSize.y);
				Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = ActualPosition + MapTileSize;
				Layers[i]->Vertices[index + 4] = ActualPosition + Vector2(MapTileSize.x, 0);

				Vector2 FramePosition = Vector2((f32)(Layers[i]->Tiles[j].ID % TileSetFrameWidth), (f32)(Layers[i]->Tiles[j].ID / TileSetFrameWidth));
				Vector2 BaseTexCoord = OneFrame * FramePosition;

				Layers[i]->TexCoords[index] = Layers[i]->TexCoords[index + 5] = BaseTexCoord;
				Layers[i]->TexCoords[index + 1] = BaseTexCoord + Vector2(0, OneFrame.y);
				Layers[i]->TexCoords[index + 2] = Layers[i]->TexCoords[index + 3] = BaseTexCoord + OneFrame;
				Layers[i]->TexCoords[index + 4] = BaseTexCoord + Vector2(OneFrame.x, 0);

				static Vector2 TempVertices[6];

				if(Layers[i]->Tiles[j].FlipFlag & TileFlipFlags::Diagonal)
				{
					memcpy(TempVertices, &Layers[i]->Vertices[index], sizeof(Vector2[6]));
					Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = TempVertices[4];
					Layers[i]->Vertices[index + 1] = TempVertices[0];
					Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = TempVertices[1];
					Layers[i]->Vertices[index + 4] = TempVertices[2];
				};

				if(Layers[i]->Tiles[j].FlipFlag & TileFlipFlags::Vertical)
				{
					memcpy(TempVertices, &Layers[i]->Vertices[index], sizeof(Vector2[6]));
					Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = TempVertices[2];
					Layers[i]->Vertices[index + 1] = TempVertices[4];
					Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = TempVertices[0];
					Layers[i]->Vertices[index + 4] = TempVertices[1];
				};

				if(Layers[i]->Tiles[j].FlipFlag & TileFlipFlags::Horizontal)
				{
					memcpy(TempVertices, &Layers[i]->Vertices[index], sizeof(Vector2[6]));
					Layers[i]->Vertices[index] = Layers[i]->Vertices[index + 5] = TempVertices[4];
					Layers[i]->Vertices[index + 1] = TempVertices[2];
					Layers[i]->Vertices[index + 2] = Layers[i]->Vertices[index + 3] = TempVertices[1];
					Layers[i]->Vertices[index + 4] = TempVertices[0];
				};
			};
		};
	};

	bool TiledMap::InitResources(const TiledMapInitOptions &Options)
	{
		PROFILE("TiledMap::InitResources", StatTypes::Rendering);

		bool KeepData = Texture::KeepData;
		Texture::KeepData = false;

		if(Options.FromPackageValue)
		{
			bool Found = false;

			for(uint32 i = 0; i < Options.PackageDirectoriesValue.size(); i++)
			{
				TileSet.UniqueTilesetTexture = ResourceManager::Instance.GetTextureFromPackage(Options.PackageDirectoriesValue[i], TileSet.UniqueTilesetTextureName);

				if(TileSet.UniqueTilesetTexture.Get())
				{
					Found = true;

					TileSet.UniqueTilesetTexture->SetTextureFiltering(TextureFiltering::Nearest);

					break;
				};
			};

			if(!Found)
			{
				TileSet.UniqueTilesetTexture = ResourceManager::Instance.GetTexture(TileSet.UniqueTilesetTextureName);

				if(!TileSet.UniqueTilesetTexture.Get())
				{
					Log::Instance.LogErr(TAG, "Unable to load Unique Tileset Texture '%s'", TileSet.UniqueTilesetTextureName.c_str());

					Texture::KeepData = KeepData;

					return false;
				};

				TileSet.UniqueTilesetTexture->SetTextureFiltering(TextureFiltering::Nearest);
			};
		}
		else
		{
			TileSet.UniqueTilesetTexture = ResourceManager::Instance.GetTexture(TileSet.UniqueTilesetTextureName);

			if(!TileSet.UniqueTilesetTexture.Get())
			{
				Log::Instance.LogErr(TAG, "Unable to load Unique Tileset Texture '%s'", TileSet.UniqueTilesetTextureName.c_str());

				Texture::KeepData = KeepData;

				return false;
			};


			TileSet.UniqueTilesetTexture->SetTextureFiltering(TextureFiltering::Nearest);
		};

		Texture::KeepData = KeepData;

		ResourceManager::Instance.Cleanup();

		ResourcesInitedValue = true;

		UpdateGeometry();

		return true;
	};

	void TiledMap::Draw(uint32 Layer, RendererManager::Renderer *Renderer)
	{
		PROFILE("TiledMap::Draw", StatTypes::Rendering);

		if(!ResourcesInitedValue || Layer >= Layers.size() || Layers[Layer]->Visible == false)
			return;

		SpriteCache::Instance.Flush(Renderer);

		bool DoTranslation = Translation != Vector2() || Scale != Vector2(1, 1);

		if(DoTranslation)
		{
			glPushMatrix();
			glScalef(Scale.x, Scale.y, 1);
			glTranslatef(Translation.x, Translation.y, 0);
		};

		Renderer->BindTexture(TileSet.UniqueTilesetTexture);
		Renderer->EnableState(GL_TEXTURE_2D);
		Renderer->EnableState(GL_VERTEX_ARRAY);
		Renderer->EnableState(GL_TEXTURE_COORD_ARRAY);
		Renderer->DisableState(GL_COLOR_ARRAY);
		Renderer->DisableState(GL_NORMAL_ARRAY);

		glColor4f(Color.x, Color.y, Color.z, Layers[Layer]->Alpha);

		glVertexPointer(2, GL_FLOAT, 0, &Layers[Layer]->Vertices[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &Layers[Layer]->TexCoords[0]);

		glDrawArrays(GL_TRIANGLES, 0, Layers[Layer]->Vertices.size());

		Layers[Layer]->OnLayerDraw(this, Layer, Renderer);

		SpriteCache::Instance.Flush(Renderer);

		glColor4f(1, 1, 1, 1);

		if(DoTranslation)
		{
			glPopMatrix();
		};
	};

	bool TiledMap::ResourcesInited()
	{
		return ResourcesInitedValue;
	};
#endif
};
