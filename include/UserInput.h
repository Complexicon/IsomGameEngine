#ifndef USERINPUTHDR
#define USERINPUTHDR

#define CAST(to, var) (*((to*)(&var)))

#include "KeysEnum.h"

enum class UserInputType{
	Mouse,
	Keyboard
};

enum class MouseEventType{
	Move,
	Click,
};

enum class MouseButton{
	Left,
	Middle,
	Right
};

class UserInputEvent{
	public:
		UserInputType type;
		UserInputEvent(UserInputType type) : type(type) {}
};

class KeyboardInputEvent : public UserInputEvent{
	public:
		bool pressed;
		Key key;
		KeyboardInputEvent(Key key, bool pressed) : pressed(pressed), key(key), UserInputEvent(UserInputType::Keyboard){}
};

class KeyDownEvent : public KeyboardInputEvent {
	public: KeyDownEvent(Key key) : KeyboardInputEvent(key, true) {}
};

class KeyUpEvent : public KeyboardInputEvent {
	public: KeyUpEvent(Key key) : KeyboardInputEvent(key, false) {}
};

class MouseInputEvent : public UserInputEvent{
	public:
		MouseEventType mouseEvent;
		MouseInputEvent(MouseEventType type) : mouseEvent(type), UserInputEvent(UserInputType::Mouse) {}
};

class MouseMoveEvent : public MouseInputEvent {
	public:
		int mouseX, mouseY;
		MouseMoveEvent(int x, int y) : mouseX(x), mouseY(y), MouseInputEvent(MouseEventType::Move) {}
};

class MouseClickEvent : public MouseInputEvent {
	public:
		MouseButton button;
		bool pressed;
		int pressedX, pressedY;
		MouseClickEvent(MouseButton b, int x, int y, bool pressed) : button(b), pressed(pressed), pressedX(x), pressedY(y), MouseInputEvent(MouseEventType::Click) {}
};

#endif