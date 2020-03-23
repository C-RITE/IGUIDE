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

private:
    
    IXAudio2*               pXAudio2;
    IXAudio2MasteringVoice* pMasteringVoice;
    IXAudio2SourceVoice*    pSourceVoice;

    bool                    m_audioAvailable;
    BYTE*                   m_soundData;

    HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
    HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition);

};

