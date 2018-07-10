#include "EJSFunctions.h"
#include "Model.h"
#include "Game.h"
#include <Camera.h>
#include <ERender.h>
#include <EMeshReference.h>


JsValueRef EJSFunction::JSVec3Prototype;
JsValueRef EJSFunction::JSTexturePrototype;
JsValueRef EJSFunction::JSMaterialPrototype;
JsValueRef EJSFunction::JSMeshPrototype;
JsValueRef EJSFunction::JSAssetPrototype;
JsValueRef EJSFunction::JSUIPrototype;
JsValueRef EJSFunction::JSRaycastHitPrototype;
JsValueRef EJSFunction::JSCameraPrototype;
JsValueRef EJSFunction::JSLevelPrototype;

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

PBRMaterial * EJSFunction::JSToNativeMaterial(JsValueRef jsMaterial)
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

RayCastHit * EJSFunction::JsToNativeRaycast(JsValueRef jsRaycast)
{
	void* p;
	JsGetExternalData(jsRaycast, &p);
	return reinterpret_cast<RayCastHit*>(p);
}

Camera * EJSFunction::JSToNativeCamera(JsValueRef jsCamera)
{
	void* p;
	JsGetExternalData(jsCamera, &p);
	return reinterpret_cast<Camera*>(p);
}
ELevel* EJSFunction::JSToNativeLevel(JsValueRef jsLevel) {
	void* p;
	JsGetExternalData(jsLevel, &p);
	return reinterpret_cast<ELevel*>(p);
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
		texture = new Texture();
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
		texture = Game::Instance().loadTexture(str.c_str());
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

	void* level;
	Asset* asset;
	if (argumentCount > 4) {
		ELevel* lev = JSToNativeLevel(arguments[4]);
		asset = new Asset(*pos, *scale, mass, assetShapes::cube, lev);
	}
	else {
		asset = new Asset(*pos, *scale, mass, assetShapes::cube);
	}


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
		ele->texture = new Texture();
		ele->alphamap = new Texture();;
		ele->zindex = 0;
	}

	JsCreateExternalObject(ele, nullptr, &output);
	JsSetPrototype(output, JSUIPrototype);

	return output;
}

JsValueRef EJSFunction::JSConstructorRaycastResult(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;

	RayCastHit* rh = new RayCastHit();

	JsCreateExternalObject(rh, nullptr, &output);
	JsSetPrototype(output, JSRaycastHitPrototype);
	return output;
}

JsValueRef EJSFunction::JSConstructorCamera(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;

	Camera* cam = new Camera();

	JsCreateExternalObject(cam, nullptr, &output);
	JsSetPrototype(output, JSCameraPrototype);
	return output;
}

JsValueRef EJSFunction::JSConstructorLevel(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;

	ELevel* lev = new ELevel();

	JsCreateExternalObject(lev, nullptr, &output);
	JsSetPrototype(output, JSLevelPrototype);
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
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		PBRMaterial* element = static_cast<PBRMaterial*>(vec);
		vec3* val = new vec3();
		*val = element->albedo;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSMaterialSetAlbedoMap(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* material;
	if (JsGetExternalData(arguments[0], &material) == JsNoError) {
		PBRMaterial* mat = static_cast<PBRMaterial*>(material);
		void* v;
		Texture* tex = JSToNativeTexture(arguments[1]);
		mat->albedoMap = tex;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSMaterialGetAlbedoMap(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		PBRMaterial* element = static_cast<PBRMaterial*>(v);
		Texture* val = element->albedoMap;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSTexturePrototype);
	}
	return output;
}

JsValueRef EJSFunction::JSMaterialSetAO(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* material;
	if (JsGetExternalData(arguments[0], &material) == JsNoError) {
		PBRMaterial* mat = static_cast<PBRMaterial*>(material);
		void* v;
		JsGetExternalData(arguments[1], &v);
		vec3* color = static_cast<vec3*>(v);
		mat->ao = *color;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSMaterialGetAO(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		PBRMaterial* element = static_cast<PBRMaterial*>(vec);
		vec3* val = new vec3();
		*val = element->ao;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSMaterialSetMetallic(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* material;
	if (JsGetExternalData(arguments[0], &material) == JsNoError) {
		PBRMaterial* mat = static_cast<PBRMaterial*>(material);
		double val;
		JsNumberToDouble(arguments[1],&val);
		mat->metallic = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSMaterialGetMetallic(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		PBRMaterial* element = static_cast<PBRMaterial*>(vec);
		double val;
		val = element->metallic;
		JsDoubleToNumber(val, &output);
	}
	return output;
}

JsValueRef EJSFunction::JSMaterialSetMetallicMap(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* material;
	if (JsGetExternalData(arguments[0], &material) == JsNoError) {
		PBRMaterial* mat = static_cast<PBRMaterial*>(material);
		void* v;
		Texture* tex = JSToNativeTexture(arguments[1]);
		mat->metallicMap = tex;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSMaterialGetMetallicMap(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		PBRMaterial* element = static_cast<PBRMaterial*>(v);
		Texture* val = element->metallicMap;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSTexturePrototype);
	}
	return output;
}

JsValueRef EJSFunction::JSMaterialSetRoughness(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* material;
	if (JsGetExternalData(arguments[0], &material) == JsNoError) {
		PBRMaterial* mat = static_cast<PBRMaterial*>(material);
		double val;
		JsNumberToDouble(arguments[1], &val);
		mat->roughness = val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSMaterialGetRoughness(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		PBRMaterial* element = static_cast<PBRMaterial*>(vec);
		double val;
		val = element->roughness;
		JsDoubleToNumber(val, &output);
	}
	return output;
}

JsValueRef EJSFunction::JSMaterialSetRoughnessMap(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* material;
	if (JsGetExternalData(arguments[0], &material) == JsNoError) {
		PBRMaterial* mat = static_cast<PBRMaterial*>(material);
		void* v;
		Texture* tex = JSToNativeTexture(arguments[1]);
		mat->roughnessMap = tex;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSMaterialGetRoughnessMap(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* v;
	if (JsGetExternalData(arguments[0], &v) == JsNoError) {
		PBRMaterial* element = static_cast<PBRMaterial*>(v);
		Texture* val = element->roughnessMap;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSTexturePrototype);
	}
	return output;
}

JsValueRef EJSFunction::JSMaterialEqual(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool equal = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		PBRMaterial* a1 = JSToNativeMaterial(arguments[0]);
		PBRMaterial* a2 = JSToNativeMaterial(arguments[1]);
		equal = (a1 == a2);
	}
	JsBoolToBoolean(equal, &output);
	return output;
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
		EMeshReference* reference = new EMeshReference(me);
		reference->AttachTo(asset);
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

JsValueRef EJSFunction::JSVec3Scale(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	//JsValueRef output = JS_INVALID_REFERENCE;
	//void* vec;
	//if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
	//	vec3 element = JSToNativeVec3(arguments[0]);
	//	double val;
	//	JsNumberToDouble(arguments[1],&val);
	//	vec3* out = new vec3();
	//	*out = element * (float)val;
	//	JsCreateExternalObject(&out, nullptr, &output);
	//	JsSetPrototype(output, JSVec3Prototype);
	//}
	//return output;

	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		vec3* element = static_cast<vec3*>(uie);
		double val;
		JsNumberToDouble(arguments[1], &val);
		*element *= val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;


}

JsValueRef EJSFunction::JSVec3Add(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	/*JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		vec3 element = JSToNativeVec3(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		vec3* out = new vec3();
		*out = element + val;
		JsCreateExternalObject(&out, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;*/

	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;

	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		vec3* element = static_cast<vec3*>(uie);
		vec3 val = JSToNativeVec3(arguments[1]);
		*element += val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;

}

JsValueRef EJSFunction::JSVec3Normalize(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		vec3* element = static_cast<vec3*>(uie);
		double val = 1;
		if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
			JsNumberToDouble(arguments[1], &val);
		}
		glm::normalize(*element);
		*element *= val;
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSVec3Equal(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool equal = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		vec3 v1 = JSToNativeVec3(arguments[0]);
		vec3 v2 = JSToNativeVec3(arguments[1]);
		bvec3 t = glm::equal(v1,v2);
		equal = t.x && t.y && t.z;
	}
	JsBoolToBoolean(equal, &output);
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

JsValueRef EJSFunction::JSUIElementEqual(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool equal = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		UIElement* a1 = JSToNativeUI(arguments[0]);
		UIElement* a2 = JSToNativeUI(arguments[1]);
		equal = (a1 == a2);
	}
	JsBoolToBoolean(equal, &output);
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
		vec3* val = new vec3();
		*val = element->position;
		JsCreateExternalObject(val, nullptr, &output);
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

JsValueRef EJSFunction::JSAssetEqual(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool equal = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Asset* a1 = JSToNativeAsset(arguments[0]);
		Asset* a2 = JSToNativeAsset(arguments[1]);
		equal = (a1 == a2);
	}
	JsBoolToBoolean(equal, &output);
	return output;
}

JsValueRef EJSFunction::JSAssetSetLevel(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Asset* asset = JSToNativeAsset(arguments[0]);
		ELevel* level = JSToNativeLevel(arguments[1]);
		asset->setLevel(level);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSAssetGetLevel(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		Asset* element = static_cast<Asset*>(vec);
		ELevel* val = element->level;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSLevelPrototype);
	}
	return output;
}

JsValueRef EJSFunction::JSCameraGetPosition(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		Camera* element = static_cast<Camera*>(vec);
		vec3* val = new vec3();
		*val = element->position;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSCameraSetPosition(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Camera* element = JSToNativeCamera(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);

		element->position = val;

		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSCameraGetForward(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		Camera* element = static_cast<Camera*>(vec);
		// recalculate the front vector by calling GetView()
		element->GetView();
		vec3* val = new vec3();
		*val = element->cameraFront;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSCameraEqual(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool equal = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		Camera* a1 = JSToNativeCamera(arguments[0]);
		Camera* a2 = JSToNativeCamera(arguments[1]);
		equal = (a1 == a2);
	}
	JsBoolToBoolean(equal, &output);
	return output;
}

// ----------------------------------------------------------------------------
// GLOBAL FUNCTIONS
// ----------------------------------------------------------------------------

JsValueRef EJSFunction::JSLog(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
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
		Game::console.Print(str);
	}
	return JS_INVALID_REFERENCE;
}

JsValueRef EJSFunction::JSScroll(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState) {
	JsValueRef output = JS_INVALID_REFERENCE;
	vec2 v = Game::getScroll();
	vec3* s = new vec3(v, 0);
	JsCreateExternalObject(s, nullptr, &output);
	JsSetPrototype(output, JSVec3Prototype);
	return output;
}

JsValueRef EJSFunction::JSKeyDown(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState) {
	JsValueRef output = JS_INVALID_REFERENCE;
	int i;
	JsNumberToInt(arguments[1], &i);
	bool v = Game::isKeyDown(i);
	JsBoolToBoolean(v, &output);
	return output;
}

JsValueRef EJSFunction::JSRaycast(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	vec3 start;
	vec3 end;
	start = JSToNativeVec3(arguments[1]);
	end = JSToNativeVec3(arguments[2]);

	RayCastHit r = Game::Instance().Raycast(start, end);
	if (r.hitAsset != nullptr) {
		RayCastHit* val = new RayCastHit();
		val->hitPos = r.hitPos;
		val->hitNormal = r.hitNormal;
		val->hitAsset = r.hitAsset;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSRaycastHitPrototype);
	}
	else {
		JsBoolToBoolean(false,&output);
	}

	return output;
}

JsValueRef EJSFunction::JSGetActiveCam(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	Camera* element = Game::activeCam;
	JsCreateExternalObject(element, nullptr, &output);
	JsSetPrototype(output, JSCameraPrototype);
	return output;
}

JsValueRef EJSFunction::JSRaycastGetHitPos(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		RayCastHit* element = static_cast<RayCastHit*>(vec);
		vec3* val = new vec3();
		*val = element->hitPos;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSRaycastGetHitNormal(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		RayCastHit* element = static_cast<RayCastHit*>(vec);
		vec3* val = new vec3();
		*val = element->hitNormal;
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSRaycastGetHitAsset(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		RayCastHit* element = static_cast<RayCastHit*>(vec);
		JsCreateExternalObject(element->hitAsset, nullptr, &output);
		JsSetPrototype(output, JSAssetPrototype);
	}
	return output;
}

JsValueRef EJSFunction::JSLevelLoad(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		ELevel* element = JSToNativeLevel(arguments[0]);
		element->Load();
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSLevelUnload(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		ELevel* element = JSToNativeLevel(arguments[0]);
		element->Unload();
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSLevelSetPosition(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		ELevel* element = JSToNativeLevel(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->setPosition(val);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSLevelGetPosition(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		ELevel* element = static_cast<ELevel*>(vec);
		vec3 pos = element->getPosition();
		vec3* val = new vec3(pos);
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}

JsValueRef EJSFunction::JSLevelSetScale(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	bool noError = false;
	void* uie;
	if (JsGetExternalData(arguments[0], &uie) == JsNoError) {
		ELevel* element = JSToNativeLevel(arguments[0]);
		vec3 val = JSToNativeVec3(arguments[1]);
		element->setScale(val);
		noError = true;
	}
	JsBoolToBoolean(noError, &output);
	return output;
}

JsValueRef EJSFunction::JSLevelGetScale(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState)
{
	JsValueRef output = JS_INVALID_REFERENCE;
	void* vec;
	if (JsGetExternalData(arguments[0], &vec) == JsNoError) {
		ELevel* element = static_cast<ELevel*>(vec);
		vec3 scale = element->getScale();
		vec3* val = new vec3(scale);
		JsCreateExternalObject(val, nullptr, &output);
		JsSetPrototype(output, JSVec3Prototype);
	}
	return output;
}
