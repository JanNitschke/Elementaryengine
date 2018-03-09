#pragma once

#include <ChakraCore.h>
#include <string>
#include <iostream>
#include <Texture.h>
#include <Material.h>
#include <Mesh.h>
#include <Asset.h>
#include <UIElement.h>

namespace EJSFunction {

	// prototypes 
	extern JsValueRef JSVec3Prototype;
	extern JsValueRef JSTexturePrototype;
	extern JsValueRef JSMaterialPrototype;
	extern JsValueRef JSMeshPrototype;
	extern JsValueRef JSAssetPrototype;
	extern JsValueRef JSUIPrototype;

	// Javascript to Native object conversion
	vec3 JSToNativeVec3(JsValueRef jsVec3);
	Texture* JSToNativeTexture(JsValueRef jsTexture);
	Material* JSToNativeMaterial(JsValueRef jsMaterial);
	Mesh* JSToNativeMesh(JsValueRef jsMesh);
	Asset* JSToNativeAsset(JsValueRef jsAsset);
	UIElement* JSToNativeUI(JsValueRef jsUI);

	// Constructors
	JsValueRef CALLBACK JSConstructorVec3(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorTexture(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorMaterial(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorMesh(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorAsset(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorUI(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	// member functions
	JsValueRef CALLBACK JSMaterialSetAlbedo(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSMeshAttachTo(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	
	// Vec3
	JsValueRef CALLBACK JSVec3GetX(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3GetY(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3GetZ(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3SetX(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3SetY(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3SetZ(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	// UIElement
	JsValueRef CALLBACK JSUIElementSetPositionPc(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetPositionPx(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetSizePc(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetSizePx(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetForegroundColor(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetBackgroundColor(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef CALLBACK LogCB(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef Scroll(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	JsValueRef Key(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);


}