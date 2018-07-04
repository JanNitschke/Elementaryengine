#pragma once
#include <stdarg.h>
#include <string>
#include <ETextElement.h>
#include <vector>
#include <UIElement.h>

using namespace std;

struct EConsoleLine 
{
public:
	EConsoleLine();
	string text;
	double time;
	unsigned int frame;
};

class EConsole
{
public:
	EConsole();
	~EConsole();

	static string formatToString(const string format, ...);
	UIElement* background;


	vector<EConsoleLine> lines;
	vector<EConsoleLine> activeLines;
	vector<ETextElement*> textElements;
	EConsoleLine header = EConsoleLine();
	glm::vec3 textColor = vec3(0.9f);
	
	const int maxLineLength = 500;
	int linesToShow = 5;
	int posLeft = 10;
	int posTop;
	int lineHeight = 24;
	bool visible = false;
	
	void Show();
	void Hide();
	void Update();
	void Toggle();
	void Print(string format, ...);
	void SetUp();

};

