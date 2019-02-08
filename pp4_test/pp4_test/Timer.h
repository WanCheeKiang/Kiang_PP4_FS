#pragma once
class Timer
{

	double countsPerSec = 0.0;
	__int64 CounterStart = 0;

	int frameCount = 0;
	int fps = 0;

	__int64 frameTimeOld = 0;
	double frameTime;

public:
	Timer();
	~Timer();
	void StartTimer();
	double GetTime();
	double GetFrameTime();
};

