#pragma once

class SoundManager : public SubSystem
{
private:
	typedef std::map<StringID, SuperSmartPointer<sf::SoundBuffer> > SoundBufferMap;
	SoundBufferMap SoundBuffers;

public:
	class Sound;
	class Music;

private:
	typedef std::map<StringID, SuperSmartPointer<Sound> > SoundMap;
	SoundMap Sounds;

	typedef std::map<StringID, SuperSmartPointer<Music> > MusicMap;
	MusicMap Musics;

public:
	class Sound
	{
		friend class SoundManager;
	private:
		sf::Sound TheSound;
	public:
		void Play();
		void Stop();
		void Pause();

		//Range 0-1
		void SetVolume(f32 Volume);
		f32 GetVolume();

		sf::Sound::Status GetStatus();

		bool IsPlaying();

		void SetLoop(bool value);
		bool IsLooping();
	};

	class Music
	{
		friend class SoundManager;
	private:
		sf::Music TheMusic;
		std::vector<uint8> Data;
	public:
		void Play();
		void Stop();
		void Pause();

		//Range 0-1
		void SetVolume(f32 Volume);
		f32 GetVolume();

		sf::Music::Status GetStatus();

		bool IsPlaying();

		void SetLoop(bool Value);
		bool IsLooping();

		void SetPitch(f32 Pitch);
		f32 GetPitch();
	};

	static SoundManager Instance;

	SoundManager() : SubSystem(SOUNDMANAGER_PRIORITY) {};

	SuperSmartPointer<Sound> GetSound(StringID ID);
	SuperSmartPointer<Music> GetMusic(StringID ID);

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