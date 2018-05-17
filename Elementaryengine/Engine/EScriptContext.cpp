#include "EScriptContext.h"
#include <string>
#include <iostream>
#include <comdef.h>

using namespace std;

EScriptContext::EScriptContext()
{
	// Create a runtime. 
	JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime);

	// Create an execution context. 
	JsCreateContext(runtime, &context);

	// Now set the current execution context.
	JsSetCurrentContext(context);

	// Convert your script result to String in JavaScript; redundant if your script returns a String
	JsValueRef resultJSString;
	JsConvertValueToString(result, &resultJSString);

	// Project script result back to C++.
	const wchar_t *resultWC;
	size_t stringLength;
	JsStringToPointer(resultJSString, &resultWC, &stringLength);

	wstring resultW(resultWC);
	cout << string(resultW.begin(), resultW.end()) << endl;

	AddBindings();

}


EScriptContext::~EScriptContext()
{
	// Dispose runtime
	JsSetCurrentContext(JS_INVALID_REFERENCE);
	JsDisposeRuntime(runtime);
}


void EScriptContext::loadScript(wstring script)
{
	this->script = script;

	// Run the script.
	JsRunScript(script.c_str(), currentSourceContext++, L"", &result);
}

void EScriptContext::RunFunction(const char * name)
{
	JsValueRef func, funcPropId, global, undefined, result;
	JsGetGlobalObject(&global);
	JsGetUndefinedValue(&undefined);
	JsValueRef args[] = { undefined };
	JsCreatePropertyId(name, strlen(name), &funcPropId);
	JsGetProperty(global, funcPropId, &func);
	// note that args[0] is thisArg of the call; actual args start at index 1
	JsCallFunction(func, args, 1, &result);
}

void EScriptContext::ReadScript(wstring filename)
{
	FILE *file;
	if (_wfopen_s(&file, filename.c_str(), L"rb"))
	{
		fwprintf(stderr, L"chakrahost: unable to open file: %s.\n", filename.c_str());
		return;
	}

	unsigned int current = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned int end = ftell(file);
	fseek(file, current, SEEK_SET);
	unsigned int lengthBytes = end - current;
	char *rawBytes = (char *)calloc(lengthBytes + 1, sizeof(char));

	if (rawBytes == nullptr)
	{
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return;
	}

	fread((void *)rawBytes, sizeof(char), lengthBytes, file);

	wchar_t *contents = (wchar_t *)calloc(lengthBytes + 1, sizeof(wchar_t));
	if (contents == nullptr)
	{
		free(rawBytes);
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return;
	}

	if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, contents, lengthBytes + 1) == 0)
	{
		free(rawBytes);
		free(contents);
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return;
	}

	wstring sc = contents;
	free(rawBytes);
	free(contents);
	script = sc;

	// Run the script.
	JsRunScript(script.c_str(), currentSourceContext++, L"", &result);

	JsValueRef vref;
	JsGetAndClearException(&vref);
	
	JsValueType jsType;
	JsGetValueType(vref, &jsType);
	return;
}

void EScriptContext::projectNativeClass(const wchar_t *className, JsNativeFunction constructor, JsValueRef &prototype, vector<const wchar_t *> memberNames, vector<JsNativeFunction> memberFuncs) {
	// project constructor to global scope 
	JsValueRef globalObject;
	JsGetGlobalObject(&globalObject);
	JsValueRef jsConstructor;
	JsCreateFunction(constructor, nullptr, &jsConstructor);
	setProperty(globalObject, className, jsConstructor);
	// create class's prototype and project its member functions
	JsCreateObject(&prototype);
	assert(memberNames.size() == memberFuncs.size());
	for (int i = 0; i < memberNames.size(); ++i) {
		setCallback(prototype, memberNames[i], memberFuncs[i], nullptr);
	}
	setProperty(jsConstructor, L"prototype", prototype);
}

void EScriptContext::projectNativeClassGlobal(const wchar_t * className, vector<const wchar_t*> memberNames, vector<JsNativeFunction> memberFuncs)
{
	// project constructor to global scope 
	JsValueRef globalObject;
	JsValueRef prototype;
	JsGetGlobalObject(&globalObject);
	// create class's prototype and project its member functions
	JsCreateObject(&prototype);
	assert(memberNames.size() == memberFuncs.size());
	for (int i = 0; i < memberNames.size(); ++i) {
		setCallback(prototype, memberNames[i], memberFuncs[i], nullptr);
	}
	JsPropertyIdRef pid;
	JsGetPropertyIdFromName(className, &pid);
	JsSetProperty(globalObject, pid, prototype, true);
}

void EScriptContext::setCallback(JsValueRef object, const wchar_t *propertyName, JsNativeFunction callback, void *callbackState)
{
	JsPropertyIdRef propertyId;
	JsGetPropertyIdFromName(propertyName, &propertyId);
	JsValueRef function;
	JsCreateFunction(callback, callbackState, &function);
	JsSetProperty(object, propertyId, function, true);
}

void EScriptContext::setProperty(JsValueRef object, const wchar_t *propertyName, JsValueRef property)
{
	JsPropertyIdRef propertyId;
	JsGetPropertyIdFromName(propertyName, &propertyId);
	JsSetProperty(object, propertyId, property, true);
}


void EScriptContext::AddBindings()
{
	// Setup the bindings for prototypes
	Vec3Bindings();
	TextureBindings();
	MaterialBindings();
	MeshBindings();
	UIElementBindings();
	AssetBindings();
	RayCastBindings();
	CameraBindings();

	// Setup bindings for global functions 
	GlobalConsoleBindings();
	GlobalInputBindings();
	GloablGameBindings();
}

void EScriptContext::Vec3Bindings()
{

	vector<const wchar_t *> memberNamesVec3;
	vector<JsNativeFunction> memberFuncsVec3;
	memberNamesVec3.push_back(L"getX");
	memberFuncsVec3.push_back(EJSFunction::JSVec3GetX);
	memberNamesVec3.push_back(L"getY");
	memberFuncsVec3.push_back(EJSFunction::JSVec3GetY);
	memberNamesVec3.push_back(L"getZ");
	memberFuncsVec3.push_back(EJSFunction::JSVec3GetZ);

	memberNamesVec3.push_back(L"setX");
	memberFuncsVec3.push_back(EJSFunction::JSVec3SetX);
	memberNamesVec3.push_back(L"setY");
	memberFuncsVec3.push_back(EJSFunction::JSVec3SetY);
	memberNamesVec3.push_back(L"setZ");
	memberFuncsVec3.push_back(EJSFunction::JSVec3SetZ);

	memberNamesVec3.push_back(L"add");
	memberFuncsVec3.push_back(EJSFunction::JSVec3Add);
	memberNamesVec3.push_back(L"scale");
	memberFuncsVec3.push_back(EJSFunction::JSVec3Scale);
	memberNamesVec3.push_back(L"normalize");
	memberFuncsVec3.push_back(EJSFunction::JSVec3Scale);

	projectNativeClass(L"Vec3", EJSFunction::JSConstructorVec3, EJSFunction::JSVec3Prototype, memberNamesVec3, memberFuncsVec3);
}

void EScriptContext::TextureBindings()
{
	vector<const wchar_t *> memberNamesTexture;
	vector<JsNativeFunction> memberFuncsTexture;
	projectNativeClass(L"Texture", EJSFunction::JSConstructorTexture, EJSFunction::JSTexturePrototype, memberNamesTexture, memberFuncsTexture);
}

void EScriptContext::MaterialBindings()
{

	vector<const wchar_t *> memberNamesMaterial;
	vector<JsNativeFunction> memberFuncsMaterial;

	memberNamesMaterial.push_back(L"setAlbedo");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialSetAlbedo);
	memberNamesMaterial.push_back(L"getAlbedo");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialGetAlbedo);

	memberNamesMaterial.push_back(L"setAlbedoMap");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialSetAlbedoMap);
	memberNamesMaterial.push_back(L"getAlbedoMap");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialGetAlbedoMap);

	memberNamesMaterial.push_back(L"setAO");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialSetAO);
	memberNamesMaterial.push_back(L"getAO");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialGetAO);

	memberNamesMaterial.push_back(L"setMetallic");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialSetMetallic);
	memberNamesMaterial.push_back(L"getMetallic");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialGetMetallic);

	memberNamesMaterial.push_back(L"setMetallicMap");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialSetMetallicMap);
	memberNamesMaterial.push_back(L"getMetallicMap");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialGetMetallicMap);

	memberNamesMaterial.push_back(L"setRoughness");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialSetRoughness);
	memberNamesMaterial.push_back(L"getRoughness");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialGetRoughness);

	memberNamesMaterial.push_back(L"setRoughnessMap");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialSetRoughnessMap);
	memberNamesMaterial.push_back(L"getRoughnessMap");
	memberFuncsMaterial.push_back(EJSFunction::JSMaterialGetRoughnessMap);

	projectNativeClass(L"Material", EJSFunction::JSConstructorMaterial, EJSFunction::JSMaterialPrototype, memberNamesMaterial, memberFuncsMaterial);

}

void EScriptContext::MeshBindings()
{

	vector<const wchar_t *> memberNamesMesh;
	vector<JsNativeFunction> memberFuncsMesh;
	memberNamesMesh.push_back(L"attachto");
	memberFuncsMesh.push_back(EJSFunction::JSMeshAttachTo);
	projectNativeClass(L"Mesh", EJSFunction::JSConstructorMesh, EJSFunction::JSMeshPrototype, memberNamesMesh, memberFuncsMesh);

}

void EScriptContext::UIElementBindings()
{
	vector<const wchar_t *> memberNamesUI;
	vector<JsNativeFunction> memberFuncsUI;

	memberNamesUI.push_back(L"setPositionPc");
	memberFuncsUI.push_back(EJSFunction::JSUIElementSetPositionPc);
	memberNamesUI.push_back(L"setPositionPx");
	memberFuncsUI.push_back(EJSFunction::JSUIElementSetPositionPx);

	memberNamesUI.push_back(L"getPositionPc");
	memberFuncsUI.push_back(EJSFunction::JSUIElementGetPositionPc);
	memberNamesUI.push_back(L"getPositionPx");
	memberFuncsUI.push_back(EJSFunction::JSUIElementGetPositionPx);

	memberNamesUI.push_back(L"setSizePc");
	memberFuncsUI.push_back(EJSFunction::JSUIElementSetSizePc);
	memberNamesUI.push_back(L"setSizePx");
	memberFuncsUI.push_back(EJSFunction::JSUIElementSetSizePx);

	memberNamesUI.push_back(L"getSizePc");
	memberFuncsUI.push_back(EJSFunction::JSUIElementGetSizePc);
	memberNamesUI.push_back(L"getSizePx");
	memberFuncsUI.push_back(EJSFunction::JSUIElementGetSizePx);

	memberNamesUI.push_back(L"setColorFG");
	memberFuncsUI.push_back(EJSFunction::JSUIElementSetForegroundColor);
	memberNamesUI.push_back(L"setColorBG");
	memberFuncsUI.push_back(EJSFunction::JSUIElementSetBackgroundColor);

	memberNamesUI.push_back(L"getColorFG");
	memberFuncsUI.push_back(EJSFunction::JSUIElementGetForegroundColor);
	memberNamesUI.push_back(L"getColorBG");
	memberFuncsUI.push_back(EJSFunction::JSUIElementGetBackgroundColor);

	memberNamesUI.push_back(L"setBackgroundBlur");
	memberFuncsUI.push_back(EJSFunction::JSUIElementSetBackgroundBlur);
	memberNamesUI.push_back(L"getBackgroundBlur");
	memberFuncsUI.push_back(EJSFunction::JSUIElementGetBackgroundBlur);

	memberNamesUI.push_back(L"setTexture");
	memberFuncsUI.push_back(EJSFunction::JSUIElementSetTexture);
	memberNamesUI.push_back(L"setAlphaMap");
	memberFuncsUI.push_back(EJSFunction::JSUIElementSetAlphamap);


	memberNamesUI.push_back(L"getTexture");
	memberFuncsUI.push_back(EJSFunction::JSUIElementGetTexture);
	memberNamesUI.push_back(L"getAlphaMap");
	memberFuncsUI.push_back(EJSFunction::JSUIElementGetAlphamap);

	projectNativeClass(L"UIElement", EJSFunction::JSConstructorUI, EJSFunction::JSUIPrototype, memberNamesUI, memberFuncsUI);
}

void EScriptContext::AssetBindings()
{
	vector<const wchar_t *> memberNamesAsset;
	vector<JsNativeFunction> memberFuncsAsset;

	memberNamesAsset.push_back(L"setPosition");
	memberFuncsAsset.push_back(EJSFunction::JSAssetSetPosition);
	memberNamesAsset.push_back(L"getPosition");
	memberFuncsAsset.push_back(EJSFunction::JSAssetGetPosition);

	memberNamesAsset.push_back(L"setScale");
	memberFuncsAsset.push_back(EJSFunction::JSAssetSetScale);
	memberNamesAsset.push_back(L"getScale");
	memberFuncsAsset.push_back(EJSFunction::JSAssetGetScale);


	memberNamesAsset.push_back(L"setRotation");
	memberFuncsAsset.push_back(EJSFunction::JSAssetSetScale);
	memberNamesAsset.push_back(L"getRotation");
	memberFuncsAsset.push_back(EJSFunction::JSAssetGetScale);

	memberNamesAsset.push_back(L"setMass");
	memberFuncsAsset.push_back(EJSFunction::JSAssetSetMass);
	memberNamesAsset.push_back(L"getMass");
	memberFuncsAsset.push_back(EJSFunction::JSAssetGetMass);

	memberNamesAsset.push_back(L"applyForce");
	memberFuncsAsset.push_back(EJSFunction::JSAssetApplyForce);

	memberNamesAsset.push_back(L"setColliderOffset");
	memberFuncsAsset.push_back(EJSFunction::JSAssetSetColliderOffset);
	memberNamesAsset.push_back(L"getColliderOffsetPos");
	memberFuncsAsset.push_back(EJSFunction::JSAssetGetColliderOffsetPos);
	memberNamesAsset.push_back(L"getColliderOffsetSize");
	memberFuncsAsset.push_back(EJSFunction::JSAssetGetColliderOffsetSize);

	memberNamesAsset.push_back(L"destroy");
	memberFuncsAsset.push_back(EJSFunction::JSAssetDelete);

	memberNamesAsset.push_back(L"equals");
	memberFuncsAsset.push_back(EJSFunction::JSAssetEqual);

	projectNativeClass(L"Asset", EJSFunction::JSConstructorAsset, EJSFunction::JSAssetPrototype, memberNamesAsset, memberFuncsAsset);
}

void EScriptContext::RayCastBindings()
{
	vector<const wchar_t *> memberNamesRC;
	vector<JsNativeFunction> memberFuncsRC;

	memberNamesRC.push_back(L"getHitPosition");
	memberFuncsRC.push_back(EJSFunction::JSRaycastGetHitPos);

	memberNamesRC.push_back(L"getHitNormal");
	memberFuncsRC.push_back(EJSFunction::JSRaycastGetHitNormal);

	memberNamesRC.push_back(L"getHitAsset");
	memberFuncsRC.push_back(EJSFunction::JSRaycastGetHitAsset);

	projectNativeClass(L"RaycastResult", EJSFunction::JSConstructorRaycastResult, EJSFunction::JSRaycastHitPrototype, memberNamesRC, memberFuncsRC);
}

void EScriptContext::CameraBindings()
{
	vector<const wchar_t *> memberNamesCamera;
	vector<JsNativeFunction> memberFuncsCamera;

	memberNamesCamera.push_back(L"getPosition");
	memberFuncsCamera.push_back(EJSFunction::JSCameraGetPosition);

	memberNamesCamera.push_back(L"setPosition");
	memberFuncsCamera.push_back(EJSFunction::JSCameraSetPosition);

	memberNamesCamera.push_back(L"getForeward");
	memberFuncsCamera.push_back(EJSFunction::JSCameraGetForward);


	projectNativeClass(L"Camera", EJSFunction::JSConstructorCamera, EJSFunction::JSCameraPrototype , memberNamesCamera, memberFuncsCamera);
}

void EScriptContext::GlobalConsoleBindings()
{
	vector<const wchar_t *> memberNames;
	vector<JsNativeFunction> memberFuncs;
	memberNames.push_back(L"log");
	memberFuncs.push_back(EJSFunction::JSLog);
	projectNativeClassGlobal(L"console", memberNames, memberFuncs);
}

void EScriptContext::GlobalInputBindings()
{
	vector<const wchar_t *> memberNames;
	vector<JsNativeFunction> memberFuncs;
	memberNames.push_back(L"getKey");
	memberFuncs.push_back(EJSFunction::JSKeyDown);
	memberNames.push_back(L"getScroll");
	memberFuncs.push_back(EJSFunction::JSScroll);
	projectNativeClassGlobal(L"input", memberNames, memberFuncs);
}

void EScriptContext::GloablGameBindings()
{
	vector<const wchar_t *> memberNames;
	vector<JsNativeFunction> memberFuncs;
	memberNames.push_back(L"RayCast");
	memberFuncs.push_back(EJSFunction::JSRaycast);
	memberNames.push_back(L"getActiveCamera");
	memberFuncs.push_back(EJSFunction::JSGetActiveCam);
	projectNativeClassGlobal(L"game", memberNames, memberFuncs);
}
