#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H


struct RenderObject {};


struct Entity : public RenderObject {
	struct Entity* PtrPai; //0x0000
	struct Entity* PtrFilho; //0x0008
	struct Entity* PtrEspecie; //0x0010
	char pad_0018[24]; //0x0018
	float X; //0x0030
	char pad_0034[4]; //0x0034
	float Y; //0x0038
	float VectorX; //0x003C
	char pad_0040[4]; //0x0040
	float VectorY; //0x0044
	char pad_0048[64]; //0x0048
	struct EntitySkill* currentSkillPtr; //0x0088
	char pad_0090[232]; //0x0090
	struct EntityName* nameEntityPtr; //0x0178
};
struct Player : public RenderObject {                           //Seria player um objeto renderizavel?
	char Padding0[144]; // Padding inicial 
	int level;
	char PaddingX[4];
	float life;          // Offset 0x98, 4 bytes
	char Padding1[4];   // Padding para alinhamento
	// Element Offset: 160
	float mana;          // Offset 0xA0, 4 bytes
	char Padding2[184]; // Padding para alcançar o próximo elemento
	// Element Offset: 348
	float X;          // Offset 0x15C, 4 bytes
	char Padding3[4];   // Padding para alinhamento
	// Element Offset: 356
	float Y;          // Offset 0x164, 4 bytes	
	char Padding4[12];
	float VectorX; //0x0174
	char pad_0178[3]; //0x0178
	bool LyingTrigger; //0x017B
	float VectorY; //0x017C

};


//TODO: remove here
struct StageRange {
	float p1x, p1y, p2x, p2y;
};


#endif // !1