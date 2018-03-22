#pragma once

#include <ChakraCore.h>
#include <string>
#include <iostream>
#include <Texture.h>
#include <Material.h>
#include <Mesh.h>
#include <Asset.h>
#include <UIElement.h>
#include <RayCastHit.h>
#include <Camera.h>

namespace EJSFunction {

	// prototypes 
	extern JsValueRef JSVec3Prototype;
	extern JsValueRef JSTexturePrototype;
	extern JsValueRef JSMaterialPrototype;
	extern JsValueRef JSMeshPrototype;
	extern JsValueRef JSAssetPrototype;
	extern JsValueRef JSUIPrototype;
	extern JsValueRef JSRaycastHitPrototype;
	extern JsValueRef JSCameraPrototype;

	// Javascript to Native object conversion
	vec3 JSToNativeVec3(JsValueRef jsVec3);
	Texture* JSToNativeTexture(JsValueRef jsTexture);
	Material* JSToNativeMaterial(JsValueRef jsMaterial);
	Mesh* JSToNativeMesh(JsValueRef jsMesh);
	Asset* JSToNativeAsset(JsValueRef jsAsset);
	UIElement* JSToNativeUI(JsValueRef jsUI);
	RayCastHit* JsToNativeRaycast(JsValueRef jsRaycast);
	Camera* JSToNativeCamera(JsValueRef jsCamera);

	// Constructors
	JsValueRef CALLBACK JSConstructorVec3(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorTexture(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorMaterial(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorMesh(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorAsset(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorUI(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorRaycastResult(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSConstructorCamera(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

// member functions

	// Material
	JsValueRef CALLBACK JSMaterialSetAlbedo(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSMaterialGetAlbedo(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef CALLBACK JSMaterialSetAlbedoMap(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSMaterialGetAlbedoMap(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef CALLBACK JSMaterialSetAO(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSMaterialGetAO(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef CALLBACK JSMaterialSetMetallic(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSMaterialGetMetallic(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef CALLBACK JSMaterialSetMetallicMap(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSMaterialGetMetallicMap(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef CALLBACK JSMaterialSetRoughness(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSMaterialGetRoughness(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef CALLBACK JSMaterialSetRoughnessMap(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSMaterialGetRoughnessMap(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);


	// Mesh
	JsValueRef CALLBACK JSMeshAttachTo(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	
	// Vec3
	JsValueRef CALLBACK JSVec3GetX(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3GetY(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3GetZ(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3SetX(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3SetY(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3SetZ(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3Scale(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3Add(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSVec3Normalize(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	// UIElement
	JsValueRef CALLBACK JSUIElementSetPositionPc(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetPositionPx(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetSizePc(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetSizePx(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetForegroundColor(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetBackgroundColor(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetBackgroundBlur(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetTexture(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementSetAlphamap(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef CALLBACK JSUIElementGetPositionPc(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementGetPositionPx(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementGetSizePc(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementGetSizePx(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementGetForegroundColor(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementGetBackgroundColor(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementGetBackgroundBlur(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementGetTexture(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSUIElementGetAlphamap(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	// Asset
	JsValueRef CALLBACK JSAssetSetPosition(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetSetScale(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetSetRotation(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetSetMass(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetApplyForce(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetSetColliderOffset(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetDelete(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	JsValueRef CALLBACK JSAssetGetPosition(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetGetScale(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetGetRotation(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetGetMass(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetGetColliderOffsetPos(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSAssetGetColliderOffsetSize(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	
	// Camera 
	JsValueRef CALLBACK JSCameraGetPosition(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSCameraSetPosition(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSCameraGetForward(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	// Global
	JsValueRef CALLBACK JSLog(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState);
	JsValueRef CALLBACK JSScroll(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	JsValueRef CALLBACK JSKeyDown(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	JsValueRef CALLBACK JSRaycast(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	JsValueRef CALLBACK JSGetActiveCam(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);

	// RaycastResult
	JsValueRef CALLBACK JSRaycastGetHitPos(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	JsValueRef CALLBACK JSRaycastGetHitNormal(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
	JsValueRef CALLBACK JSRaycastGetHitAsset(JsValueRef callee, bool isConstructCall, JsValueRef * arguments, unsigned short argumentCount, void * callbackState);
}