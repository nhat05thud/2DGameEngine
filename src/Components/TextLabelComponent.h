#ifndef TEXTLABELCOMPONENT_H
#define TEXTLABELCOMPONENT_H

#include <string>
#include <glm/glm.hpp>
#include <SDL.h>

struct TextLabelComponent
{
	glm::vec2 position;
	std::string text;
	std::string assetId;
	SDL_Color color;
	bool isFixed;

	TextLabelComponent(glm::vec2 position = glm::vec2(0), std::string text = "", std::string assetId = "", SDL_Color color = {0, 0, 0}, bool isFixed = true) {
		this->position = position;
		this->assetId = assetId;
		this->text = text;
		this->color = color;
		this->isFixed = isFixed;
	}
};


#endif