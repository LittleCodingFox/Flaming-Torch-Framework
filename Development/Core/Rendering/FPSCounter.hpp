#pragma once
/*!
*	FPS Counter Subsystem
*/
class FPSCounter : public SubSystem
{
private:
	f32 Time;
	uint32 Counter;
	uint32 CurrentFPS;

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
public:
	FPSCounter() : SubSystem(FPSCOUNTER_PRIORITY), Time(0), Counter(0), CurrentFPS(0) {}

	/*!
	*	\return the Current Frames Per Second
	*/
	uint32 FPS();
};

extern FPSCounter g_FPSCounter;
