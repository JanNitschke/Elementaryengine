#pragma once
#include <chakra_bridge.h>
#include <string>
#include <iostream>

using namespace std;
class EScriptRuntime
{
public:
	EScriptRuntime();
	~EScriptRuntime();

	void loadScript(wstring path);
	static jsc::context ctx;
	static jsc::runtime runtime;

private:
	void ExposeNative();
	void Test();
	wstring script;
};

