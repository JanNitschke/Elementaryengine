#include "EConsole.h"
#include <stdio.h>
#include <algorithm>
#include <Game.h>

EConsole::EConsole()
{
	header = EConsoleLine();
	header.frame = 0;
	header.time = 0;


}


EConsole::~EConsole()
{
}

void EConsole::Show()
{
	visible = true;
}

void EConsole::Hide()
{
	visible = false;
}

void EConsole::Update()
{
	posTop = Game::displaySettings->windowHeight - lineHeight;
	// %06.2f is 3 leading 0s and 2 numbers accuracy
	header.text = formatToString("Frame: %05u, Frametime %06.2f ms, FPS: %06.2f, PhysicsFPS: %06.2f ",Game::frameCount, Game::deltaTime * 1000, Game::smoothFps, Game::physicsFps);
	
	for each (ETextElement * te in textElements)
	{
		Game::textElements.erase(std::remove(Game::textElements.begin(), Game::textElements.end(), te), Game::textElements.end());
		delete te;
	}		
	textElements.clear();
	if (visible) {
	
		activeLines.clear();
		activeLines.push_back(header);
		for (int i = 0; i < linesToShow; i++)
		{
			if (i < lines.size()) {
				int pos = lines.size() - i;
				pos -= 1;
				activeLines.push_back(lines[pos]);
			}
		}
		for each (EConsoleLine cl in activeLines) {
			ETextElement* te = new ETextElement(cl.text, posLeft, posTop, (float)lineHeight / 48.0f, textColor);
			textElements.push_back(te);
			posTop -= lineHeight;
		}

		for each (ETextElement* te in textElements)
		{
			Game::textElements.push_back(te);
		}
		background->positionPixel = vec2(0, posTop + 10);
	}
	else {
		background->positionPixel = vec2(0, posTop + 30);

	}
}

void EConsole::Toggle()
{
	visible = !visible;
}

void EConsole::Print(string format, ...)
{
	va_list args;
	va_start(args, format);
	string lineFormat = "f%04i: ";
	string text = formatToString(lineFormat + format,Game::frameCount, args);
	va_end(args);
	EConsoleLine line = EConsoleLine();
	line.text = text;
	line.frame = Game::frameCount;
	line.time = Game::currentTime;
	lines.push_back(line);
}

void EConsole::SetUp()
{
	background = new UIElement();
	background->opacity = 0.6;
	background->foregroundColor = vec3(0.1);
	background->backgroundColor = vec3(0.1);
	background->sizePercent = vec2(100, 100);
	background->posisionPercent = vec2(0, 0);
	background->sizePixel = vec2(0);
	background->backgoundBlur = 10;
	background->zindex = 100000;

}

string EConsole::formatToString(string format, ...)
{
	va_list args;
	va_start(args, format);
	char buffer[200];
	vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
	va_end(args);
	std::string formated = buffer;
	return formated;
}

EConsoleLine::EConsoleLine()
{

}
