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
	FPSCounter() : SubSystem(FPSCOUNTER_PRIORITY), Time(0), Counter(0), CurrentFPS(0) {}
public:
	static FPSCounter Instance;

	/*!
	*	\return the Current Frames Per Second
	*/
	uint32 FPS();

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
};
