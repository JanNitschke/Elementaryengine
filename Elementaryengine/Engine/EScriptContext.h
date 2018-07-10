#pragma once

#include <ChakraCore.h>
#include <string>
#include <iostream>
#include "EJSFunctions.h"
#include <queue>

using namespace std;

class EScriptContext
{
public:
	EScriptContext();
	~EScriptContext();


	///<summary>
	/// load a Script parsed as string
	///</summary> 
	///<param name="script">
	///	the content of the script NOT THE PATH
	///</param>
	void loadScript(wstring script);


	///<summary>
	/// run a function of the script
	///</summary> 
	///<param name="name">
	///	the name of the function to call
	///</param>
	void RunFunction(const char* name);

	void ReadScript(wstring filename);

	static void projectNativeClass(const wchar_t *className, JsNativeFunction constructor, JsValueRef &prototype, vector<const wchar_t *> memberNames, vector<JsNativeFunction> memberFuncs);

	static void projectNativeClassGlobal(const wchar_t *className, vector<const wchar_t *> memberNames, vector<JsNativeFunction> memberFuncs);

	static void setCallback(JsValueRef object, const wchar_t *propertyName, JsNativeFunction callback, void *callbackState);

	static void setProperty(JsValueRef object, const wchar_t *propertyName, JsValueRef property);

	void AddBindings();

private:
	wstring script;
	queue<JsValueRef> taskQueue;

	JsRuntimeHandle runtime;
	JsContextRef context;
	JsValueRef result;
	JsValueRef global;
	unsigned currentSourceContext = 0;

	// setup prototypes
	void Vec3Bindings();
	void TextureBindings();
	void MaterialBindings();
	void MeshBindings();
	void UIElementBindings();
	void AssetBindings();
	void RayCastBindings();
	void CameraBindings();
	void LevelBindings();

	// setup global functions
	void GlobalConsoleBindings();
	void GlobalInputBindings();
	void GloablGameBindings();
};