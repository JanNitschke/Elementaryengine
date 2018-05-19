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

	for (int i = 0; i < TextureCount; i++)
	{
		eOpenGl->freeLayers.push_back(i);
	}
	loadTexture("Assets/Textures/empty.jpg");
	// Setup Render buffers for mdei
	glGenVertexArrays(1, &eOpenGl->vao);
	glGenBuffers(1, &eOpenGl->gElementBuffer);
	glGenBuffers(1, &eOpenGl->gIndirectBuffer);
	glGenBuffers(1, &eOpenGl->gVertexBuffer);

	Asset::rendererAssetCreatedCallback = &AssetCreatedCallback;
	Asset::rendererAssetChangedCallback = &AssetChangedCallback;
	Asset::rendererAssetDestroyedCallback = &AssetDestroyedCallback;

	GLuint shadowMaps;
	EOpenGl* eOpenGL = Game::eOpenGl;
	shadowPass = new EShadowPass(eOpenGL->vao, eOpenGL->gElementBuffer, eOpenGL->gIndirectBuffer, eOpenGL->instance, &shadowMaps);

	geometryPass = new EGeometryPass(&eOpenGL->gPosition,&eOpenGL->gNormal,&eOpenGL->gAlbedoSpec,&eOpenGL->gMaterial,&eOpenGL->gDepth);
	illuminationPass = new EIlluminationPass(eOpenGL->gPosition, eOpenGL->gNormal, eOpenGL->gAlbedoSpec, eOpenGL->gMaterial,eOpenGL->gDepth);
	postPass = new EPostPass(eOpenGL->gPosition, eOpenGL->gNormal, eOpenGL->gAlbedoSpec, eOpenGL->gMaterial, illuminationPass->frameOut, eOpenGL->gDepth);

	renderPasses.push_back(shadowPass);
	renderPasses.push_back(geometryPass);
	renderPasses.push_back(illuminationPass);
	renderPasses.push_back(postPass);

	for each (ERenderPass* pass in renderPasses)
	{
		pass->Initialize();
	}
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
	// see if new mesh is loaded or new asset is created
	if (meshChanged || assetsChanged) {
		// clear the composed Mesh if needed
		if (meshChanged) {
			eOpenGl->vVertex.clear();
			eOpenGl->vVertex.resize(0);
			eOpenGl->gIndex.clear();
			eOpenGl->gIndex.resize(0);
		}
		// clear the draw command buffer if needed
		if (assetsChanged || meshChanged) {
			eOpenGl->dICommands.clear();
			eOpenGl->dICommands.resize(0);
			eOpenGl->drawInstanceOffset.clear();
			eOpenGl->drawInstanceOffset.push_back(0);
			eOpenGl->currentIndexOffset = 0;
			eOpenGl->currentVertexOffset = 0;
			eOpenGl->instance = 0;
		}
		// redo mesh array
		for each (Mesh* m in Game::meshs) {

			// only need to add the mesh to the composed mesh if the composed mesh needs rebuilding
			if (meshChanged) {
				// Append indices
				eOpenGl->gIndex.insert(eOpenGl->gIndex.end(), m->indices.begin(), m->indices.end());

				// Append vertices
				eOpenGl->vVertex.insert(eOpenGl->vVertex.end(), m->vertices.begin(), m->vertices.end());
			}
			// create a new draw command
			DrawElementsIndirectCommand c = DrawElementsIndirectCommand();
			c.count = m->indices.size();
			int parentcount = m->parents.size();

			// tell opengl to render one time for each parent of the mesh
			c.primCount = parentcount;

			int lastoffset = eOpenGl->drawInstanceOffset.back();

			// calculate the offset of the render command due to multible renderings of the same mesh. Calculated in Shader: int drawid = gl_DrawID + offsets[gl_DrawID] + gl_InstanceID;
			// example: If 3 Spheres are rendered and then 2 cubes, the Renderer needs to know the position offset in the draw atribute array for the cube since 1nd call + 0st instance would be 1, should be 3.
			parentcount = (parentcount > 0) ? parentcount - 1 : -1;
			eOpenGl->drawInstanceOffset.push_back(lastoffset + parentcount);

			// first index of the composed mesh that belongs to the current mesh
			c.firstIndex = eOpenGl->currentIndexOffset;

			// first vertex of the composed mesh that belongs to the current mesh
			c.baseVertex = eOpenGl->currentVertexOffset;

			// index of the mesh in the composed mesh
			c.baseInstance = eOpenGl->instance;

			// increase counters
			eOpenGl->instance++;
			eOpenGl->currentIndexOffset += m->indices.size();
			eOpenGl->currentVertexOffset += m->vertices.size();

			// add draw command to the list
			eOpenGl->dICommands.push_back(c);

		}
		// copy to GPU buffers

		// bind main vertex array object (prior vao should be from the PostFx stage);
		glBindVertexArray(eOpenGl->vao);

		// resize vertex array object
		glBindBuffer(GL_ARRAY_BUFFER, eOpenGl->gVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, eOpenGl->vVertex.size() * sizeof(Vertex), &eOpenGl->vVertex[0], GL_STATIC_DRAW);


		// copy vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// copy vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		// copy vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));


		// resize and copy element buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eOpenGl->gElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, eOpenGl->gIndex.size() * sizeof(unsigned int), &eOpenGl->gIndex[0], GL_STATIC_DRAW);

		// resize and copy draw command buffer
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, eOpenGl->gIndirectBuffer);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, eOpenGl->dICommands.size() * sizeof(DrawElementsIndirectCommand), &eOpenGl->dICommands[0], GL_STATIC_DRAW);

		// resize and copy offset buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, eOpenGl->drawIdOffsetBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * eOpenGl->drawInstanceOffset.size(), eOpenGl->drawInstanceOffset.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, eOpenGl->drawIdOffsetBuffer);

	}
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

			for each(Asset* as in m->parents) {
				// create a new atribute
				DrawMeshAtributes a = DrawMeshAtributes();

				// calculate model matrix and add it to the draw atribute
				mat4 model = mat4(1.0f);
				model = translate(model, as->position + m->posOffset);
				model = glm::scale(model, as->scale + m->scaleOffset);
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
	BuildMeshes(assetChanged || assetCreated, meshChanged, eOpenGl);
	BuildDrawAtrib(eOpenGl);
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
		vec3 outpos = l->parents[0]->position;
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
