#include "stdafx.h"
#include "Sfx.h"
#include "Resource.h"

using namespace DirectX;

void Sfx::initSound() {

	// Create DirectXTK for Audio objects
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif

	m_audEngine = std::make_unique<AudioEngine>(eflags);

	m_audioEvent = 0;
	m_audioTimerAcc = 10.f;
	m_retryDefault = false;

	// Loading WAVE file as a resource
	HRSRC hResInfo;
	HGLOBAL hResData;
	DWORD dwSize;
	VOID* pvRes;
	hResInfo = FindResourceW(NULL, MAKEINTRESOURCE(IDR_WAVE1), L"WAVE");
	hResData = LoadResource(GetModuleHandle(NULL), hResInfo);
	dwSize = SizeofResource(GetModuleHandle(NULL), hResInfo);
	pvRes = LockResource(hResData);

	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 22050;

	CHAR* m_pResourceBuffer = new CHAR[dwSize];
	memcpy(m_pResourceBuffer, pvRes, dwSize);

	m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"");
	
	m_effect1 = m_soundEffect->CreateInstance();

	m_effect1->Play(true);

}