#include "EMultiDrawContainer.h"



EMultiDrawContainer::EMultiDrawContainer()
{
	/*
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

			// replace with instance count
			int parentcount = 1;

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
		glBindVertexArray(vao);

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
	*/
}


EMultiDrawContainer::~EMultiDrawContainer()
{
}

void EMultiDrawContainer::BuildMeshes()
{
}
