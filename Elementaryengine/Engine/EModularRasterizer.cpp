#include "EModularRasterizer.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

#include <stb_image.h>
#include <stb_image_resize.h>


bool EModularRasterizer::assetCreated = true;
bool EModularRasterizer::assetChanged = true;

EModularRasterizer::EModularRasterizer()
{

}


EModularRasterizer::~EModularRasterizer()
{

}

void EModularRasterizer::Setup(EOpenGl * eOpenGl, EDisplaySettings * displaySettings)
{

	glGenTextures(1, &Game::eOpenGl->textureArray);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, Game::eOpenGl->textureArray);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, TextureSize, TextureSize, TextureCount);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set TexHandle
	//textureArrayHandle = glGetTextureHandleARB(eOpenGl->textureArray);
	//glMakeTextureHandleResidentARB(textureArrayHandle);

	// Set all texture layers to empty. Then load an empty texture at index 0.
	for (int i = 0; i < TextureCount; i++)
	{
		eOpenGl->freeLayers.push_back(i);
	}
	loadTexture("Assets/Textures/empty.jpg");

	// Setup Render buffers for MultiDrawIndirect
	glGenVertexArrays(1, &eOpenGl->vao);
	glGenBuffers(1, &eOpenGl->gElementBuffer);
	glGenBuffers(1, &eOpenGl->gIndirectBuffer);
	glGenBuffers(1, &eOpenGl->gVertexBuffer);

	Asset::rendererAssetCreatedCallback = &AssetCreatedCallback;
	Asset::rendererAssetChangedCallback = &AssetChangedCallback;
	Asset::rendererAssetDestroyedCallback = &AssetDestroyedCallback;

	GLuint shadowMaps, elementBuffer, indirectBuffer, positionBuffer, normalBuffer, albedoSpecBuffer, materialBuffer, depthBuffer;
	EOpenGl* eOpenGL = Game::eOpenGl;

	//shadowPass = new EShadowPass(eOpenGL->vao, eOpenGL->gElementBuffer, eOpenGL->gIndirectBuffer, eOpenGL->instance, &shadowMaps);
	//geometryPass = new EGeometryPass(&eOpenGL->gPosition,&eOpenGL->gNormal,&eOpenGL->gAlbedoSpec,&eOpenGL->gMaterial,&eOpenGL->gDepth);
	//illuminationPass = new EIlluminationPass(eOpenGL->gPosition, eOpenGL->gNormal, eOpenGL->gAlbedoSpec, eOpenGL->gMaterial,eOpenGL->gDepth);
	//postPass = new EPostPass(eOpenGL->gPosition, eOpenGL->gNormal, eOpenGL->gAlbedoSpec, eOpenGL->gMaterial, illuminationPass->frameOut, eOpenGL->gDepth);

	shadowPass = new EShadowPass(eOpenGL->vao, eOpenGL->gElementBuffer, eOpenGL->gIndirectBuffer, eOpenGL->instance, &shadowMaps);
	geometryPass = new EGeometryPass(&eOpenGL->gPosition, &eOpenGL->gNormal, &eOpenGL->gAlbedoSpec, &eOpenGL->gMaterial, &eOpenGL->gDepth);
	illuminationPass = new EIlluminationPass(eOpenGL->gPosition, eOpenGL->gNormal, eOpenGL->gAlbedoSpec, eOpenGL->gMaterial, eOpenGL->gDepth);
	postPass = new EPostPass(eOpenGL->gPosition, eOpenGL->gNormal, eOpenGL->gAlbedoSpec, eOpenGL->gMaterial, illuminationPass->frameOut, eOpenGL->gDepth);
	textPass = new ETextPass();

	renderPasses.push_back(shadowPass);
	renderPasses.push_back(geometryPass);
	renderPasses.push_back(illuminationPass);
	renderPasses.push_back(postPass);
	renderPasses.push_back(textPass);

	for each (ERenderPass* pass in renderPasses)
	{

		pass->Initialize();
	}
}
string EModularRasterizer::getShaderDefines()
{
	string out = "#version 460 core \n";
	out += "#define near 0.1 \n";
	out += "#define far 100 \n";
	out += "#define vlSampleCount ";
	out.append(std::to_string(renderSettings.basicVolumetricLightingSteps));
	out += " \n";
	out += "#define useBasicVl ";

	if (renderSettings.useBasicVolumetricLighting) {
		out += "true \n";
	}
	else {
		out += "false \n";
	}

	out += "#define useSSR ";
	if (renderSettings.useSSR) {
		out += "true \n";
	}
	else {
		out += "false \n";
	}

	out.append("#define vlMax ");
	out.append(std::to_string(renderSettings.basicVolumetricLightingMaxBrightness));
	out.append(" \n");

	return out;
}
Texture* EModularRasterizer::loadTexture(const char * path)
{
	Texture* tex = new Texture();
	if (!Game::isServer) {
		int width, height, nrChannels;
		int size = 1024;
		unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
		//unsigned char *fdata;
		//stbir_resize_uint8(data, width, height, 0, fdata, size, size, 0, nrChannels);
		if (width != size || height != size) {
			cout << "Invalid Texture size\n";
			return nullptr;
		}
		tex->layer = 10000;
		for each (int i in Game::eOpenGl->freeLayers)
		{
			if (i < tex->layer) {
				tex->layer = i;
			}
		}
		glBindTexture(GL_TEXTURE_2D_ARRAY, Game::eOpenGl->textureArray);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, tex->layer, size, size, 1, GL_RGB, GL_UNSIGNED_BYTE, data);

		Game::eOpenGl->freeLayers.erase(std::remove(Game::eOpenGl->freeLayers.begin(), Game::eOpenGl->freeLayers.end(), tex->layer), Game::eOpenGl->freeLayers.end());

		stbi_image_free(data);
	}
	return tex;
}

void EModularRasterizer::AssetCreatedCallback(Asset * asset)
{
	assetCreated = true;
}

void EModularRasterizer::AssetChangedCallback(Asset * asset)
{
	bool c = false;
	for each (AssetComponent* com in asset->components)
	{
		if (Mesh* m = static_cast<Mesh*>(com)) {
			c = true;
		}
	}
	if (c)assetChanged = true;
}

void EModularRasterizer::AssetDestroyedCallback(Asset * asset)
{
	assetCreated = true;
}


void EModularRasterizer::BuildMeshes(bool assetsChanged, bool meshChanged, EOpenGl* eOpenGl)
{
	
}

void EModularRasterizer::BuildUI(EOpenGl * eOpenGl)
{
	eOpenGl->ERUIElements.clear();
	eOpenGl->ERUIElements.resize(0);
	for each(UIElement* uie in Game::uiElements) {
		ERendererUIElement u = ERendererUIElement();
		u.positionPixel = uie->positionPixel;
		u.posisionPercent = uie->posisionPercent;
		u.sizePixel = uie->sizePixel;
		u.sizePercent = uie->sizePercent;

		u.foregroundColor = uie->foregroundColor;
		u.backgroundColor = uie->backgroundColor;
		u.texture = uie->texture->layer;
		u.alphamap = uie->alphamap->layer;
		u.backgoundBlur = uie->backgoundBlur;
		u.foregroundBlur = uie->foregroundBlur;
		u.opacity = uie->opacity;
		u.z = uie->zindex;
		eOpenGl->ERUIElements.push_back(u);
	}
	// copy the atribute vector to the GPU
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, eOpenGl->uiElementsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ERendererUIElement) * eOpenGl->ERUIElements.size(), eOpenGl->ERUIElements.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void EModularRasterizer::BuildDrawAtrib(EOpenGl * eOpenGl)
{
	if (assetChanged || assetCreated) {
		// create a vector for the draw Atributes
		int i = 0;
		vector<DrawMeshAtributes> drawAtrib;
		for each (Mesh* m in Game::meshs) {
			int lastoffset = eOpenGl->drawInstanceOffset.back();

			if (m->parent) {
				Asset* as = m->parent;
				// create a new atribute
				DrawMeshAtributes a = DrawMeshAtributes();

				// calculate model matrix and add it to the draw atribute
				mat4 model = mat4(1.0f);
				model = translate(model, as->getPosition() + m->posOffset);
				model = glm::scale(model, as->getScale() + m->scaleOffset);
				a.Model = model;

				// set rotation atribute
				a.Rot = glm::toMat4(as->q);

				// set all material parameters
				PBRMaterial* mat = dynamic_cast<PBRMaterial*>(m->material);
				a.albedo = mat->albedo;
				a.ao = mat->ao;
				a.roughness = mat->roughness;
				a.metallic = mat->metallic;
				a.metallicTex = mat->metallicMap->layer;
				a.roughnessTex = mat->roughnessMap->layer;
				a.albedoTex = mat->albedoMap->layer;

				// add the atribute to the atrubute vector
				drawAtrib.push_back(a);
				as->renderPos = i;
				i++;
			}
		}

		// copy the atribute vector to the GPU
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, eOpenGl->meshDataSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DrawMeshAtributes) * drawAtrib.size(), drawAtrib.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void EModularRasterizer::SetupFrame(bool meshChanged, EOpenGl * eOpenGl)
{
	//BuildMeshes(assetChanged || assetCreated, meshChanged, eOpenGl);
	//BuildDrawAtrib(eOpenGl);
}

void EModularRasterizer::RenderFrame(EOpenGl * eOpenGl, EDisplaySettings * displaySettings, mat4 View, mat4 Projection)
{
	RenderFrameMain(eOpenGl, displaySettings, View, Projection);
	assetCreated = false;
	assetChanged = false;
}

void EModularRasterizer::RenderFX(EOpenGl * eOpenGl, EDisplaySettings * displaySettings)
{
}

void EModularRasterizer::RenderFrameMain(EOpenGl* eOpenGl, EDisplaySettings* displaySettings, mat4 View, mat4 Projection)
{
	
	for each (ERenderPass* pass in renderPasses)
	{
		pass->Render();
	}

	// write to default framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void EModularRasterizer::SetupLamps(EOpenGl * eOpenGl, Shader * shader)
{
	// create vectors for light colors and positions
	vector<vec4> lightColors;
	vector<vec4> lightPositions;

	// add color and position for each light to vectors
	for each (Lamp* l in Game::lamps) {
		vec3 outcol = l->color;
		vec3 outpos = l->parent->position;
		lightColors.push_back(vec4(outcol, 0));
		lightPositions.push_back(vec4(outpos, 0));
	}

	// dont do this for the ssr shader
	if (shader != Mesh::ssrShader) {
		// bind Shadowmap texture array and set its uniform
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, Game::eOpenGl->shadowMaps);
		if (eOpenGl->lightingUniformShadowMaps < 0) {
			eOpenGl->lightingUniformShadowMaps = glGetUniformLocation(shader->ID, "shadowMaps");
		}
		shader->setInt(eOpenGl->lightingUniformShadowMaps, 8);
	}


	// copy color SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, eOpenGl->lightColorSSBO);
	GLsizeiptr lcs = sizeof(glm::vec4) * (lightColors.size());
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, eOpenGl->lightColorSSBO, 0, lcs);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lcs, lightColors.data(), GL_DYNAMIC_DRAW);

	// copy position SSBO
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, eOpenGl->lightPositionSSBO);
	GLsizeiptr lps = sizeof(glm::vec4) * lightPositions.size();
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 4, eOpenGl->lightPositionSSBO, 0, lps);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lps, lightPositions.data(), GL_DYNAMIC_DRAW);
}

void EModularRasterizer::RenderUI(EOpenGl * eOpenGl, EDisplaySettings * displaySettings)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);


	eOpenGl->renderQuad();

	glDisable(GL_BLEND);

}

DWORD AssetDataThread(LPVOID lpParam)
{
	return 0;
}
