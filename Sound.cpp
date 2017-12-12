#include "Main.h"


void CSound::DestroySound()
{
	alDeleteSources(1, &alSource);
	alDeleteBuffers(1, &alBufferDataData);
}

bool CSound::LoadSound(char* Filename, bool looping)
{
	if(strlen(Filename) == 0)
		return false;

	FILE *pFile;
	char fname[255] = "";

	//Filename = RemoveFileExt(Filename);

	sprintf(fname, "%sMaps\\%s\\Audio\\%s", g_sGameName, Game.sMapName, Filename);
	
	if(fopen_s(&pFile, fname, "rb") != NULL)
	{
		sprintf_s(fname, "%sAudio\\%s", g_sGameName, Filename);

		if(fopen_s(&pFile, fname, "rb") != NULL)
		{
			char buffer[255] = "";
			sprintf(buffer, "ERROR[Sound.Load]: File '%s' not found!\n", Filename);
			Console->Output(buffer);

			return false;
		}
	}
	fclose(pFile);

	ALboolean loop;
	loop = looping;
	alutLoadWAVFile(fname, &alFormatBuffer, (void **)&alBuffer, (ALsizei *)&alBufferLen, &alFreqBuffer, &loop);
	//alutLoadMemoryFromFile(fname, &alFormatBuffer, (ALsizei *)&alBufferLen, &alFreqBuffer);

	ALenum alError = alGetError();
	ALenum alcError = alcGetError(Device);
	ALenum alutError = alutGetError();
	char buffer[255] = "";

	if(alError != AL_NO_ERROR || alcError != ALC_NO_ERROR || alutError != ALUT_ERROR_NO_ERROR)
	{
		char errstr[80] = "";
		switch(alutError) //error type
		{
		case ALUT_ERROR_OUT_OF_MEMORY:
			sprintf(errstr, "ALUT_ERROR_OUT_OF_MEMORY");
			break;
		case ALUT_ERROR_INVALID_OPERATION:
			sprintf(errstr, "ALUT_ERROR_INVALID_OPERATION");
			break;
		case ALUT_ERROR_NO_CURRENT_CONTEXT:
			sprintf(errstr, "ALUT_ERROR_NO_CURRENT_CONTEXT");
			break;
		case ALUT_ERROR_AL_ERROR_ON_ENTRY:
			sprintf(errstr, "ALUT_ERROR_AL_ERROR_ON_ENTRY");
			break;
		case ALUT_ERROR_ALC_ERROR_ON_ENTRY:
			sprintf(errstr, "ALUT_ERROR_ALC_ERROR_ON_ENTRY");
			break;
		case ALUT_ERROR_GEN_BUFFERS:
			sprintf(errstr, "ALUT_ERROR_GEN_BUFFERS");
			break;
		case ALUT_ERROR_BUFFER_DATA:
			sprintf(errstr, "ALUT_ERROR_BUFFER_DATA");
			break;
		case ALUT_ERROR_IO_ERROR:
			sprintf(errstr, "ALUT_ERROR_IO_ERROR");
			break;
		case ALUT_ERROR_UNSUPPORTED_FILE_TYPE:
			sprintf(errstr, "ALUT_ERROR_UNSUPPORTED_FILE_TYPE");
			break;
		case ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE:
			sprintf(errstr, "ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE");
			break;
		case ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA:
			sprintf(errstr, "ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA");
			break;
		}

		sprintf(buffer, "ERROR[LoadSound]: OpenAL error %s\n", errstr);
		Console->Output(buffer);

		return false;
	}

	alGenSources(1, &alSource);
	alGenBuffers(1, &alBufferDataData);
	alBufferData(alBufferDataData, alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);
	alSourcei(alSource, AL_BUFFER, alBufferDataData);

	alutUnloadWAV(alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);			

	//set the pitch
	alSourcef(alSource, AL_PITCH, 1.0f);
	//set the gain
	alSourcef(alSource, AL_GAIN, 1.0f);
	//set looping to true
	alSourcei(alSource, AL_LOOPING, loop);

	sprintf(Name, "%s", Filename);

	return true;
}

void CSound::PauseSound()
{
	alSourcePause(alSource);
}

void CSound::PlaySound()
{
	bInUse = true;
	alSourcePlay(alSource);
}

void CSound::SetPosition(CVector3 pos)
{
	vPosition = pos;
	alSource3f(alSource, AL_POSITION, vPosition.x, vPosition.y, vPosition.z);
}

//this function makes a sound source relative so all direction and velocity
//parameters become relative to the source rather than the listener
//useful for background music that you want to stay constant relative to the listener
//no matter where they go
void CSound::SetSourceRelative()
{
	alSourcei(alSource, AL_SOURCE_RELATIVE, AL_TRUE);
}

void CSound::SetVelocity(CVector3 vel)
{
	vVelocity = vel;
	alSource3f(alSource, AL_VELOCITY, vVelocity.x, vVelocity.y, vVelocity.z);
}

void CSound::StopSound()
{
	bInUse = false;
	alSourceStop(alSource);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


CSoundManager::CSoundManager()
{
	// Initialize Open AL
	Device = alcOpenDevice(NULL); // open default device
	if(Device != NULL)
	{
		Context = alcCreateContext(Device, NULL); // create context
		if(Context != NULL)
			alcMakeContextCurrent(Context); // set active context
	}

	// Initialize Framework
	//ALFWInit();

	//if(!ALFWInitOpenAL())
	//{
	//	ALFWShutdown();
	//	return;
	//}


	//alutInit(NULL, 0);


	//Device = alcOpenDevice("DirectSound3D");
	//if(Device == NULL)
	//{
	//  exit(-1);
	//}

	////Create context(s)
	//Context = alcCreateContext(Device,NULL);

	////Set active context
	//alcMakeContextCurrent(Context);


	ALenum alError = alGetError();
	ALenum alcError = alcGetError(Device);
	ALenum alutError = alutGetError();

	if(alError != AL_NO_ERROR || alcError != ALC_NO_ERROR || alutError != ALUT_ERROR_NO_ERROR)
	{
		char errstr[80] = "";
		switch(alutError) //error type
		{
		case ALUT_ERROR_OUT_OF_MEMORY:
			sprintf(errstr, "ALUT_ERROR_OUT_OF_MEMORY");
			break;
		case ALUT_ERROR_INVALID_OPERATION:
			sprintf(errstr, "ALUT_ERROR_INVALID_OPERATION");
			break;
		case ALUT_ERROR_NO_CURRENT_CONTEXT:
			sprintf(errstr, "ALUT_ERROR_NO_CURRENT_CONTEXT");
			break;
		case ALUT_ERROR_AL_ERROR_ON_ENTRY:
			sprintf(errstr, "ALUT_ERROR_AL_ERROR_ON_ENTRY");
			break;
		case ALUT_ERROR_ALC_ERROR_ON_ENTRY:
			sprintf(errstr, "ALUT_ERROR_ALC_ERROR_ON_ENTRY");
			break;
		case ALUT_ERROR_GEN_BUFFERS:
			sprintf(errstr, "ALUT_ERROR_GEN_BUFFERS");
			break;
		case ALUT_ERROR_BUFFER_DATA:
			sprintf(errstr, "ALUT_ERROR_BUFFER_DATA");
			break;
		case ALUT_ERROR_IO_ERROR:
			sprintf(errstr, "ALUT_ERROR_IO_ERROR");
			break;
		case ALUT_ERROR_UNSUPPORTED_FILE_TYPE:
			sprintf(errstr, "ALUT_ERROR_UNSUPPORTED_FILE_TYPE");
			break;
		case ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE:
			sprintf(errstr, "ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE");
			break;
		case ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA:
			sprintf(errstr, "ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA");
			break;
		}

		char buffer[255] = "";
		sprintf(buffer, "ERROR[SoundManager.Init]: OpenAL error '%s'\n", errstr);
		Console->Output(buffer);

		return;
	}

	// Clear Error Code
	alGetError();
	alcGetError(Device);
	alutGetError();

	Count = 0;
}

CSoundManager::~CSoundManager()
{
	ALFWShutdownOpenAL();

	ALFWShutdown();

	alutExit();

	Count = 0;
}

int CSoundManager::Add(char Filename[255], bool bLooping)
{
	if(Count >= MAX_SOUNDS)
	{
		Console->Output("ERROR[g_Manager_Sound.Add]: Max sounds reached!\n");
		return -1;
	}

	if(strcmp(Filename, "") == 0)
		return -1;

	int index = IndexOf(Filename);
	if(index != -1)
		return index;

	Repository[Count].Device = Device;
	if(!Repository[Count].LoadSound(Filename, bLooping))
		return -1;

	Repository[Count].SetSourceRelative();

	Count += 1;

	return Count - 1;
}

void CSoundManager::Clear()
{
	for(int i = Count - 1; i >= 0; i -= 1)
	{
		Repository[i].StopSound();
		Repository[i].DestroySound();
	}

	Count = 0;
}

int CSoundManager::IndexOf(char Name[255])
{
	if(strcmpi(Name, "") == 0)
		return -1;

	//for(int i = 0; i < Count; i += 1)
	for(int i = Count - 1; i >= 0; i -= 1)
	{
		if(strcmpi(Name, Repository[i].Name) == 0)
			return i;
	}

	return -1;
}

void CSoundManager::Manage()
{
	if(ActiveSounds.size() > 0)
	{
		for(unsigned int i = 0; i < ActiveSounds.size(); i += 1)
		{
			//if(ActiveSounds[i].
		}
	}
}

bool CSoundManager::PlaySoundByID(int id)
{
	if(id < 0 || id >= Count)
		return false;

	ActiveSounds.push_back(Repository[id]);
	
	int i = ActiveSounds.size() - 1;
	ActiveSounds[i].PlaySound();

	return true;
}

bool CSoundManager::PlaySoundByName(char Name[255])
{
	if(strcmpi(Name, "") == 0)
		return false;

	int id = IndexOf(Name);

	if(id == -1)
		return false;

	ActiveSounds.push_back(Repository[id]);
	
	int i = ActiveSounds.size() - 1;
	ActiveSounds[i].PlaySound();

	return true;
}

void CSoundManager::SetListenerPosition(float x, float y, float z)
{
	//set the position using 3 seperate floats
	alListener3f(AL_POSITION, x, y, z);
}

void CSoundManager::SetListenerOrientation(float fx, float fy, float fz, float ux, float uy, float uz)
{
	//set the orientation using an array of floats
	float vec[6];
	vec[0] = fx;
	vec[1] = fy;
	vec[2] = fz;
	vec[3] = ux;
	vec[4] = uy;
	vec[5] = uz;
	alListenerfv(AL_ORIENTATION, vec);
}