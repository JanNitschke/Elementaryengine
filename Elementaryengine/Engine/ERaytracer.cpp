#include "ERaytracer.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

ERaytracer::ERaytracer()
{
}


ERaytracer::~ERaytracer()
{
}
void ERaytracer::Setup(EOpenGl * eOpenGl, EDisplaySettings * displaySettings)
{
	// create an output texture
	glGenTextures(1, &outputTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, outputTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, displaySettings->windowWidth, displaySettings->windowHeight, 0, GL_RGBA, GL_FLOAT,
		NULL);
	glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n",
		work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);

	int work_grp_inv;

	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);
	
	rayComputeShader = new Shader("..\\shaders\\RTCompute.comp");
	rayDisplayShader = new Shader("..\\shaders\\geometry.vert", "..\\shaders\\RTPost.frag");

	Asset::rendererAssetCreatedCallback = ERenderer::AssetCreatedCallback;
	Asset::rendererAssetChangedCallback = ERenderer::AssetChangedCallback;
	Asset::rendererAssetDestroyedCallback = ERenderer::AssetDestroyedCallback;

	glGenBuffers(1, &triangleBuffer);

}


void ERaytracer::SetupFrame(bool meshChanged, EOpenGl * eOpenGl)
{
}

void ERaytracer::RenderFrame(EOpenGl * eOpenGl, EDisplaySettings * displaySettings, mat4 View, mat4 Projection)
{
	rayComputeShader->use();
		

	rayComputeShader->setMat4f("invProj", inverse(Projection));
	rayComputeShader->setMat4f("invView", inverse(View));
	/*
	if (eyeUniform < 0) {
		eyeUniform = glGetUniformLocation(rayComputeShader->ID, "viewPos");
	}
	rayComputeShader->set3Float(eyeUniform, Game::activeCam->position);
	*/
	rayComputeShader->set3Float("viewPos", Game::activeCam->position);

	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
		
	vector<RaytracerTriangle> tirangles;
	
	int assetNum = 0;
	for each (Asset* a in Game::assets)
	{
		for each (AssetComponent* c in a->components)
		{
			if (Mesh* m = static_cast<Mesh*>(c)) {
				mat4 model = mat4(1.0f);
				model = translate(model, a->position + m->posOffset);
				model = glm::scale(model, a->scale + m->scaleOffset);

				mat4 Rot = glm::toMat4(a->q);

				for (int i = 0; i < m->indices.size() / 3; i++) {
					RaytracerTriangle tri = RaytracerTriangle();
					vec4 p0 = vec4(m->vertices[m->indices[i * 3    ]].Position, 1);
					vec4 p1 = vec4(m->vertices[m->indices[i * 3 + 1]].Position, 1);
					vec4 p2 = vec4(m->vertices[m->indices[i * 3 + 2]].Position, 1);
					/*		
					tri.p0T = vec3(model * vec4(m->vertices[m->indices[i * 3    ]].TexCoords, 1, 1));
					tri.p1T = vec3(model * vec4(m->vertices[m->indices[i * 3 + 1]].TexCoords, 1, 1));
					tri.p2T = vec3(model * vec4(m->vertices[m->indices[i * 3 + 2]].TexCoords, 1, 1));
					*/
					tri.p0 = model * p0;
					tri.p1 = model * p1;
					tri.p2 = model * p2;

					vec3 v1 = (tri.p0 - tri.p2);
					vec3 v2 = (tri.p0 - tri.p1);
					tri.normal = vec4(normalize(cross(v1,v2)),1);
					tri.drawData = assetNum;
					tirangles.push_back(tri);
				}
				assetNum++;
			}
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleBuffer);
	GLsizeiptr lcs = tirangles.size() * sizeof(RaytracerTriangle);
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, triangleBuffer, 0, lcs);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lcs, tirangles.data(), GL_DYNAMIC_DRAW);

	glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	int w = (GLuint)displaySettings->windowWidth / 8;
	int h = (GLuint)displaySettings->windowHeight / 8;
	w = nextPowerOfTwo(w);
	h = nextPowerOfTwo(h);
	glDispatchCompute(w, h, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	rayDisplayShader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, outputTexture);


	if (outputTextureUniform < 0) {
		outputTextureUniform = glGetUniformLocation(rayDisplayShader->ID, "rtResult");
	}
	rayDisplayShader->setInt(outputTextureUniform, 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	eOpenGl->renderQuad();

}

void ERaytracer::RenderFX(EOpenGl * eOpenGl, EDisplaySettings * displaySettings)
{
}


Texture * ERaytracer::loadTexture(const char * path)
{
	return nullptr;
}

