#pragma once
#include "Audio.h"

class Sfx
{
public:

	void					initSound();

    // DirectXTK for Audio objects.
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;

    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;
    bool                                                                    m_retryDefault;

};

