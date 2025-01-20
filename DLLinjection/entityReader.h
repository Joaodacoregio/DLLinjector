#ifndef ENTITYREADER_H
#define ENTITYREADER_H
 
 
 
#include "includes.h"
#include "objectRenderReader.h"
 

class ObjectRenderReader;
struct StageRange;
struct Entity;
 

class EntityReader {
public:
	EntityReader(ObjectRenderReader* ptrReaderObjectRender) {
		this->ptrReaderObjectRender = ptrReaderObjectRender;
	}

	std::wstring getEntityName(const Entity& entityBuffer);
	bool EntityNav(uintptr_t entityAddr);
	bool EspecieNav(uintptr_t especieAddr);
	void collectEntities(uintptr_t nextEntityAddr);
	void entityBrowser();
	//Pega o vetor de endereços lê e retorna um vetor de entidades 
	void ReadEntities();
	void validEntitiesAddr(StageRange range);
	void validEntities(StageRange range);
	Entity readEntityAddr(uintptr_t entityAddr);

	//Getters and setters for private members (no copy)
	void setNpcNames(std::vector<std::wstring> npcNames) { this->npcNames = npcNames; }
	const std::vector<Entity>& getEntities() const { return entities; }
	const std::vector<Entity>& getRangeEntities() const { return rangeEntities; }
	const std::vector<uintptr_t>& getRangeEntitiesAddrs() const { return rangeEntitiesAddrs; }
	const std::vector<uintptr_t>& getEntitiesAddrs() const { return entitiesAddrs; }
	const std::vector<uintptr_t>& getEspeciesAddrs() const { return especiesAddrs; }
private:
	std::vector<uintptr_t> entitiesAddrs;
	std::vector<uintptr_t> especiesAddrs;
	std::vector<Entity> entities;
	std::vector<Entity> rangeEntities;
	std::vector<uintptr_t> rangeEntitiesAddrs;
	std::vector<std::wstring> npcNames; // This names aren't valid 

	ObjectRenderReader* ptrReaderObjectRender;
};


#endif // ENTITYREADER_H