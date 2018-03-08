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
	UIElement* JSToNativeUI(JsValueRef jsAsset);

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



	JsValueRef CALLBACK LogCB(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

}