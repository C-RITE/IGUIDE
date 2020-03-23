#pragma once

#include <xaudio2.h>
#include "Audio.h"

class SoundEffect
{
public:

    SoundEffect();
    ~SoundEffect();
  /*  void Initialize(
       IXAudio2* masteringEngine,
       WAVEFORMATEX* sourceFormat,
       BYTE* soundData);*/

    void PlaySound();
    LPVOID LoadWavFromResource(int IDRESOURCE);

private:
    
    IXAudio2*               pXAudio2;
    IXAudio2MasteringVoice* pMasteringVoice;
    IXAudio2SourceVoice*    pSourceVoice;

    BYTE*                   m_soundData;
    bool                    m_audioAvailable;

    HRESULT ReadChunkData(LPVOID RIFF, void* buffer, DWORD buffersize, DWORD bufferoffset);
    HRESULT FindChunk(LPVOID RIFF, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);

};

