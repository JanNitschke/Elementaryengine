

//Per asset rendering

//for each (Asset* a in assets)
//{
//	for each (AssetComponent* c in a->components)
//	{
//		if (Model* d = dynamic_cast<Model*>(c)) {
//			for each(Mesh* m in d->meshes) {
//				// Append Indices
//				gIndex.insert(gIndex.end(), m->indices.begin(), m->indices.end());

//				// Append Indices
//				vVertex.insert(vVertex.end(), m->vertices.begin(), m->vertices.end());

//				DrawElementsIndirectCommand c = DrawElementsIndirectCommand();
//				c.count = m->indices.size();
//				// TODO: change this later for grouping mesh copys
//				c.primCount = 1;
//				c.firstIndex = currentIndexOffset;
//				c.baseVertex = currentVertexOffset;
//				c.baseInstance = instance;
//				instance++;
//				currentIndexOffset += m->indices.size();
//				currentVertexOffset += m->vertices.size();
//				dICommands.push_back(c);
//			}
//		}
//		else if (Mesh* m = dynamic_cast<Mesh*>(c)) {
//			// Append Indices
//			gIndex.insert(gIndex.end(), m->indices.begin(), m->indices.end());

//			// Append Indices
//			vVertex.insert(vVertex.end(), m->vertices.begin(), m->vertices.end());

//			DrawElementsIndirectCommand c = DrawElementsIndirectCommand();
//			c.count = m->indices.size();
//			// TODO: change this later for grouping mesh copys
//			c.primCount = 1;
//			c.firstIndex = currentIndexOffset;
//			c.baseVertex = currentVertexOffset;
//			c.baseInstance = instance;
//			instance++;
//			currentIndexOffset += m->indices.size();
//			currentVertexOffset += m->vertices.size();
//			dICommands.push_back(c);
//		}
//	}
//}


/*for each (Asset* as in assets)
{
for each (AssetComponent* c in as->components)
{
if (Model* d = dynamic_cast<Model*>(c)) {
for each(Mesh* m in d->meshes) {
DrawMeshAtributes a = DrawMeshAtributes();
mat4 model = mat4(1.0f);
model = translate(model, as->position + m->posOffset);
model = glm::scale(model, as->scale + m->scaleOffset);
a.Model = model;
a.Rot = glm::toMat4(as->q);
PBRMaterial* mat = dynamic_cast<PBRMaterial*>(m->material);
a.albedo = mat->albedo;
a.ao = mat->ao;
a.roughness = mat->roughness;
a.metallic = mat->metallic;
a.metallicTex = mat->metallicMap->layer;
a.roughnessTex = mat->roughnessMap->layer;
a.albedoTex = mat->albedoMap->layer;
drawAtrib.push_back(a);
}
}
else if (Mesh* m = dynamic_cast<Mesh*>(c)) {
DrawMeshAtributes a = DrawMeshAtributes();
mat4 model = mat4(1.0f);
model = translate(model, as->position + m->posOffset);
model = glm::scale(model, as->scale + m->scaleOffset);
a.Model = model;
a.Rot = glm::toMat4(as->q);
PBRMaterial* mat = dynamic_cast<PBRMaterial*>(m->material);
a.albedo = mat->albedo;
a.ao = mat->ao;
a.roughness = mat->roughness;
a.metallic = mat->metallic;
a.metallicTex = mat->metallicMap->layer;
a.roughnessTex = mat->roughnessMap->layer;
a.albedoTex = mat->albedoMap->layer;
drawAtrib.push_back(a);
}
}
}*/
