#pragma once
#include <Mesh.h>
#include <EEngine.h>
#include <Camera.h>
#include <Asset.h>
#include <Lamp.h>
#include <EOpenGl.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>


///<summary>
///Elementary Renderer Functions
///</summary> 
namespace ERender {
	using namespace std;

	void SetupRender();

	///<summary>
	///builds the composed mesh needed for multiDrawIndirect and copies it to the GPU buffers
	///</summary> 
	///<param name="assetsChanged">
	///indicates if the assets changed last frame (NOT their atributes)
	///</param>
	///<param name="meshChanged">
	///indicates if the mesh changed last frame (INCLUDING their atributes)
	///</param>
	///<param name="eOpenGl">
	///the EOpenGl object that holds the buffers ids that should be worked on
	///</param>
	void BuildMeshes(bool assetsChanged, bool meshChanged, EOpenGl* eOpenGl);

	///<summary>
	///builds the list of draw atributes and copies it to the GPU buffers
	///</summary> 
	///<param name="eOpenGl">
	///the EOpenGl object that holds the buffers ids that should be worked on
	///</param>
	void BuildDrawAtrib(EOpenGl* eOpenGl);

	///<summary>
	///change the draw atributes for certain assets if only the asset atributes have changed, not the assets
	///</summary> 
	// TODO: implement ChangeAssetInfo using compute shader on ssbo
	void ChangeAssetInfo();

	///<summary>
	///renders the Shadowmaps to the cubemap array specified in eOpenGl
	///</summary> 
	///<param name="eOpenGl">
	///the EOpenGl object that holds the buffer ids that should be worked on and the cubemap array texture
	///</param>
	void RenderShadowMaps(EOpenGl* eOpenGl);

	///<summary>
	///renders the frame and PostFX in 3 passes: geometry, lighting, postFX
	///</summary> 
	///<param name="eOpenGl">
	///the EOpenGl object that holds the buffer and texture ids 
	///</param>
	///<param name="displaySettings">
	///the display settings to use
	///</param>
	///<param name="View">
	///the view matrix to use
	///</param>
	///<param name="Projection">
	///the projection matrix to use
	///</param>
	void RenderFrame(EOpenGl* eOpenGl, EDisplaySettings* displaySettings, mat4 View, mat4 Projection);

	///<summary>
	///set the uniforms and ssbos for the lamps
	///</summary> 
	///<param name="eOpenGl">
	///the EOpenGl object that holds the buffer and texture ids 
	///</param>
	///<param name="shader">
	///the shader that needs the uniforms set
	///</param>
	void SetupLamps(EOpenGl* eOpenGl, Shader * shader);

	///<summary>
	///render the UI elements
	///</summary> 
	///<param name="eOpenGl">
	///the EOpenGl object that holds the buffer and texture ids 
	///</param>
	///<param name="displaySettings">
	///the display settings to use
	///</param>
	void RenderUI(EOpenGl* eOpenGl, EDisplaySettings* displaySettings);
}