#include "EJSFunctions.h"
#include "Model.h"
#include "Game.h"

JsValueRef EJSFunction::JSVec3Prototype;
JsValueRef EJSFunction::JSTexturePrototype;
JsValueRef EJSFunction::JSMaterialPrototype;
JsValueRef EJSFunction::JSMeshPrototype;
JsValueRef EJSFunction::JSAssetPrototype;
JsValueRef EJSFunction::JSUIPrototype;


vec3 EJSFunction::JSToNativeVec3(JsValueRef jsVec3)
{
	void* p;
	JsGetExternalData(jsVec3, &p);
	return *reinterpret_cast<glm::vec3*> (p);
}

Texture * EJSFunction::JSToNativeTexture(JsValueRef jsTexture)
{
	void* p;
	JsGetExternalData(jsTexture, &p);
	return reinterpret_cast<Texture*>(p);
}

Material * EJSFunction::JSToNativeMaterial(JsValueRef jsMaterial)
{
	void* p;
	JsGetExternalData(jsMaterial, &p);
	return reinterpret_cast<PBRMaterial*>(p);
}

Mesh * EJSFunction::JSToNativeMesh(JsValueRef jsMesh)
{
	void* p;
	JsGetExternalData(jsMesh, &p);
	return reinterpret_cast<Mesh*>(p);
}

Asset * EJSFunction::JSToNativeAsset(JsValueRef jsAsset)
{
	void* p;
	JsGetExternalData(jsAsset, &p);
	return reinterpret_cast<Asset*>(p);
}

UIElement * EJSFunction::JSToNativeUI(JsValueRef jsUI)
{
	void* p;
	JsGetExternalData(jsUI, &p);
	return reinterpret_cast<UIElement*>(p);
}

// new Vec3(number x, number y, number z)
JsValueRef EJSFunction::JSConstructorVec3(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	assert(isConstructCall && argumentCount == 4);
	JsValueRef output = JS_INVALID_REFERENCE;
	double x, y, z;

	bool toArray;
	char* buffer;


	JsNumberToDouble(arguments[1], &x);
	JsNumberToDouble(arguments[2], &y);
	JsNumberToDouble(arguments[3], &z);

	vec3* v = new vec3(x, y, z);

	JsCreateExternalObject(v, nullptr, &output);
	JsSetPrototype(output, JSVec3Prototype);
	return output;
}

// new Texture(string path)
JsValueRef EJSFunction::JSConstructorTexture(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	Texture* texture;
	JsValueRef output = JS_INVALID_REFERENCE;

	assert(isConstructCall);
	JsValueType type;
	JsGetValueType(arguments[1], &type);
	if (type == JsBoolean) {
		texture = new Texture("");
	}else if(type == JsString) {
		const wchar_t path = L't';
		const wchar_t* p = &path;
		const wchar_t** pa = &p;
		size_t t;
		size_t length;

		char* buffer;

		JsStringToPointer(arguments[1], pa, &t);

		wstring ws(p);
		string str(ws.begin(), ws.end());
		texture = new Texture(str.c_str(), true);
	}

	JsCreateExternalObject(texture, nullptr, &output);
	JsSetPrototype(output, JSTexturePrototype);
	return output;
}

// new Material(vec3 color)
JsValueRef EJSFunction::JSConstructorMaterial(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	
	void* v;
	JsGetExternalData(arguments[1], &v);
	vec3* color = static_cast<vec3*>(v);

	PBRMaterial* mat = new PBRMaterial();
	mat->albedo = *color;

	JsCreateExternalObject(mat, nullptr, &output);
	JsSetPrototype(output, JSMaterialPrototype);
	return output;
}

// new Mesh(string path, Material mat)
JsValueRef EJSFunction::JSConstructorMesh(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	const wchar_t path = L't';
	const wchar_t* p = &path;
	const wchar_t** pa = &p;
	size_t t;
	size_t length;

	bool toArray;
	char* buffer;

	JsStringToPointer(arguments[1], pa, &t);

	wstring ws(p);
	string str(ws.begin(), ws.end());
	char *cpath = new char[str.length() + 1];
	strcpy(cpath, str.c_str());
	Model* mod = new Model(cpath);
	delete[] cpath;
	Mesh* mesh = mod->meshes[0];

	void* mat;
	JsGetExternalData(arguments[2], &mat);
	PBRMaterial* material = static_cast<PBRMaterial*>(mat);

	mesh->material = material;

	JsCreateExternalObject(mesh, nullptr, &output);
	JsSetPrototype(output, JSMeshPrototype);
	return output;
}

// new Asset(vec3 position, vec3 scale, int mass)
JsValueRef EJSFunction::JSConstructorAsset(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;

	void* posV;
	JsGetExternalData(arguments[1], &posV);
	vec3* pos = static_cast<vec3*>(posV);

	void* scaleV;
	JsGetExternalData(arguments[2], &scaleV);
	vec3* scale = static_cast<vec3*>(scaleV);
		
	int mass;
	JsNumberToInt(arguments[3], &mass);

	Asset* asset = new Asset(*pos, *scale, mass, assetShapes::cube);

	JsCreateExternalObject(asset, nullptr, &output);
	JsSetPrototype(output, JSAssetPrototype);
	return output;
}

JsValueRef EJSFunction::JSConstructorUI(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	UIElement* ele = new UIElement();

	if (argumentCount > 1) {
		void* posV;
		JsGetExternalData(arguments[1], &posV);
		vec3* pos = static_cast<vec3*>(posV);

		void* posPxV;
		JsGetExternalData(arguments[2], &posPxV);
		vec3* posPx = static_cast<vec3*>(posPxV);

		void* sizeV;
		JsGetExternalData(arguments[3], &sizeV);
		vec3* size = static_cast<vec3*>(sizeV);

		void* sizePxV;
		JsGetExternalData(arguments[4], &sizePxV);
		vec3* sizePx = static_cast<vec3*>(sizePxV);

		void* foregroundColorV;
		JsGetExternalData(arguments[5], &foregroundColorV);
		vec3* foregroundColor = static_cast<vec3*>(foregroundColorV);

		void* backgroundColorV;
		JsGetExternalData(arguments[6], &sizePxV);
		vec3* backgroundColor = static_cast<vec3*>(sizePxV);

		double opacity;
		JsNumberToDouble(arguments[7], &opacity);

		double bb;
		JsNumberToDouble(arguments[8], &bb);

		double zid;
		JsNumberToDouble(arguments[9], &zid);


		ele->opacity = opacity;
		ele->foregroundColor = *foregroundColor;
		ele->backgroundColor = *backgroundColor;
		ele->sizePixel = vec2(sizePx->x, sizePx->y);
		ele->sizePercent = vec2(size->x, size->y);
		ele->posisionPercent = vec2(pos->x, pos->y);
		ele->positionPixel = vec2(posPx->x, posPx->y);
		ele->backgoundBlur = bb;
		ele->texture = JSToNativeTexture(arguments[10]);
		ele->alphamap = JSToNativeTexture(arguments[11]);
		ele->zindex = zid;

	}else {
		ele->opacity = 1;
		ele->foregroundColor = vec3(1);
		ele->backgroundColor = vec3(0);
		ele->sizePixel = vec2(0);
		ele->sizePercent = vec2(0);
		ele->posisionPercent = vec2(0);
		ele->positionPixel = vec2(0);
		ele->backgoundBlur = 0;
		ele->texture = new Texture("");
		ele->alphamap = new Texture("");;
		ele->zindex = 0;
	}

	

	JsCreateExternalObject(ele, nullptr, &output);
	JsSetPrototype(output, JSUIPrototype);

	return output;
}

// material.setAlbedo(vec3 color)
JsValueRef EJSFunction::JSMaterialSetAlbedo(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* material;
	if (JsGetExternalData(arguments[0], &material) == JsNoError) {
		PBRMaterial* mat = static_cast<PBRMaterial*>(material);
		void* v;
		JsGetExternalData(arguments[1], &v);
		vec3* color = static_cast<vec3*>(v);
		mat->albedo = *color;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSMaterialGetAlbedo(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	// TODO: implement
	return JsValueRef();
}

// mesh.attachTo();
JsValueRef EJSFunction::JSMeshAttachTo(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* mesh;
	if (JsGetExternalData(arguments[0], &mesh) == JsNoError) {
		Mesh* me = static_cast<Mesh*>(mesh);
		void* v;
		JsGetExternalData(arguments[1], &v);
		Asset* asset = static_cast<Asset*>(v);
		me->attachTo(asset);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

// ----------------------------------------------------------------------------
// VEC3 MEMBER FUNCTIONS
// ----------------------------------------------------------------------------

JsValueRef EJSFunction::JSVec3GetX(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		vec3 element = JSToNativeVec3(arguments[0]);
		double val = element.x;
		JsDoubleToNumber(val, &output);
	}
	return output;
}

JsValueRef EJSFunction::JSVec3GetY(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		vec3* element = static_cast<vec3*>(vec);
		double val = element->y;
		JsDoubleToNumber(val, &output);
	}
	return output;
}

JsValueRef EJSFunction::JSVec3GetZ(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		vec3 element = JSToNativeVec3(arguments[0]);
		double val = element.z;
		JsDoubleToNumber(val, &output);
	}
	return output;
}

JsValueRef EJSFunction::JSVec3SetX(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		vec3* element = static_cast<vec3*>(uie);
		double val;
		JsNumberToDouble(arguments[1], &val);
		element->x = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSVec3SetY(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		vec3* element = static_cast<vec3*>(uie);
		double val;
		JsNumberToDouble(arguments[1], &val);
		element->y = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSVec3SetZ(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		vec3* element = static_cast<vec3*>(uie);
		double val;
		JsNumberToDouble(arguments[1], &val);
		element->z = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

// ----------------------------------------------------------------------------
// UIELEMENT MEMBER FUNCTIONS
// ----------------------------------------------------------------------------

JsValueRef EJSFunction::JSUIElementSetPositionPc(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* element = JSToNativeUI(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->posisionPercent = vec2(val.x,val.y);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSUIElementSetPositionPx(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* element = JSToNativeUI(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->positionPixel = vec2(val.x, val.y);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSUIElementSetSizePc(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* element = JSToNativeUI(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->sizePercent = vec2(val.x, val.y);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSUIElementSetSizePx(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* element = JSToNativeUI(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->sizePixel = vec2(val.x, val.y);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSUIElementSetForegroundColor(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* element = JSToNativeUI(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->foregroundColor = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSUIElementSetBackgroundColor(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* element = JSToNativeUI(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->backgroundColor = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSUIElementSetBackgroundBlur(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* element = JSToNativeUI(arguments[0]);
		double val;
		JsNumberToDouble(arguments[1],&val);
		element->backgoundBlur = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSUIElementSetTexture(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* element = JSToNativeUI(arguments[0]);
		Texture* val = JSToNativeTexture(arguments[1]);
		element->texture = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSUIElementSetAlphamap(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* element = JSToNativeUI(arguments[0]);
		Texture* val = JSToNativeTexture(arguments[1]);
		element->alphamap = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSUIElementGetPositionPc(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		UIElement* element = static_cast<UIElement*>(v);
		vec3 val = vec3(element->posisionPercent,0);
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSUIElementGetPositionPx(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		UIElement* element = static_cast<UIElement*>(v);
		vec3 val = vec3(element->positionPixel, 0);
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSUIElementGetSizePc(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		UIElement* element = static_cast<UIElement*>(v);
		vec3 val = vec3(element->sizePercent, 0);
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSUIElementGetSizePx(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		UIElement* element = static_cast<UIElement*>(v);
		vec3 val = vec3(element->sizePixel, 0);
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSUIElementGetForegroundColor(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		UIElement* element = static_cast<UIElement*>(v);
		vec3 val = element->foregroundColor;
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSUIElementGetBackgroundColor(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		UIElement* element = static_cast<UIElement*>(v);
		vec3 val = element->backgroundColor;
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSUIElementGetBackgroundBlur(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		UIElement* element = static_cast<UIElement*>(v);
		double val = element->backgoundBlur;
		JsDoubleToNumber(val, &output);
	}
	return output;
}

JsValueRef EJSFunction::JSUIElementGetTexture(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		UIElement* element = static_cast<UIElement*>(v);
		Texture* val = element->texture;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSTexturePrototype);
	}
	return output;
}

JsValueRef EJSFunction::JSUIElementGetAlphamap(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		UIElement* element = static_cast<UIElement*>(v);
		Texture* val = element->alphamap;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSTexturePrototype);
	}
	return output;
}

// ----------------------------------------------------------------------------
// ASSET MEMBER FUNCTIONS
// ----------------------------------------------------------------------------

JsValueRef EJSFunction::JSAssetSetPosition(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Asset* element = JSToNativeAsset(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->setPosition(val);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSAssetSetScale(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Asset* element = JSToNativeAsset(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->setScale(val);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSAssetSetRotation(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Asset* element = JSToNativeAsset(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->setRotation(quat(val));
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSAssetSetMass(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Asset* element = JSToNativeAsset(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->setRotation(quat(val));
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSAssetApplyForce(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Asset* element = JSToNativeAsset(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->applyForce(val);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSAssetSetColliderOffset(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Asset* element = JSToNativeAsset(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		vec3 val2 = JSToNativeVec3(arguments[2]);

		element->setCollisionPositionOffset(val);
		element->setCollisionSizeOffset(val2);

		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSAssetDelete(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	bool noError = false;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		Asset* element = static_cast<Asset*>(vec);
		element->Destroy();
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSAssetGetPosition(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		Asset* element = static_cast<Asset*>(vec);
		vec3 val = element->position;
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSAssetGetScale(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		Asset* element = static_cast<Asset*>(vec);
		vec3 val = element->scale;
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSAssetGetRotation(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		Asset* element = static_cast<Asset*>(vec);
		vec3 val = element->rotation;
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSAssetGetMass(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		Asset* element = JSToNativeAsset(arguments[0]);
		double val = element->mass;
		JsDoubleToNumber(val, &output);
	}
	return output;
}

JsValueRef EJSFunction::JSAssetGetColliderOffsetPos(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		Asset* element = static_cast<Asset*>(vec);
		vec3 val = element->collisionPosOffset;
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSAssetGetColliderOffsetSize(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		Asset* element = static_cast<Asset*>(vec);
		vec3 val = element->collisionSizeOffset;
		JsCreateExternalObject(&val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

// ----------------------------------------------------------------------------
// GLOBAL FUNCTIONS
// ----------------------------------------------------------------------------

JsValueRef EJSFunction::LogCB(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	for (unsigned int index = 1; index < argumentCount; index++)
	{
		if (index > 1)
		{
			printf(" ");
		}
		const wchar_t path = L't';
		const wchar_t* p = &path;
		const wchar_t** pa = &p;

		size_t t;
		size_t length;

		bool toArray;
		char* buffer;

		JsStringToPointer(arguments[1], pa, &t);

		wstring ws(p);
		string str(ws.begin(), ws.end());
		printf("%s", str.c_str());

	}
	printf("\n");
	return JS_INVALID_REFERENCE;
}

JsValueRef EJSFunction::Scroll(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState) {
	JsValueRef output = JS_INVALID_REFERENCE;
	vec2 v = Game::getScroll();
	vec3* s = new vec3(v, 0);
	JsCreateExternalObject(s, nullptr, &output);
	JsSetPrototype(output, JSVec3Prototype);
	return output;
}

JsValueRef EJSFunction::Key(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState) {
	JsValueRef output = JS_INVALID_REFERENCE;
	int i;
	JsNumberToInt(arguments[1], &i);
	bool v = Game::isKeyDown(i);
	JsBoolToBoolean(v, &output);
	return output;
}