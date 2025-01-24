#ifndef OBSTACLEREADER_H
#define OBSTACLEREADER_H

#include "objectRenderReader.h"
#include "includes.h"

class ObjectRenderReader;
struct StageRange;
struct Player;
struct Obstacle;

class ObstacleReader
{
public:
	ObstacleReader(ObjectRenderReader* ptrReaderObjectRender_) {
		this->ptrReaderObjectRender = ptrReaderObjectRender_;
	}
	// Verifica se um endere�o de StageObjects j� est� na lista
	bool stageObjNav(uintptr_t stageObjAddr);
	// Fun��o para coletar StageObjects a partir de uma raiz especificada
	void collectStageObjects(uintptr_t nextStageObj);
	// Fun��o principal para buscar e retornar todos os StageObjects encontrados
	void stageObjBrowser();
	void readStageObjects();
	Obstacle readStageObjAddr(uintptr_t stageObjectsAddr);
	void validObstacleAddr(StageRange range);
	void validStageObjects(StageRange range);
	Obstacle getNearestStageObjects(Player player);
	uintptr_t getNearestStageObjectsAddr(Player player);
	std::wstring getObstacleName(const Obstacle& objBuffer);
	std::set<std::wstring> getObstacleData();
	void setValidStageStuffNames(std::vector<std::wstring> validStageStuffNames_) { validStageStuffNames = validStageStuffNames_; }

	//Getters for private members (no copy)
	const std::vector<std::wstring>& getValidStageStuffNames() const { return validStageStuffNames; }
	const std::vector<Obstacle>& getStageObjects() const { return stageObjects; }
	const std::vector<Obstacle>& getRangeStageObjects() const { return rangeStageObjects; }
	const std::vector<uintptr_t>& getStageObjectsAddrs() const { return stageObjectsAddrs; }
	const std::vector<uintptr_t>& getRangeStageObjectsAddrs() const { return validStageObjectsAddrs; }
private:
	std::vector<Obstacle> stageObjects;
	std::vector<Obstacle> rangeStageObjects;
	std::vector<uintptr_t> stageObjectsAddrs;
	std::vector<uintptr_t> validStageObjectsAddrs;
	std::vector<std::wstring> validStageStuffNames;

	ObjectRenderReader* ptrReaderObjectRender;
};


#endif // OBSTACLEREADER_H