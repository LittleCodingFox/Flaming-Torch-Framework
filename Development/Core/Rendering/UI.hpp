#pragma once

class Renderer;
class UIRenderer;

class UIRootWidget : public TBWidget
{
public:
	virtual void OnInvalid();
private:
	UIRenderer *Owner;
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
	Renderer *Owner;
	VertexBufferHandle VertexHandle;
	bool FirstBatch; //For Debugging

	UIRenderer();
	virtual void BeginPaint(int render_target_w, int render_target_h) override;
	virtual void EndPaint() override;

	virtual TBBitmap *CreateBitmap(int width, int height, uint32 *data) override;

	virtual void RenderBatch(Batch *batch) override;
	virtual void SetClipRect(const TBRect &rect) override;
};

class UIInputProcessor : public InputCenter::Context
{
public:
	MODIFIER_KEYS CurrentModifiers;

	UIInputProcessor();
	bool OnKey(const InputCenter::KeyInfo &Key) override;
	bool OnTouch(const InputCenter::TouchInfo &Touch) override;
	bool OnMouseButton(const InputCenter::MouseButtonInfo &Button) override;
	bool OnJoystickButton(const InputCenter::JoystickButtonInfo &Button) override;
	bool OnJoystickAxis(const InputCenter::JoystickAxisInfo &Axis) override;
	void OnJoystickConnected(uint8 Index) override;
	void OnJoystickDisconnected(uint8 Index) override;
	void OnMouseMove(const Vector2 &Position) override;
	void OnCharacterEntered(wchar_t Character) override;
	void OnAction(const InputCenter::Action &TheAction) override;
	void OnGainFocus() override;
	void OnLoseFocus() override;
};
