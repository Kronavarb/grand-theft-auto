>= 3.5:

struct SkinSplit
{
  int boneLimit;
  int numMeshes;
  int numRLE;
  char *remapIndices;
  __int16 *RLECount;
  __int16 *RLE;
};

struct RpSkin
{
  int numBones;
  int numUsedBones;
  uint8 *usedBones;
  RwMatrix *inverseMatrices;
  int numWeights;
  uint8 *indices;
  float *weights;
  int field_1C;
  int field_20;
#ifdef SKY
  int field_24;
  int field_28;
#endif
  SkinSplit split;
  void *data;
};

/////////////////////////

3.4:

struct SkinSplit
{
  int boneLimit;
  int numMeshes;
  int rleSize;
  char *remapIndices;
  __int16 *RLECount;
  __int16 *RLE;
};

struct RpSkin
{
  int numBones;
  int numUsedBones;
  uint8 *usedBones;
  RwMatrix *inverseMatrices;
  int numWeights;
  uint8 *indices;
  float *weights;
  int field_1C;
  SkinSplit split;
  void *data;
};

/////////////////////////

3.3:

struct RpSkin
{
  int numBones;
  RwMatrix *inverseMatrices;
  uint8 *indices;
  float *weights;
  int maxIndex;
  int singleIndex;
  void *data;
};