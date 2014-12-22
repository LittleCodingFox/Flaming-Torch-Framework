#pragma once

class NULLSoundManagerImplementation : public ISoundManagerImplementation
{
public:
	MusicHandle CreateMusic(const void *Source, uint32 Length) override;
	SoundHandle CreateSound(const void *Source, uint32 Length) override;
	void DestroyMusic(MusicHandle Handle) override;
	void DestroySound(SoundHandle Handle) override;
	void SetMusicVolume(MusicHandle Handle, f32 Volume) override;
	void SetSoundVolume(SoundHandle Handle, f32 Volume) override;
	f32 GetMusicVolume(MusicHandle Handle) override;
	f32 GetSoundVolume(SoundHandle Handle) override;
	uint8 GetMusicStatus(MusicHandle Handle) override;
	uint8 GetSoundStatus(SoundHandle Handle) override;
	void SetMusicLooping(MusicHandle Handle, bool Loop) override;
	void SetSoundLooping(SoundHandle Handle, bool Loop) override;
	bool MusicLooping(MusicHandle Handle) override;
	bool SoundLooping(SoundHandle Handle) override;
	void SetMusicPitch(MusicHandle Handle, f32 Pitch) override;
	void SetSoundPitch(SoundHandle Handle, f32 Pitch) override;
	f32 MusicPitch(MusicHandle Handle) override;
	f32 SoundPitch(SoundHandle Handle) override;
	void PlayMusic(MusicHandle Handle) override;
	void PlaySound(SoundHandle Handle) override;
	void PauseMusic(MusicHandle Handle) override;
	void PauseSound(SoundHandle Handle) override;
	void StopMusic(MusicHandle Handle) override;
	void StopSound(SoundHandle Handle) override;
};
