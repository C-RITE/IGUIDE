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

	m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"C:\\Users\\Michen\\source\\repos\\IGUIDE\\IGUIDE\\res\\mloop1.wav");
	m_effect1 = m_soundEffect->CreateInstance();

	m_effect1->Play(true);
	
}