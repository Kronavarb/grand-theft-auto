struct RpGeometry
{
	RwObject            object; 
	RwResEntry         *repEntry;
	RwUInt16            lockedSinceLastInst;
	RwInt16             refCount;
	RpMaterialList      matList;
	RwInt32             numTriangles;
	RwInt32             numVertices;
	RwInt32             numMorphTargets;
	RwInt32             numTexCoordSets;
	RpTriangle         *triangles;
	RwRGBA             *preLitLum;
	RwTexCoords        *texCoords[2];
	RwSurfaceProperties ignoredSurfaceProps;
	RpMeshHeader       *mesh;
	RpMorphTarget      *morphTarget;
};
