#pragma once

class Renderer;
class UIRenderer;

class UIRootWidget : public TBWidget
{
public:
	virtual void OnInvalid();
};

class UIBitmap : public TBBitmap
{
public:
	DisposablePointer<Texture> ContainedTexture;
	UIRenderer *Owner;

	~UIBitmap();
	bool Init(int width, int height, uint32 *data);
	virtual int Width() override;
	virtual int Height() override;
	virtual void SetData(uint32 *data) override;
};

class UIRenderer : public TBRendererBatcher
{
public:
	VertexBufferHandle VertexHandle;
	bool FirstBatch; //For Debugging

	UIRenderer();
	virtual void BeginPaint(int render_target_w, int render_target_h) override;
	virtual void EndPaint() override;

	virtual TBBitmap *CreateBitmap(int width, int height, uint32 *data) override;

	virtual void RenderBatch(Batch *batch) override;
	virtual void SetClipRect(const TBRect &rect) override;
};

class UIInputProcessor : public Input::Context
{
public:
	struct ClickCounter
	{
		uint32 Count;
		uint64 ClickTimer;

		ClickCounter() : Count(0), ClickTimer(GameClockTimeNoPause()) {}
	};

	typedef std::map<uint8, ClickCounter> MouseClickMap;
	
	MouseClickMap MouseClicks;

	MODIFIER_KEYS CurrentModifiers;

	UIInputProcessor();
	bool OnKey(const Input::KeyInfo &Key) override;
	bool OnTouch(const Input::TouchInfo &Touch) override;
	bool OnMouseButton(const Input::MouseButtonInfo &Button) override;
	bool OnJoystickButton(const Input::JoystickButtonInfo &Button) override;
	bool OnJoystickAxis(const Input::JoystickAxisInfo &Axis) override;
	void OnJoystickConnected(uint8 Index) override;
	void OnJoystickDisconnected(uint8 Index) override;
	void OnMouseMove(const Vector2 &Position) override;
	void OnCharacterEntered(uint32 Character) override;
	void OnAction(const Input::Action &TheAction) override;
	void OnGainFocus() override;
	void OnLoseFocus() override;
};
