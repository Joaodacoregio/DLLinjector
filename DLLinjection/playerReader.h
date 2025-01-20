#ifndef PLAYERREADER_H
#define PLAYERREADER_H
 

#include "includes.h"
#include "objectRenderReader.h"

class ObjectRenderReader;
struct Player;

class PlayerReader {
public:
	PlayerReader(ObjectRenderReader* ptrReaderObjectRender_) {
		this->ptrReaderObjectRender = ptrReaderObjectRender_;
	}
	void readPlayerAddr();
	void infoDataPlayer();

	//Getter for private members (no copy)
	const Player& getPlayer() const { return player; }
private:
	Player player;
	ObjectRenderReader* ptrReaderObjectRender;
};


#endif  