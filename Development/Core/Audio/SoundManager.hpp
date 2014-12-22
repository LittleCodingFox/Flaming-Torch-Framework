#pragma once
#if USE_SOUND
//TODO: Remake API
class ISoundManagerImplementation;

namespace SoundStatus
{
	enum SoundStatus
	{
		Stopped = 0,
		Playing,
		Paused
	};
};

class SoundManager : public SubSystem
{
private:
	DisposablePointer<ISoundManagerImplementation> Impl;

public:
	class Sound;
	class Music;

private:
	typedef std::map<StringID, DisposablePointer<Sound> > SoundMap;
	SoundMap Sounds;

	typedef std::map<StringID, DisposablePointer<Music> > MusicMap;
	MusicMap Musics;

	static uint64 SoundCounter, MusicCounter;
public:
	class Sound
	{
		friend class SoundManager;
	private:
		DisposablePointer<ISoundManagerImplementation> Impl;
		SoundHandle Handle;
	public:
		void Play();
		void Stop();
		void Pause();

		//Range 0-1
		void SetVolume(f32 Volume);
		f32 GetVolume();

		uint8 GetStatus();

		bool IsPlaying();

		void SetLoop(bool value);
		bool IsLooping();

		void SetPitch(f32 Pitch);
		f32 GetPitch();
	};

	class Music
	{
		friend class SoundManager;
	private:
		DisposablePointer<ISoundManagerImplementation> Impl;
		MusicHandle Handle;
	public:
		void Play();
		void Stop();
		void Pause();

		//Range 0-1
		void SetVolume(f32 Volume);
		f32 GetVolume();

		uint8 GetStatus();

		bool IsPlaying();

		void SetLoop(bool Value);
		bool IsLooping();

		void SetPitch(f32 Pitch);
		f32 GetPitch();
	};

	static SoundManager Instance;

	SoundManager() : SubSystem(SOUNDMANAGER_PRIORITY) {};

	DisposablePointer<Sound> GetSound(StringID ID);
	DisposablePointer<Music> GetMusic(StringID ID);

	//returns 0xFFFFFFFF on failure
	StringID SoundFromStream(Stream *In);
	StringID SoundFromPackage(StringID Directory, StringID FileName);
	StringID SoundFromFile(const char *FileName);
	StringID MusicFromStream(Stream *In);
	StringID MusicFromPackage(StringID Directory, StringID FileName);
	StringID MusicFromFile(const char *FileName);

	void Cleanup();

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
};

class ISoundManagerImplementation
{
public:
	virtual ~ISoundManagerImplementation() {};
	virtual MusicHandle CreateMusic(const void *Source, uint32 Length) = 0;
	virtual SoundHandle CreateSound(const void *Source, uint32 Length) = 0;
	virtual void DestroyMusic(MusicHandle Handle) = 0;
	virtual void DestroySound(SoundHandle Handle) = 0;
	virtual void SetMusicVolume(MusicHandle Handle, f32 Volume) = 0;
	virtual void SetSoundVolume(SoundHandle Handle, f32 Volume) = 0;
	virtual f32 GetMusicVolume(MusicHandle Handle) = 0;
	virtual f32 GetSoundVolume(SoundHandle Handle) = 0;
	virtual uint8 GetMusicStatus(MusicHandle Handle) = 0;
	virtual uint8 GetSoundStatus(SoundHandle Handle) = 0;
	virtual void SetMusicLooping(MusicHandle Handle, bool Loop) = 0;
	virtual void SetSoundLooping(SoundHandle Handle, bool Loop) = 0;
	virtual bool MusicLooping(MusicHandle Handle) = 0;
	virtual bool SoundLooping(SoundHandle Handle) = 0;
	virtual void SetMusicPitch(MusicHandle Handle, f32 Pitch) = 0;
	virtual void SetSoundPitch(SoundHandle Handle, f32 Pitch) = 0;
	virtual f32 MusicPitch(MusicHandle Handle) = 0;
	virtual f32 SoundPitch(SoundHandle Handle) = 0;
	virtual void PlayMusic(MusicHandle Handle) = 0;
	virtual void PlaySound(SoundHandle Handle) = 0;
	virtual void PauseMusic(MusicHandle Handle) = 0;
	virtual void PauseSound(SoundHandle Handle) = 0;
	virtual void StopMusic(MusicHandle Handle) = 0;
	virtual void StopSound(SoundHandle Handle) = 0;
};

#if USE_SFML_SOUNDMANAGER
#	define DEFAULT_SOUNDMANAGER_IMPLEMENTATION SFMLSoundManagerImplementation
#	include "Implementations/SFMLSoundManager.hpp"
#else
#	define DEFAULT_SOUNDMANAGER_IMPLEMENTATION NULLSoundManagerImplementation
#	include "Implementations/NULLSoundManager.hpp"
#endif

#endif
