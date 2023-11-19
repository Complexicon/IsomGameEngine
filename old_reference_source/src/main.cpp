#include <stdio.h>

#include "engine/Game.h"
#include "Map.h"

#define FLOATIFY(in, max) ((((float)in / max) * 2) - 1.f)
#define Width (game()->GetWindow()->width())
#define Height (game()->GetWindow()->height())
#define IN_BOX(x, y, szX, szY, bX, bY) (x - bX >= 0 && x - bX <= szX && y - bY >= 0 && y - bY <= szY)

#define VECPOS(name, vec) (string(name) + ": [x: " + std::to_string(vec.x) + ", y: " + std::to_string(vec.y) + "]\n")

#define PIXOFF(in) ((in - (int)in) * fieldSize)
#define PIXTOTILE(in) \
	{ (in.x + (int)PIXOFF(cam.x)) / fieldSize + (int)cam.x, (in.y + (int)PIXOFF(cam.y)) / fieldSize + (int)cam.y }

#define IN_BOUNDS(vec, r, szX, szY) (vec.x - r >= 0 && vec.x + r <= szX && vec.y - r >= 0 && vec.y + r <= szY)
#define CEILDIV(x, y) ((x + y - 1) / y)

class PauseScene : public IScene {

	IScene* prev;

	bool overContinue = false;
	bool overQuit = false;

  public:
	PauseScene(Game* owner, IScene* prev) : IScene(owner) { this->prev = prev; }

	void tick(float d) {}

	void render(Renderer* r) {
		prev->render(r);
		r->FillRect(0, 0, Width, Height, Color(0, 100));

		r->DrawRect(500, 500, 200, 100, Color(0xdddddd));
		if(overContinue) r->FillRect(500, 500, 200, 100, Color(0xadadad, 150));
		r->DrawString(500, 500, 200, 100, Color(0xffffff), "Continue");

		r->DrawRect(500, 650, 200, 100, Color(0xdddddd));
		if(overQuit) r->FillRect(500, 650, 200, 100, Color(0xadadad, 150));
		r->DrawString(500, 650, 200, 100, Color(0xffffff), "Quit");
	}

	void userinput(const UserInputEvent& e) {
		switch(e.type) {
		case UserInputType::Mouse:
			if(CAST(MouseInputEvent, e).mouseEvent == MouseEventType::Move) {
				MouseMoveEvent me = CAST(MouseMoveEvent, e);
				overContinue = IN_BOX(me.mouseX, me.mouseY, 200, 100, 500, 500);
				overQuit = IN_BOX(me.mouseX, me.mouseY, 200, 100, 500, 650);
			} else {
				MouseClickEvent me = CAST(MouseClickEvent, e);
				if(!me.pressed && me.button != MouseButton::Left) return;
				if(overContinue) game()->activeScene = prev;
				else if(overQuit)
					game()->stop();
			}
			return;
		case UserInputType::Keyboard:
			KeyboardInputEvent ke = CAST(KeyboardInputEvent, e);
			if(!ke.pressed) return;
			switch(ke.key) {
			case Key::ESCAPE: game()->activeScene = prev; return;
			default: return;
			}
			return;
		}
	}
};

class DebugScene : public IScene {

	Map m;
	int fieldSize;
	int fieldsOnScreenX;
	int fieldsOnScreenY;
	fV2D cam;

	fV2D mouse;
	iV2D mouseReal;
	iV2D selectedTile;
	bool draw = false;

	// Layer background;
	// Layer object;
	// Layer userInterface;

	Sprite* map = 0;

  public:
	DebugScene(Game* owner) : IScene(owner) {
		fieldSize = 55;
		m.Generate();
		map = owner->renderer->CreateSprite(m.GetMapWidth(), m.GetMapHeight());
		regenmap();
		zoom();
	}

	void regenmap() {
		auto r = map->target();
		r->BeginDraw();
		for(int y = 0; y < m.GetMapHeight(); y++) {
			for(int x = 0; x < m.GetMapWidth(); x++) {
				switch(m.Get(x, y)) {
				case Tile::HOVER: r->FillRect(x, y, 1, 1, Color(0xffffff, 50)); break;
				case Tile::GRASS: r->FillRect(x, y, 1, 1, Color(0x088c21)); break;
				case Tile::WATER: r->FillRect(x, y, 1, 1, Color(0x084a8c)); break;
				case Tile::DIRT: r->FillRect(x, y, 1, 1, Color(0x664b02)); break;
				case Tile::SAND: r->FillRect(x, y, 1, 1, Color(0xd6b458)); break;
				case Tile::GRAVEL: r->FillRect(x, y, 1, 1, Color(0x73716c)); break;
				case Tile::SNOW: r->FillRect(x, y, 1, 1, Color(0xd1d0cd)); break;
				case Tile::NONE: break;
				default: r->FillRect(x, y, 1,1, Color(0x00ff00, 50)); break;
				}
			}
		}
		r->EndDraw();
	}

	int tickCtr = 0;

	void zoom(bool in = false) {
		if(!in && fieldSize <= 10) return; // temp smallest cap
		fieldSize += in ? 5 : -5;
		fieldsOnScreenX = CEILDIV(Width, fieldSize);
		fieldsOnScreenY = CEILDIV(Height, fieldSize);
	}

	void resize() {
		fieldsOnScreenX = CEILDIV(Width, fieldSize);
		fieldsOnScreenY = CEILDIV(Height, fieldSize);
	}

	void tick(float delta) {

		if(mouse.x > 0.8 || mouse.x < -0.8) cam.x += mouse.x / 2;
		if(mouse.y > 0.7 || mouse.y < -0.7) cam.y += mouse.y / 2;

		selectedTile = PIXTOTILE(mouseReal);

		tickCtr++;
	}

	void render(Renderer* r) {

		// TODO: LAYERS

		r->Clear(Color(0x111111));

		fV2D tmp = cam; // tearing fix
		fV2D pxOff = {PIXOFF(tmp.x), PIXOFF(tmp.y)};

		r->DrawSprite(0 - (int)tmp.x * fieldSize - pxOff.x, 0 - (int)tmp.y * fieldSize - pxOff.y, fieldSize * m.GetMapWidth(), fieldSize * m.GetMapHeight(), map);

		r->FillRect((selectedTile.x - (int)tmp.x) * fieldSize - pxOff.x, (selectedTile.y - (int)tmp.y) * fieldSize - pxOff.y,
					fieldSize + 1, fieldSize + 1, Color(0x00ff00, 50));

		string debugText;
		debugText += string("ticks passed: ") + std::to_string(tickCtr) + "\n";
		debugText += VECPOS("cam", tmp);
		debugText += VECPOS("mouse", mouse);
		debugText += VECPOS("mouse selected tile", selectedTile);
		debugText += string("drawing") + std::to_string(draw);

		game()->additionalDebugInfo = debugText;
	}

	void userinput(const UserInputEvent& e) {
		switch(e.type) {
		case UserInputType::Mouse:
			if(CAST(MouseInputEvent, e).mouseEvent == MouseEventType::Move) {
				MouseMoveEvent me = CAST(MouseMoveEvent, e);
				mouse = {FLOATIFY(me.mouseX, Width), FLOATIFY(me.mouseY, Height)};
				mouseReal = {me.mouseX, me.mouseY};
			} else {
				MouseClickEvent me = CAST(MouseClickEvent, e);
				if(me.button != MouseButton::Left) return;
				draw = me.pressed;
			}
			return;
		case UserInputType::Keyboard:
			KeyboardInputEvent ke = CAST(KeyboardInputEvent, e);
			if(!ke.pressed) return;
			switch(ke.key) {
			case Key::OEM_PLUS: zoom(true); return;
			case Key::OEM_MINUS: zoom(); return;
			case Key::LBUTTON: draw = !draw; return;
			case Key::CONTROL: game()->debug = !game()->debug; return;
			case Key::ESCAPE: game()->activeScene = new PauseScene(game(), this); return;
			case Key::R: m.Generate(); regenmap(); return;
			default: return;
			}
			return;
		}
	}
};

class CastleSim : public Game {

  public:
	CastleSim() : Game("", true) {

		targetFPS = 144;
		targetTPS = 64;

		activeScene = new DebugScene(this);
	}
};

int main(int argc, char const* argv[]) {

	CastleSim().start();

	return 0;
}
