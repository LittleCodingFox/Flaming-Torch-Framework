#pragma once

class WorldStreamer;

class WorldChunk
{
public:
	/*!
	*	The chunk's local coordinate
	*/
	Vector3 Coordinate;
	/*!
	*	The chunk's global coordinate
	*/
	Vector3 GlobalCoordinate;

	/*!
	*	The chunk's Owner
	*/
	WorldStreamer *Owner;

	/*!
	*	The chunk's user data. Override this to store a connection between your world data and this chunk
	*/
	void *UserData;

	WorldChunk(WorldStreamer *_Owner) : UserData(NULL), Owner(_Owner) {};

	/*!
	*	Update the local coordinate of this chunk
	*	\param ChunkIndex the current chunk index
	*/
	void UpdateCoordinate(uint32 ChunkIndex);
};

class WorldStreamerCallback
{
public:
	virtual ~WorldStreamerCallback() {};

	/*!
	*	Perform the loading process on this chunk
	*	\param Chunk the chunk to be loaded
	*	\param State the state collection associated to the chunk
	*	\return whether the map chunk was loaded
	*/
	virtual bool OnMapChunkLoad(WorldChunk *Chunk, StateCollection *State) = 0;
	/*!
	*	Perform the unload process on this chunk (like saving data)
	*	\param Chunk the chunk to be unloaded
	*	\param State the state collection associated to the chunk
	*/
	virtual void OnMapChunkUnload(WorldChunk *Chunk, StateCollection *State) = 0;

	/*!
	*	Updates a chunk's local coordinate
	*	\param Chunk the chunk to be updated, with the new coordinate
	*/
	virtual void OnMapChunkCoordinateUpdate(WorldChunk *Chunk) = 0;

	/*!
	*	\return the size in pixels or units of each chunk (separately)
	*/
	virtual const Vector3 &ChunkSize() = 0;

	/*!
	*	Inform the streamer whether a specific chunk can be loaded
	*	\param GlobalCoordinate the glbal coordinate of this chunk
	*	\return whether the map chunk can be loaded
	*/
	virtual bool ChunkNeedsLoad(const Vector3 &GlobalCoordinate) = 0;
};

class ScriptedWorldStreamerCallback : public WorldStreamerCallback
{
public:
	luabind::object ChunkLoadFunction, ChunkUnloadFunction, ChunkCoordinateUpdateFunction, ChunkSizeFunction, ChunkNeedsLoadFunction;

	SuperSmartPointer<LuaScript> ScriptInstance;

	ScriptedWorldStreamerCallback(SuperSmartPointer<LuaScript> Script) : ScriptInstance(Script)
	{
		if(ScriptInstance.Get())
		{
			luabind::object Globals = luabind::globals(ScriptInstance.Get()->State);

			ChunkLoadFunction = Globals["WorldStreamerChunkLoad"];
			ChunkUnloadFunction = Globals["WorldStreamerChunkUnload"];
			ChunkCoordinateUpdateFunction = Globals["WorldStreamerChunkCoordinateUpdate"];
			ChunkSizeFunction = Globals["WorldStreamerChunkSize"];
			ChunkNeedsLoadFunction = Globals["WorldStreamerChunkNeedsLoad"];
		};
	};

	bool OnMapChunkLoad(WorldChunk *Chunk, StateCollection *State)
	{
		if(ChunkLoadFunction)
		{
			try
			{
				return ProtectedLuaCast<bool>(ChunkLoadFunction(Chunk));
			}
			catch(std::exception &e)
			{
				Log::Instance.LogErr("ScriptedWorldStreamerCallback", "OnMapChunkLoad: Scripting Error: '%s'", e.what());

				return false;
			};
		};

		return false;
	};

	void OnMapChunkUnload(WorldChunk *Chunk, StateCollection *State)
	{
		if(ChunkUnloadFunction)
		{
			try
			{
				ChunkUnloadFunction(Chunk);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogErr("ScriptedWorldStreamerCallback", "OnMapChunkUnload: Scripting Error: '%s'", e.what());
			};
		};
	};

	void OnMapChunkCoordinateUpdate(WorldChunk *Chunk)
	{
		if(ChunkCoordinateUpdateFunction)
		{
			try
			{
				ChunkCoordinateUpdateFunction(Chunk);
			}
			catch(std::exception &e)
			{
				Log::Instance.LogErr("ScriptedWorldStreamerCallback", "OnMapChunkCoordinateUpdate: Scripting Error: '%s'", e.what());
			};
		};
	};

	const Vector3 &ChunkSize()
	{
		if(ChunkSizeFunction)
		{
			try
			{
				return ProtectedLuaCast<Vector3>(ChunkSizeFunction());
			}
			catch(std::exception &e)
			{
				Log::Instance.LogErr("ScriptedWorldStreamerCallback", "ChunkSize: Scripting Error: '%s'", e.what());
			};
		};

		static Vector3 Zero;
		return Zero;
	};

	bool ChunkNeedsLoad(const Vector3 &GlobalCoordinate)
	{
		if(ChunkNeedsLoadFunction)
		{
			try
			{
				return ProtectedLuaCast<bool>(ChunkNeedsLoadFunction(GlobalCoordinate));
			}
			catch(std::exception &e)
			{
				Log::Instance.LogErr("ScriptedWorldStreamerCallback", "ChunkNeedsLoadLoad: Scripting Error: '%s'", e.what());
			};
		};

		return false;
	};
};

class WorldStreamer
{
	friend class WorldChunk;
private:
	/*!
	*	Directions of the world for chunk indexing
	*/
	std::vector<Vector3> WorldDirections;

	/*!
	*	Load Cache to prevent us from re-checking for chunk loads on Update()
	*/
	std::vector<bool> ChunkLoadFlagCache;

	/*!
	*	World radius on chunks
	*/
	f32 WorldRadiusValue;

	/*!
	*	Loaded Chunk Cache
	*/
	std::vector<SuperSmartPointer<WorldChunk> > LoadedChunks;

	/*!
	*	Global Coordinate of this world
	*/
	Vector3 GlobalCoordinate;
public:
	/*!
	*	Client Callback
	*/
	WorldStreamerCallback *Callback;

	WorldStreamer() : Callback(NULL)
	{
		SetWorldRadius(1);
	};

	/*!
	*	Update the world looking for chunks to load
	*/
	void Update();

	/*!
	*	Unload all chunks
	*/
	void Reset();

	/*!
	*	Get the state collection associated to a chunk
	*	\param Chunk the chunk to check
	*	\param Create whether to create the state if it doesn't exist
	*	\return the state collection, or NULL
	*/
	StateCollection *GetChunkCollection(WorldChunk *Chunk, bool Create = false);

	/*!
	*	Unload a chunk
	*	\param Index the chunk to unload
	*	\note Will call the callback OnMapUnload
	*/
	void UnloadChunk(uint32 Index);

	/*!
	*	Clip a unit's position around the central chunk
	*	\param Position the unit's position
	*	\return the new unit position (or the same unchanged one)
	*/
	Vector3 ClipUnitPosition(const Vector3 &Position);

	/*!
	*	Get the chunk index from a local coordinate
	*	\param Coordinate the local coordinate of the chunk
	*	\return the chunk index or -1
	*/
	int32 GetChunkIndex(const Vector3 &Coordinate);

	/*!
	*	Translates the main chunk in a certain direction, effectively moving around the world
	*	\param Direction the direction to move to
	*/
	void TranslateMainChunk(const Vector3 &Direction);

	/*!
	*	\return the current world radius
	*/
	uint8 GetWorldRadius();

	/*!
	*	\param ChunkRadius how many chunks around the center we should load
	*/
	void SetWorldRadius(uint8 ChunkRadius);

	/*!
	*	Sets our current global coordinate
	*	\param NewGlobalCoordinate the new global coordinate
	*/
	void SetGlobalCoordinate(const Vector3 &NewGlobalCoordinate);

	/*!
	*	\return our current global coordinate
	*/
	const Vector3 &GetGlobalCoordinate();
};
