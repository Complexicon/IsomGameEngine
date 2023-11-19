#pragma once
class Renderer;
class Layer {
  public:
	Layer();
	void select();
	void render(Renderer*);
};