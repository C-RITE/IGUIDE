#include "stdafx.h"
#include "SoundEffect.h"
#include "Resource.h"
#include <exception>

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

namespace DX {

    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DirectX API errors
            throw std::exception();
        }
    }
}

SoundEffect::SoundEffect() :
    m_audioAvailable(false),
    pXAudio2(NULL),
    pMasteringVoice(NULL),
    pSourceVoice(NULL)
{

    DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;

#ifdef _DEBUG
    eflags = eflags | DirectX::AudioEngine_Debug;
#endif

    DX::ThrowIfFailed(XAudio2Create(
        &pXAudio2, XAUDIO2_DEFAULT_PROCESSOR)
        );

    // Create a source voice for this sound effect. 
    DX::ThrowIfFailed(
        pXAudio2->CreateMasteringVoice(&pMasteringVoice)
        );

}

SoundEffect::~SoundEffect() {

    delete m_soundData;
    
}

//---------------------------------------------------------------------- 
/*
void SoundEffect::Initialize(
        IXAudio2* masteringEngine,
        WAVEFORMATEX* sourceFormat,
        BYTE* soundData)
{
    m_soundData = soundData;

    if (masteringEngine == nullptr)
    {
        // Audio is not available so just return. 
     //   m_audioAvailable = false;
       // return;


        DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
#ifdef _DEBUG
        eflags = eflags | DirectX::AudioEngine_Debug;
#endif

        DirectX::AudioEngine ae(eflags);
        masteringEngine = ae.GetInterface();

    
    // Create a source voice for this sound effect. 
    DX::ThrowIfFailed(
        masteringEngine->CreateSourceVoice(
            &m_sourceVoice,
            &wfx
            )
        );
    m_audioAvailable = true;

    }
    
}
*/
//---------------------------------------------------------------------- 

LPVOID SoundEffect::LoadWavFromResource(int IDRESOURCE) {

    // Loading WAVE file as a resource
    HRSRC hResInfo;
    HGLOBAL hResData;
    DWORD dwSize;
    
    hResInfo = FindResourceW(NULL, MAKEINTRESOURCE(IDRESOURCE), L"WAVE");
    hResData = LoadResource(GetModuleHandle(NULL), hResInfo);
    dwSize = SizeofResource(GetModuleHandle(NULL), hResInfo);
    
    return LockResource(hResData);

}

void SoundEffect::PlaySound()
{
   

    //if (!m_audioAvailable)
    //{
    //    // Audio is not available so just return. 
    //    return;
    //}

    // Interrupt sound effect if it is currently playing. 
    //DX::ThrowIfFailed(
    //    m_sourceVoice->Stop()
    //    );
    //DX::ThrowIfFailed(
    //    m_sourceVoice->FlushSourceBuffers()
    //    );


    

    // populate WAVEFORMAT structure
    WAVEFORMATEX wfx = { 0 };

    DWORD dwChunkSize;
    DWORD dwChunkPosition;

    LPVOID wavResource = LoadWavFromResource(IDR_WAVE3);

    FindChunk(wavResource, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData(wavResource, &wfx, dwChunkSize, dwChunkPosition);

    // populate audio buffer
    XAUDIO2_BUFFER buffer = { 0 };

    FindChunk(wavResource, fourccDATA, dwChunkSize, dwChunkPosition);
    
    m_soundData = new BYTE[dwChunkSize];

    ReadChunkData(wavResource, m_soundData, dwChunkSize, dwChunkPosition);

    buffer.AudioBytes = dwChunkSize;
    buffer.pAudioData = m_soundData;
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    // Create a source voice for this sound effect. 
    DX::ThrowIfFailed(
        pXAudio2->CreateSourceVoice(
            &pSourceVoice,
            &wfx
            )
        );

    // Queue the memory buffer for playback and start the voice. 
        pSourceVoice->SubmitSourceBuffer(&buffer);

        DX::ThrowIfFailed(
            pSourceVoice->Start()
        );

}

HRESULT SoundEffect::FindChunk(LPVOID RIFF, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

    BYTE* riff = static_cast<BYTE*>(RIFF);
	
    memcpy(&dwChunkType, riff, sizeof(DWORD));
    dwOffset += sizeof(DWORD);
	memcpy(&dwChunkDataSize, riff + dwOffset, sizeof(DWORD));

	switch (dwChunkType)
	{
	case fourccRIFF:
		dwRIFFDataSize = dwChunkDataSize;
		dwChunkDataSize = 4;
		break;
	}

	while (bytesRead < dwRIFFDataSize) {

		memcpy(&dwChunkType, riff + dwOffset, sizeof(DWORD));

		if (dwChunkType == fourcc)
		{
			switch (fourcc) {

			case fourccFMT:
				dwChunkSize = dwChunkDataSize * 4;
				dwChunkDataPosition = dwOffset + dwChunkDataSize * 2;
				break;

			case fourccDATA:
                memcpy(&dwChunkSize, riff + dwOffset + sizeof(DWORD), dwChunkDataSize);
				dwChunkDataPosition = dwOffset + sizeof(DWORD) * 2;
				break;

			}

     		return S_OK;

		}

		dwOffset += sizeof(DWORD);
		bytesRead += dwChunkDataSize;

	}

	return S_OK;

}

HRESULT SoundEffect::ReadChunkData(LPVOID RIFF, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    BYTE* riff = static_cast<BYTE*>(RIFF);
    memcpy(buffer, riff + bufferoffset, buffersize);

    return S_OK;

}