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


    // Loading WAVE file as a resource
    HRSRC hResInfo;
    HGLOBAL hResData;
    DWORD dwSize;
    VOID* pvRes;

    hResInfo = FindResourceW(NULL, MAKEINTRESOURCE(IDR_WAVE1), L"WAVE");
    hResData = LoadResource(GetModuleHandle(NULL), hResInfo);
    dwSize = SizeofResource(GetModuleHandle(NULL), hResInfo);
    pvRes = LockResource(hResData);

    // populate WAVEFORMAT structure
    WAVEFORMATEX wfx = { 0 };
    DWORD dwChunkSize;
    DWORD dwChunkPosition;

    wchar_t* strFileName = _T("res\\MDONE.WAV");

    // Open the file
    HANDLE hFile = CreateFile(
        strFileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
    ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

    // populate audio buffer
    XAUDIO2_BUFFER buffer = { 0 };

    FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
    m_soundData = new BYTE[dwChunkSize];
    ReadChunkData(hFile, m_soundData, dwChunkSize, dwChunkPosition);

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

HRESULT SoundEffect::FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
{

    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            dwChunkSize = dwChunkDataSize;
            dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;

}

HRESULT SoundEffect::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}