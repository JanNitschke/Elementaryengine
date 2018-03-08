#include "UIElement.h"
#include "Game.h"


UIElement::UIElement()
{
	texture = new Texture("");
	Game::uiElements.push_back(this);
}


UIElement::~UIElement()
{

}
