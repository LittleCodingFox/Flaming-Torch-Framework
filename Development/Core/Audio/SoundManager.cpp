#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_SOUND
#	define TAG "SoundManager"
	uint64 SoundManager::SoundCounter = 0, SoundManager::MusicCounter = 0;
	SoundManager SoundManager::Instance;

	SoundManager::Music::~Music()
	{
		Stop();
	}

	void SoundManager::Music::Play()
	{
		Impl->PlayMusic(Handle);
	}

	void SoundManager::Music::Stop()
	{
		Impl->StopMusic(Handle);
	}

	void SoundManager::Music::Pause()
	{
		Impl->PauseMusic(Handle);
	}

	void SoundManager::Music::SetVolume(f32 Volume)
	{
		Impl->SetMusicVolume(Handle, Volume);
	}

	f32 SoundManager::Music::GetVolume()
	{
		return Impl->GetMusicVolume(Handle);
	}

	uint8 SoundManager::Music::GetStatus()
	{
		return Impl->GetMusicStatus(Handle);
	}

	bool SoundManager::Music::IsPlaying()
	{
		return GetStatus() == SoundStatus::Playing;
	}

	void SoundManager::Music::SetLoop(bool Value)
	{
		Impl->SetMusicLooping(Handle, Value);
	}

	bool SoundManager::Music::IsLooping()
	{
		return Impl->MusicLooping(Handle);
	}

	void SoundManager::Music::SetPitch(f32 Pitch)
	{
		Impl->SetMusicPitch(Handle, Pitch);
	}

	f32 SoundManager::Music::GetPitch()
	{
		return Impl->MusicPitch(Handle);
	}

	SoundManager::Sound::~Sound()
	{
		Stop();
	}

	void SoundManager::Sound::Play()
	{
		Impl->PlaySound(Handle);
	}

	void SoundManager::Sound::Stop()
	{
		Impl->StopSound(Handle);
	}

	void SoundManager::Sound::Pause()
	{
		Impl->PauseSound(Handle);
	}

	void SoundManager::Sound::SetVolume(f32 Volume)
	{
		Impl->SetSoundVolume(Handle, Volume);
	}

	f32 SoundManager::Sound::GetVolume()
	{
		return Impl->GetSoundVolume(Handle);
	}

	uint8 SoundManager::Sound::GetStatus()
	{
		return Impl->GetSoundStatus(Handle);
	}

	bool SoundManager::Sound::IsPlaying()
	{
		return GetStatus() == SoundStatus::Playing;
	}

	void SoundManager::Sound::SetLoop(bool Value)
	{
		Impl->SetSoundLooping(Handle, Value);
	}

	bool SoundManager::Sound::IsLooping()
	{
		return Impl->SoundLooping(Handle);
	}

	void SoundManager::Sound::SetPitch(f32 Pitch)
	{
		Impl->SetSoundPitch(Handle, Pitch);
	}

	f32 SoundManager::Sound::GetPitch()
	{
		return Impl->SoundPitch(Handle);
	}

	DisposablePointer<SoundManager::Sound> SoundManager::GetSound(StringID ID)
	{
		if(!WasStarted)
		{
			return DisposablePointer<Sound>();
		}

		SoundMap::iterator it = Sounds.find(ID);

		if(it == Sounds.end())
		{
			return DisposablePointer<Sound>();
		}

		if(it->second.Get() == NULL)
		{
			Sounds.erase(it);

			return DisposablePointer<Sound>();
		}

		return it->second;
	}

	DisposablePointer<SoundManager::Music> SoundManager::GetMusic(StringID ID)
	{
		if(!WasStarted)
		{
			return DisposablePointer<Music>();
		}

		MusicMap::iterator it = Musics.find(ID);

		if(it == Musics.end())
		{
			return DisposablePointer<Music>();
		}

		if(it->second.Get() == NULL)
		{
			Musics.erase(it);

			return DisposablePointer<Music>();
		}

		return it->second;
	}

	StringID SoundManager::SoundFromStream(Stream *In)
	{
		if(!WasStarted)
		{
			return 0xFFFFFFFF;
		}

		StringID SoundID = MakeStringID("SOUND__STREAM__" + StringUtils::PointerString(In) + "_" + StringUtils::MakeIntString(SoundCounter));

		SoundMap::iterator sit = Sounds.find(SoundID);

		if(sit != Sounds.end())
		{
			return sit->first;
		}

		std::vector<uint8> Data((uint32)(In->Length() - In->Position()));

		if(!In->Read2<uint8>(&Data[0], Data.size()))
			return INVALID_FTGHANDLE;

		DisposablePointer<Sound> TheSound(new Sound());
		TheSound->Impl = Impl;
		TheSound->Handle = Impl->CreateSound(&Data[0], Data.size());

		if (TheSound->Handle == INVALID_FTGHANDLE)
			return INVALID_FTGHANDLE;

		Sounds[SoundID] = TheSound;

		SoundCounter++;

		return SoundID;
	}

	StringID SoundManager::SoundFromPackage(StringID Directory, StringID FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		DisposablePointer<Stream> PackageStream = PackageFileSystemManager::Instance.GetFile(Directory, FileName);

		if(PackageStream.Get() == NULL)
		{
			return 0xFFFFFFFF;
		}

		return SoundFromStream(PackageStream.Get());
	}

	StringID SoundManager::SoundFromFile(const char *FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		DisposablePointer<FileStream> Stream(new FileStream());

		if(!Stream->Open(FileName, StreamFlags::Read))
		{
			return 0xFFFFFFFF;
		}

		return SoundFromStream(Stream.Get());
	}

	StringID SoundManager::MusicFromStream(Stream *In)
	{
		if(!WasStarted)
		{
			return 0xFFFFFFFF;
		}

		std::stringstream str;
		str << "MUSIC__STREAM__" << std::hex << (uint64)In << GameClockTimeNoPause();

		StringID MusicID = MakeStringID("MUSIC__STREAM__" + StringUtils::PointerString(In) + "_" + StringUtils::MakeIntString(MusicCounter));

		MusicMap::iterator sit = Musics.find(MusicID);

		if(sit != Musics.end())
		{
			return sit->first;
		}

		DisposablePointer<Music> Out(new Music());

		std::vector<uint8> Data((uint32)(In->Length() - In->Position()));

		if (!In->Read2<uint8>(&Data[0], Data.size()))
			return INVALID_FTGHANDLE;

		Out->Impl = Impl;
		Out->Handle = Impl->CreateMusic(&Data[0], Data.size());

		if(Out->Handle == INVALID_FTGHANDLE)
			return INVALID_FTGHANDLE;

		Musics[MusicID] = Out;

		MusicCounter++;

		return MusicID;
	}

	StringID SoundManager::MusicFromPackage(StringID Directory, StringID FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		DisposablePointer<Stream> PackageStream = PackageFileSystemManager::Instance.GetFile(Directory, FileName);

		if(PackageStream.Get() == NULL)
		{
			return 0xFFFFFFFF;
		}

		return MusicFromStream(PackageStream.Get());
	}

	StringID SoundManager::MusicFromFile(const char *FileName)
	{
		if(!WasStarted)
			return 0xFFFFFFFF;

		DisposablePointer<FileStream> Stream(new FileStream());

		if(!Stream->Open(FileName, StreamFlags::Read))
		{
			return 0xFFFFFFFF;
		}

		return MusicFromStream(Stream.Get());
	}

	void SoundManager::Crossfade(DisposablePointer<Music> From, DisposablePointer<Music> To, uint64 Delay)
	{
		CrossfadeFrom = From;
		CrossfadeTo = To;
		CrossfadeTimer = GameClockTimeNoPause();
		CrossfadeDelay = Delay;

		CrossfadeTo->Stop();
	}

	void SoundManager::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "SoundManager Subsystem starting..");

		Impl.Reset(new DEFAULT_SOUNDMANAGER_IMPLEMENTATION());
	}

	void SoundManager::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo(TAG, "SoundManager Subsystem terminating..");

		while(Sounds.begin() != Sounds.end())
		{
			Sounds.begin()->second.Dispose();
			Sounds.erase(Sounds.begin());
		}

		while(Musics.begin() != Musics.end())
		{
			Musics.begin()->second.Dispose();
			Musics.erase(Musics.begin());
		}

		Impl.Dispose();

		SubSystem::Shutdown(Priority);
	}

	void SoundManager::Update(uint32 Priority)
	{
		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Cleanup();

		if (CrossfadeFrom.Get() || CrossfadeTo.Get())
		{
			if (!CrossfadeFrom.Get() || !CrossfadeTo.Get())
			{
				CrossfadeFrom = CrossfadeTo = DisposablePointer<Music>();

				return;
			}

			uint64 Difference = GameClockDiff(CrossfadeTimer);

			CrossfadeFrom->SetVolume(MathUtils::Clamp(1 - Difference / (f32)CrossfadeDelay));
			CrossfadeTo->SetVolume(MathUtils::Clamp(Difference / (f32)CrossfadeDelay));

			if (!CrossfadeFrom->IsPlaying())
				CrossfadeFrom->Play();

			if (!CrossfadeTo->IsPlaying())
				CrossfadeTo->Play();

			if (Difference >= CrossfadeDelay)
			{
				CrossfadeFrom->Stop();
				CrossfadeFrom = CrossfadeTo = DisposablePointer<Music>();
			}
		}
	}

	void SoundManager::Cleanup()
	{
		uint32 TotalObjects = Sounds.size() + Musics.size();

		for(SoundMap::iterator it = Sounds.begin(); it != Sounds.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.UseCount() == 1)
			{
				if (it->second.Get())
					it->second->Stop();

				Sounds.erase(it);
				it = Sounds.begin();

				if(it == Sounds.end())
					break;
			}

			if(it == Sounds.end())
				break;
		}

		for(MusicMap::iterator it = Musics.begin(); it != Musics.end(); it++)
		{
			while(it->second.Get() == NULL || it->second.UseCount() == 1)
			{
				if (it->second.Get())
					it->second->Stop();

				Musics.erase(it);
				it = Musics.begin();

				if(it == Musics.end())
					break;
			}

			if(it == Musics.end())
				break;
		}

		uint32 CurrentObjects = Sounds.size() + Musics.size();

		if(CurrentObjects != TotalObjects)
		{
			Log::Instance.LogDebug(TAG, "Cleared %d objects (Prev: %d/Now: %d)", TotalObjects - CurrentObjects, TotalObjects, CurrentObjects);
		}
	}
#endif
}
