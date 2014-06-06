#include "FlamingCore.hpp"
namespace FlamingTorch
{
	StringID MakeCoordinateID(const Vector3 &Coordinate)
	{
		static std::stringstream str;
		str.str("");
		str << Coordinate.x << "_" << Coordinate.y << "_" << Coordinate.z;

		return MakeStringID(str.str());
	};

	uint8 WorldStreamer::GetWorldRadius()
	{
		return (uint8)WorldRadiusValue;
	};

	void WorldStreamer::SetWorldRadius(uint8 ChunkRadius)
	{
		WorldRadiusValue = (f32)ChunkRadius;

		WorldDirections.clear();
		ChunkLoadFlagCache.clear();
		LoadedChunks.clear();

		for(f32 x = -WorldRadiusValue; x <= WorldRadiusValue; x++)
		{
			for(f32 y = -WorldRadiusValue; y <= WorldRadiusValue; y++)
			{
				for(f32 z = -WorldRadiusValue; z <= WorldRadiusValue; z++)
				{
					WorldDirections.push_back(Vector3(x, y, z));
				};
			};
		};

		LoadedChunks.resize(WorldDirections.size());
		ChunkLoadFlagCache.resize(WorldDirections.size());

		for(uint32 i = 0; i < ChunkLoadFlagCache.size(); i++)
		{
			ChunkLoadFlagCache[i] = true;
		};
	};

	void WorldStreamer::Reset()
	{
		for(uint32 i = 0; i < LoadedChunks.size(); i++)
		{
			UnloadChunk(i);
		};

		StateManager::Instance.Save();
	};

	StateCollection *WorldStreamer::GetChunkCollection(WorldChunk *Chunk, bool Create)
	{
		/*
		StringID ChunkID = MakeStringID(MakeChunkNameString(Chunk->RealCoordinate));
		bool GotSession = StateManager::Instance.Collections.find(ChunkID) != StateManager::Instance.Collections.end();

		if(!GotSession)
		{
			if(StateManager::Instance.Load("Auto", MakeChunkNameString(Chunk->RealCoordinate)))
				GotSession = StateManager::Instance.Collections.find(ChunkID) != StateManager::Instance.Collections.end();
		};

		if(!GotSession)
		{
			StateManager::Instance.Collections[ChunkID].Directory = "Auto";
		};

		return (GotSession || Create) ? &StateManager::Instance.Collections[ChunkID] : NULL;
		*/

		return NULL;
	};

	void WorldChunk::UpdateCoordinate(uint32 ChunkIndex)
	{
		Coordinate = Owner->WorldDirections[ChunkIndex];

		if(Owner->Callback)
			Owner->Callback->OnMapChunkCoordinateUpdate(this);
	};

	Vector3 WorldStreamer::ClipUnitPosition(const Vector3 &Position)
	{
		const Vector3 &ChunkSize = Callback->ChunkSize();

		if(Position.x < 0)
			return Vector3(ChunkSize.x, Position.y, Position.z);

		if(Position.y < 0)
			return Vector3(Position.x, ChunkSize.y, Position.z);

		if(Position.z < 0)
			return Vector3(Position.x, Position.y, ChunkSize.z);

		if(Position.x >= ChunkSize.x)
			return Vector3(0, Position.y, Position.z);

		if(Position.y >= ChunkSize.y)
			return Vector3(Position.x, 0, Position.z);

		if(Position.z >= ChunkSize.z)
			return Vector3(Position.x, Position.y, 0);

		return Position;
	};

	int32 WorldStreamer::GetChunkIndex(const Vector3 &Coordinate)
	{
		for(uint32 i = 0; i < WorldDirections.size(); i++)
		{
			if(WorldDirections[i] == Coordinate)
				return i;
		};

		return -1;
	};
	
	void WorldStreamer::UnloadChunk(uint32 Index)
	{
		if(Index >= LoadedChunks.size())
			return;

		if(LoadedChunks[Index])
		{
			if(Callback)
			{
				Callback->OnMapChunkUnload(LoadedChunks[Index], GetChunkCollection(LoadedChunks[Index], true));
			};

			LoadedChunks[Index].Dispose();
		};

		ChunkLoadFlagCache[Index] = true;
	};

	void WorldStreamer::TranslateMainChunk(const Vector3 &Direction)
	{
		static std::vector<uint32> SwappedChunks;

		GlobalCoordinate -= Direction;

		SwappedChunks.clear();

		for(uint32 i = 0; i < WorldDirections.size(); i++)
		{
			//Ignore chunks that are going to leave the stream
			if(fabs(WorldDirections[i].x + Direction.x) > WorldRadiusValue ||
				fabs(WorldDirections[i].y + Direction.y) > WorldRadiusValue ||
				fabs(WorldDirections[i].z + Direction.z) > WorldRadiusValue)
			{
				UnloadChunk(i);

				continue;
			};
		};

		/*
		*	Sort of slow but fixes issues regarding multiple moves and we don't duplicate code...
		*	original problem was as follows:
		*	moving from to left was OK, since we were checking from left to right
		*	moving from to right was a problem, since stuff like this happened: "swapped chunk (-1, 0) to (0, 0); swapped chunk (0, 0) to (1, 0)"
		*/
		for(f32 x = WorldRadiusValue * (Direction.x < 0 ? -1 : 1);
			(Direction.x < 0 ? x <= WorldRadiusValue : x >= -WorldRadiusValue);
			(Direction.x < 0 ? x++ : x--))
		{
			for(f32 y = WorldRadiusValue * (Direction.y < 0 ? -1 : 1);
				(Direction.y < 0 ? y <= WorldRadiusValue : y >= -WorldRadiusValue);
				(Direction.y < 0 ? y++ : y--))
			{
				for(f32 z = WorldRadiusValue * (Direction.z < 0 ? -1 : 1);
					(Direction.z < 0 ? z <= WorldRadiusValue : z >= -WorldRadiusValue);
					(Direction.z < 0 ? z++ : z--))
				{
					//Ignore chunks that are going to leave the stream
					if(fabs(x + Direction.x) > WorldRadiusValue ||
						fabs(y + Direction.y) > WorldRadiusValue ||
						fabs(z + Direction.z) > WorldRadiusValue)
						continue;

					int32 ChunkIndex = GetChunkIndex(Vector3(x, y, z));
					int32 TargetChunkIndex = GetChunkIndex(Vector3(x, y, z) + Direction);

					if(TargetChunkIndex != -1)
					{
						std::swap(LoadedChunks[TargetChunkIndex], LoadedChunks[ChunkIndex]);

						/*
						if(y == 0 && z == 0)
							Log::Instance.LogInfo("WorldStreamer", "Swapped chunks (%f, %f, %f, 0x%08x) and (%f, %f, %f, 0x%08x)",
							WorldDirections[ChunkIndex].x, WorldDirections[ChunkIndex].y, WorldDirections[ChunkIndex].z, LoadedChunks[ChunkIndex].Get(),
							x + Direction.x, y + Direction.y, z + Direction.z, LoadedChunks[TargetChunkIndex].Get());
						*/

						SwappedChunks.push_back(ChunkIndex);
						SwappedChunks.push_back(TargetChunkIndex);
					};
				};
			};
		};

		for(f32 x = WorldRadiusValue * (Direction.x < 0 ? -1 : 1);
			(Direction.x < 0 ? x <= WorldRadiusValue : x >= -WorldRadiusValue);
			(Direction.x < 0 ? x++ : x--))
		{
			for(f32 y = WorldRadiusValue * (Direction.y < 0 ? -1 : 1);
				(Direction.y < 0 ? y <= WorldRadiusValue : y >= -WorldRadiusValue);
				(Direction.y < 0 ? y++ : y--))
			{
				for(f32 z = WorldRadiusValue * (Direction.z < 0 ? -1 : 1);
					(Direction.z < 0 ? z <= WorldRadiusValue : z >= -WorldRadiusValue);
					(Direction.z < 0 ? z++ : z--))
				{
					uint32 Index = GetChunkIndex(Vector3(x, y, z));
					bool Found = false;

					for(uint32 j = 0; j < SwappedChunks.size(); j++)
					{
						if(SwappedChunks[j] == Index)
						{
							Found = true;

							break;
						};
					};

					if(!Found)
					{
						UnloadChunk(Index);

						if(Callback && Callback->ChunkNeedsLoad(GlobalCoordinate + WorldDirections[Index]))
						{
							LoadedChunks[Index].Reset(new WorldChunk(this));

							WorldChunk *Target = LoadedChunks[Index];
							Target->Owner = this;
							Target->Coordinate = WorldDirections[Index];
							Target->GlobalCoordinate = GlobalCoordinate + WorldDirections[Index];

							if(!Callback->OnMapChunkLoad(Target, GetChunkCollection(Target)))
							{
								LoadedChunks[Index].Dispose();

								ChunkLoadFlagCache[Index] = false;
							}
							else
							{
								ChunkLoadFlagCache[Index] = true;
							};
						}
						else
						{
							ChunkLoadFlagCache[Index] = false;
						};
					};

					if(LoadedChunks[Index].Get())
						LoadedChunks[Index]->UpdateCoordinate(Index);
				};
			};
		};

		StateManager::Instance.Save();
	};

	void WorldStreamer::Update()
	{
		if(Callback)
		{
			for(uint32 i = 0; i < LoadedChunks.size(); i++)
			{
				if(!LoadedChunks[i].Get() && ChunkLoadFlagCache[i])
				{
					ChunkLoadFlagCache[i] = Callback->ChunkNeedsLoad(GlobalCoordinate + WorldDirections[i]);

					if(ChunkLoadFlagCache[i])
					{
						LoadedChunks[i].Reset(new WorldChunk(this));

						WorldChunk *Target = LoadedChunks[i];
						Target->Owner = this;
						Target->Coordinate = WorldDirections[i];
						Target->GlobalCoordinate = GlobalCoordinate + WorldDirections[i];

						if(!Callback->OnMapChunkLoad(Target, GetChunkCollection(Target)))
						{
							LoadedChunks[i].Dispose();

							ChunkLoadFlagCache[i] = false;
						};
					};
				};
			};
		};
	};

	void WorldStreamer::SetGlobalCoordinate(const Vector3 &NewGlobalCoordinate)
	{
		for(uint32 i = 0; i < LoadedChunks.size(); i++)
		{
			UnloadChunk(i);
		};

		StateManager::Instance.Save();

		GlobalCoordinate = NewGlobalCoordinate;
	};

	const Vector3 &WorldStreamer::GetGlobalCoordinate()
	{
		return GlobalCoordinate;
	};
};
