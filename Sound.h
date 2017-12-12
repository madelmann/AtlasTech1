#ifndef _SOUND_H
#define _SOUND_H


#include <al.h>
#include <alc.h>
#include <alut.h>
#include "Framework.h"
#include "Main.h"


#define MAX_SOUNDS	32


class CSound
{
public:
	void DestroySound();
	bool LoadSound(char* Filename, bool looping);
	void PauseSound();
	void PlaySound();
	void SetPosition(CVector3 pos);
	void SetSourceRelative();
	void SetVelocity(CVector3 vel);
	void StopSound();

	bool			bInUse;
	char			Name[255];
	ALCdevice*		Device;
	CVector3		vPosition;

private:
	char*			alBuffer;
	ALenum			alFormatBuffer;
	ALsizei			alFreqBuffer;
	long			alBufferLen;
	ALboolean		alLoop;
	unsigned int	alSource;
	unsigned int	alBufferDataData;
	CVector3		vVelocity;
};

class CSoundManager
{
public:
	CSoundManager();
	~CSoundManager();
	int Add(char Filename[255], bool bLooping);
	void Clear();
	int IndexOf(char Name[255]);
	void Manage();
	bool PlaySoundByID(int id);
	bool PlaySoundByName(char Name[255]);
	void SetListenerPosition(float x, float y, float z);
	void SetListenerOrientation(float fx, float fy, float fz, float ux, float uy, float uz);

	int Count;
	CSound Repository[MAX_SOUNDS];
	vector<CSound> ActiveSounds;
	ALCcontext *Context;
	ALCdevice *Device;
};

#endif