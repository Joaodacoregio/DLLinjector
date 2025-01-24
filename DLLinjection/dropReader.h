#ifndef DROPREADER_H
#define DROPREADER_H

#include "includes.h" 
#include "objectRenderReader.h"


struct StageRange;
struct Drop;
struct Player;
class ObjectRenderReader;

class DropReader
{
public:
	DropReader(ObjectRenderReader* ptrReaderObjectRender_) {
		this->ptrReaderObjectRender = ptrReaderObjectRender_;
	}
	// Verifica se um endere�o de drop j� est� na lista
	bool dropNav(uintptr_t dropAddr);
	std::wstring getDropName(Drop& dropBuffer);
	// Fun��o para coletar drops a partir de uma raiz especificada
	void collectDrops(uintptr_t nextDropAddrs);
	// Fun��o principal para buscar e retornar todos os drops encontrados
	void dropBrowser();
	void readDrops();
	Drop readDropAddr(uintptr_t dropAddrs);
	void validDropsAddr(StageRange range);
	void validDrops(StageRange range);
	Drop getNearestDrop(Player player);
	uintptr_t getValidDropAddrsUsingName(const std::wstring& dropName);
	uintptr_t getNearestDropAddr(Player player);
	std::set<std::wstring> getDropData();

	//Getters for private members (no copy)
	const std::vector<Drop>& getDrops() const { return drops; }
	const std::vector<uintptr_t>& getDropsAddrs() const { return dropsAddrs; }
	const std::vector<Drop>& getRangeDrops() const { return rangeDrops; }
	const std::vector<uintptr_t>& getRangeDropsAddrs() const { return rangeDropsAddrs; }
private:
	std::vector<Drop> drops;
	std::vector<Drop> rangeDrops;
	std::vector<uintptr_t> dropsAddrs;
	std::vector<uintptr_t> rangeDropsAddrs;
	ObjectRenderReader* ptrReaderObjectRender;
};

#endif // DROPREADER_H