#pragma once

class Noise
{
private:
	int m_frequency;
	int m_mode;

public:
	void set(int mode, int frequency);
	void cut();
	void getFocus();
	void play();
};

inline Noise gNoise;