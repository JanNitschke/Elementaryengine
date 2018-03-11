#pragma once

#include <ChakraCore.h>
#include <string>
#include <iostream>
#include "EJSFunctions.h"

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

	static void setCallback(JsValueRef object, const wchar_t *propertyName, JsNativeFunction callback, void *callbackState);

	static void setProperty(JsValueRef object, const wchar_t *propertyName, JsValueRef property);

	void AddBindings();

private:
	wstring script;

	JsRuntimeHandle runtime;
	JsContextRef context;
	JsValueRef result;
	unsigned currentSourceContext = 0;

	void Vec3Bindings();
	void TextureBindings();
	void MaterialBindings();
	void MeshBindings();
	void UIElementBindings();
	void AssetBindings();
};

