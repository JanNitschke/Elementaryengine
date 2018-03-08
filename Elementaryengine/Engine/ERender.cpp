#include "ERender.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

void ERender::SetupRender()
{

}

void ERender::BuildMeshes(bool assetsChanged, bool meshChanged, EOpenGl* eOpenGl)
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

void ERender::BuildUI(EOpenGl * eOpenGl)
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
		u.backgoundBlurr = uie->backgoundBlurr;
		u.foregroundBlurr = uie->foregroundBlurr;
		u.opacity = uie->opacity;
		u.z = uie->zindex;
		eOpenGl->ERUIElements.push_back(u);
	}
	// copy the atribute vector to the GPU
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, eOpenGl->uiElementsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ERendererUIElement) * eOpenGl->ERUIElements.size(), eOpenGl->ERUIElements.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void ERender::BuildDrawAtrib(EOpenGl * eOpenGl)	
{
	// create a vector for the draw Atributes
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
		}
	}

	// copy the atribute vector to the GPU
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, eOpenGl->meshDataSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(DrawMeshAtributes) * drawAtrib.size(), drawAtrib.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void ERender::RenderShadowMaps(EOpenGl * eOpenGl)
{
	// use the lightmap shader
	Shader* shader = Mesh::lightmapShader;
	shader->use();
	
	// get the number of lights in a scene
	int lightcount = Game::lamps.size();

	// bind the cubemap array containing the shadowmap
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, eOpenGl->shadowMaps);

	// setup the texture array to resize to fit all the lights in the scene
	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT32, Lamp::SHADOW_WIDTH, Lamp::SHADOW_HEIGHT, 6 * lightcount, 0, GL_DEPTH_COMPONENT	, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// change the viewport to the resolution of the shadowmaps
	glViewport(0, 0, Lamp::SHADOW_WIDTH, Lamp::SHADOW_HEIGHT);

	// bind the Texture array to framebuffer and clear it
	glBindFramebuffer(GL_FRAMEBUFFER, Lamp::depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, eOpenGl->shadowMaps, 0);
	glClearDepth(1);
	glClear(GL_DEPTH_BUFFER_BIT);

	// render each lamp to a layer
	int count = 0;
	for each (Lamp* l in Game::lamps)
	{

		// only rerender the layer if a shadowmap is needed
		if (l->throwShadows) {

			// create the projection matrix
			float aspect = (float)Lamp::SHADOW_WIDTH / (float)Lamp::SHADOW_WIDTH;
			float Snear = 1.0f;
			float Sfar = 25.0f;
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, Snear, Sfar);
			vec3 lightPos = l->parents[0]->position;
			std::vector<glm::mat4> shadowTransforms;
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
			shadowTransforms.push_back(shadowProj *
				glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

			// set layer uniform wich tell the shader wich slice of the texture array to render to
			if (eOpenGl->shadowUniformLayer < 0) {
				eOpenGl->shadowUniformLayer = glGetUniformLocation(shader->ID, "layer");
			}
			shader->setInt(eOpenGl->shadowUniformLayer, count);

			// copy the created shadow matrix to the GPU
			if (eOpenGl->shadowUniformShadowMatrices < 0) {
				eOpenGl->shadowUniformShadowMatrices = glGetUniformLocation(shader->ID, "shadowMatrices");
			}
			glUniformMatrix4fv(eOpenGl->shadowUniformShadowMatrices, shadowTransforms.size(), GL_FALSE, glm::value_ptr(shadowTransforms[0]));
			count++;


			if (eOpenGl->shadowUniformFar_plane < 0) {
				eOpenGl->shadowUniformFar_plane = glGetUniformLocation(shader->ID, "far_plane");
			}
			shader->setFloat(eOpenGl->shadowUniformFar_plane, 25);

			if (eOpenGl->shadoeUniformLightPos < 0) {
				eOpenGl->shadoeUniformLightPos = glGetUniformLocation(shader->ID, "lightPos");
			}
			shader->set3Float(eOpenGl->shadoeUniformLightPos, l->parents[0]->position);

			glBindVertexArray(eOpenGl->vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eOpenGl->gElementBuffer);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, eOpenGl->gIndirectBuffer);

			glMultiDrawElementsIndirect(GL_TRIANGLES,
				GL_UNSIGNED_INT,
				(GLvoid*)0,
				eOpenGl->instance,
				0);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ERender::RenderFrame(EOpenGl* eOpenGl, EDisplaySettings* displaySettings, mat4 View, mat4 Projection)
{
// geometry pass

	// bind and setup framebuffer for geometry pass
	glBindFramebuffer(GL_FRAMEBUFFER, eOpenGl->gBuffer);
	glDisable(GL_BLEND);
	glClearColor(0.050f, 0.125f, 0.247f, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	//set viewport
	glViewport(0, 0, displaySettings->windowWidth, displaySettings->windowHeight);

	// use the geometry shader
	Mesh::geometryShader->use();

	// copy View Projection Matrix to GPU
	if (eOpenGl->geometryUniformVP < 0){
		eOpenGl->geometryUniformVP = glGetUniformLocation(Mesh::geometryShader->ID, "VP");
	}
	Mesh::geometryShader->setMat4f(eOpenGl->geometryUniformVP, Projection * View);

	// bind the composed mesh and its buffers
	glBindVertexArray(eOpenGl->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eOpenGl->gElementBuffer);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, eOpenGl->gIndirectBuffer);

	// bind the array of textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, eOpenGl->textureArray);
	if (eOpenGl->geometryUniformTextures < 0) {
		eOpenGl->geometryUniformTextures = glGetUniformLocation(Mesh::geometryShader->ID, "textures");
	}
	Mesh::geometryShader->setInt(eOpenGl->geometryUniformTextures, 0);

	// draw all elements to the geometry buffer
	glMultiDrawElementsIndirect(GL_TRIANGLES,
		GL_UNSIGNED_INT,
		(GLvoid*)0,
		eOpenGl->instance,
		0);


// lighting pass	
	// bind and setup framebuffer for lighting pass
	glBindFramebuffer(GL_FRAMEBUFFER, eOpenGl->lBuffer);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->frameOut);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, displaySettings->windowWidth, displaySettings->windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, eOpenGl->frameOut, 0);

	// use the pbrlighting shader
	Shader* shader = Mesh::pbrShader;
	shader->use();

	// clear buffers
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind Position buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gPosition);
	if (eOpenGl->lightingUniformPosition < 0) {
		eOpenGl->lightingUniformPosition = glGetUniformLocation(Mesh::pbrShader->ID, "gPosition");
	}
	shader->setInt(eOpenGl->lightingUniformPosition, 0);

	// bind Normal buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gNormal);
	if (eOpenGl->lightingUniformNormal < 0) {
		eOpenGl->lightingUniformNormal = glGetUniformLocation(Mesh::pbrShader->ID, "gNormal");
	}
	shader->setInt(eOpenGl->lightingUniformNormal, 1);

	// bind AlbedoSpec buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gAlbedoSpec);
	if (eOpenGl->lightingUniformAlbedoSpec < 0) {
		eOpenGl->lightingUniformAlbedoSpec = glGetUniformLocation(Mesh::pbrShader->ID, "gAlbedoSpec");
	}
	shader->setInt(eOpenGl->lightingUniformAlbedoSpec, 2);

	// bind MaterialBuffer texture and set its uniform
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gMaterial);
	if (eOpenGl->lightingUniformMaterial < 0) {
		eOpenGl->lightingUniformMaterial = glGetUniformLocation(Mesh::pbrShader->ID, "gMaterial");
	}
	shader->setInt(eOpenGl->lightingUniformMaterial, 3);

	// set far_plane uniform
	if (eOpenGl->lightingUniformFar_plane < 0) {
		eOpenGl->lightingUniformFar_plane = glGetUniformLocation(Mesh::pbrShader->ID, "far_plane");
	}
	shader->setFloat(eOpenGl->lightingUniformFar_plane, 25);

	// bind ColorCorrection texture and set its uniform
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Mesh::colorCorrection->id);
	if (eOpenGl->lightingUniformColorCorrection < 0) {
		eOpenGl->lightingUniformColorCorrection = glGetUniformLocation(Mesh::pbrShader->ID, "colorCorrection");
	}
	shader->setInt(eOpenGl->lightingUniformColorCorrection, 4);

	// bind depth texture and set its uniform
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gDepth);
	if (eOpenGl->lightingUniformDepth < 0) {
		eOpenGl->lightingUniformDepth = glGetUniformLocation(shader->ID, "gDepth");
	}
	shader->setInt(eOpenGl->lightingUniformDepth, 6);

	// set projection unform
	if (eOpenGl->lightingUniformProj < 0) {
		eOpenGl->lightingUniformProj = glGetUniformLocation(shader->ID, "invProj");
	}
	shader->setMat4f(eOpenGl->lightingUniformProj, inverse(Projection));

	// set view unform
	if (eOpenGl->lightingUniformView < 0) {
		eOpenGl->lightingUniformView = glGetUniformLocation(shader->ID, "invView");
	}
	shader->setMat4f(eOpenGl->lightingUniformView, inverse(View));

	// set view position uniform
	if (eOpenGl->lightingUniformViewPos < 0) {
		eOpenGl->lightingUniformViewPos = glGetUniformLocation(shader->ID, "viewPos");
	}
	shader->set3Float(eOpenGl->lightingUniformViewPos, Game::activeCam->position);




	SetupLamps(eOpenGl,shader);

	eOpenGl->renderQuad();

//post fx (ssr) 
	glBlitFramebuffer(0, 0, displaySettings->windowWidth, displaySettings->windowHeight, 0, 0, displaySettings->windowWidth, displaySettings->windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// use the ssr Shader
	shader = Mesh::ssrShader;
	shader->use();

	// clear the frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind Position buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gPosition);
	if (eOpenGl->ssrUniformPosition < 0) {
		eOpenGl->ssrUniformPosition = glGetUniformLocation(shader->ID, "gPosition");
	}
	shader->setInt(eOpenGl->ssrUniformPosition, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gNormal);
	if (eOpenGl->ssrUniformNormal < 0) {
		eOpenGl->ssrUniformNormal = glGetUniformLocation(shader->ID, "gNormal");
	}
	shader->setInt(eOpenGl->ssrUniformNormal, 1);

	// bind AlbedoSpec buffer texture and set its uniform
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gAlbedoSpec);
	if (eOpenGl->ssrUniformAlbedoSpec < 0) {
		eOpenGl->ssrUniformAlbedoSpec = glGetUniformLocation(shader->ID, "gAlbedoSpec");
	}
	shader->setInt(eOpenGl->ssrUniformAlbedoSpec, 2);

	// bind MaterialBuffer texture and set its uniform
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gMaterial);
	if (eOpenGl->ssrUniformMaterial < 0) {
		eOpenGl->ssrUniformMaterial = glGetUniformLocation(shader->ID, "gMaterial");
	}
	shader->setInt(eOpenGl->ssrUniformMaterial, 3);

	// set far_plane uniform
	if (eOpenGl->ssrUniformFar_plane < 0) {
		eOpenGl->ssrUniformFar_plane = glGetUniformLocation(shader->ID, "far_plane");
	}
	shader->setFloat(eOpenGl->ssrUniformFar_plane, 25);

	// bind Lighting pass output texture and set its uniform
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->frameOut);
	if (eOpenGl->ssrUniformColor < 0) {
		eOpenGl->ssrUniformColor = glGetUniformLocation(shader->ID, "gColor");
	}
	shader->setInt(eOpenGl->ssrUniformColor, 5);

	// bind depth texture and set its uniform
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, eOpenGl->gDepth);
	if (eOpenGl->ssrUniformDepth < 0) {
		eOpenGl->ssrUniformDepth = glGetUniformLocation(shader->ID, "gDepth");
	}
	shader->setInt(eOpenGl->ssrUniformDepth, 6);

	//SetupLamps(eOpenGl, shader);
	// set view position uniform
	if (eOpenGl->ssrUniformViewPos < 0) {
		eOpenGl->ssrUniformViewPos = glGetUniformLocation(shader->ID, "viewPos");
	}
	shader->set3Float(eOpenGl->ssrUniformViewPos, Game::activeCam->position);
	
	// set view inverse unform
	if (eOpenGl->ssrUniformInvView < 0) {
		eOpenGl->ssrUniformInvView = glGetUniformLocation(shader->ID, "invView");
	}
	shader->setMat4f(eOpenGl->ssrUniformInvView, inverse(View));

	// set projection inverse unform
	if (eOpenGl->ssrUniformInvProj < 0) {
		eOpenGl->ssrUniformInvProj = glGetUniformLocation(shader->ID, "invProj");
	}
	shader->setMat4f(eOpenGl->ssrUniformInvProj, inverse(Projection));

	// set view unform
	if (eOpenGl->ssrUniformView < 0) {
		eOpenGl->ssrUniformView = glGetUniformLocation(shader->ID, "view");
	}
	shader->setMat4f(eOpenGl->ssrUniformView, View);

	// set projection unform
	if (eOpenGl->ssrUniformProj < 0) {
		eOpenGl->ssrUniformProj = glGetUniformLocation(shader->ID, "proj");
	}


	if (eOpenGl->ssrUniformSW < 0) {
		eOpenGl->ssrUniformSW = glGetUniformLocation(shader->ID, "screenX");
	}
	shader->setInt(eOpenGl->ssrUniformSW, Game::Instance().displaySettings->windowWidth);

	if (eOpenGl->ssrUniformSH < 0) {
		eOpenGl->ssrUniformSH = glGetUniformLocation(shader->ID, "screenY");
	}
	shader->setInt(eOpenGl->ssrUniformSH, Game::Instance().displaySettings->windowHeight);


	shader->setMat4f(eOpenGl->ssrUniformProj, Projection);

	BuildUI(eOpenGl);

	// bind the array of textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, eOpenGl->textureArray);
	if (eOpenGl->ssrUniformTex < 0) {
		eOpenGl->ssrUniformTex = glGetUniformLocation(shader->ID, "textures");
	}
	shader->setInt(eOpenGl->ssrUniformTex, 0);


	eOpenGl->renderQuad();



	// copy content of geometry's depth buffer to default framebuffer's depth buffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, eOpenGl->gBuffer);

	// write to default framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	glEnable(GL_DEPTH_TEST);
}

void ERender::SetupLamps(EOpenGl * eOpenGl, Shader * shader)
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

void ERender::RenderUI(EOpenGl * eOpenGl, EDisplaySettings * displaySettings)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);


	eOpenGl->renderQuad();

	glDisable(GL_BLEND);

}
