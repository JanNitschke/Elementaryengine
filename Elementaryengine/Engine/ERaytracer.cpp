#include "ERaytracer.h"



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

}


void ERaytracer::SetupFrame(bool meshChanged, EOpenGl * eOpenGl)
{
}

void ERaytracer::RenderFrame(EOpenGl * eOpenGl, EDisplaySettings * displaySettings, mat4 View, mat4 Projection)
{

	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
		
	glUseProgram(rayComputeShader->ID);
	glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute((GLuint)displaySettings->windowWidth, (GLuint)displaySettings->windowHeight, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	rayDisplayShader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, outputTexture);

	if (outputTextureUniform < 0) {
		outputTextureUniform = glGetUniformLocation(Mesh::pbrShader->ID, "rtResult");
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
