#include "noise.h"
#include "sound.h"

void Noise::set(int mode, int frequency)
{
	m_frequency = frequency;
	m_mode = mode;

	this->play();
}

void Noise::cut()
{
	m_mode = 0;
	playSound(SFX_StreamNoiseSSS2, 0);
	playSound(SFX_StreamNoiseSSS1, 0);
	playSound(SFX_HelicopterBladesSPS, 0);
}

void Noise::play()
{
	if (m_mode == 1)
	{
		//changeSoundFrequency(SFX_StreamNoiseSSS2, m_frequency);
		//changeSoundFrequency(SFX_StreamNoiseSSS1, m_frequency + 100);
		playSound(SFX_StreamNoiseSSS2, -1);
		playSound(SFX_StreamNoiseSSS1, -1);
	}
	else if (m_mode == 2)
		playSound(SFX_HelicopterBladesSPS, -1);
}