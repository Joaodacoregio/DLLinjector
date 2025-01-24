#ifndef STAGERADAR_H
#define STAGERADAR_H


#include "includes.h"
#include "objectRenderReader.h"

// Forward declarations for undefined classes
class ObjectRenderReader;
struct StageRange;
struct Entity;
struct Drop;
struct Obstacle;
struct Player;


//TODO: criar uma current range que � um dado interno
class StageRadar {
public:
    StageRadar(ObjectRenderReader* ptrObjectRenderReader_) : ptrObjectRenderReader(ptrObjectRenderReader_) {};
    void scanAllObjRenderInRange(StageRange range);
    bool isSubStageClear(StageRange range);
    bool isEntityDead(StageRange range, uintptr_t nearestEntAddrs);
    bool isDestroyedObstacle(StageRange range, uintptr_t nearestObstacleAddrs);
    uintptr_t getNearestRenderObject(StageRange range);
    std::wstring getRenderObjectName(uintptr_t renderObjAddrs);
    std::wstring detectEntityCurrentAction(Entity ent);
    void readPlayerAddrs();
    Entity readEntityAddrs(uintptr_t nearestRenderObjAddrs);
    Obstacle readObstacleAddr(uintptr_t nearestRenderObjAddrs);
    Drop readDropAddr(uintptr_t dropAddrs);
    Player getPlayer();
    std::vector<uintptr_t> getRangeEntitiesAddrs();
    float getMana();
    bool isRealyNearestObjRender(uintptr_t nearestRenderObjAddrs, StageRange range);
    bool isCollectDrop(uintptr_t nearestRenderObjAddrs, StageRange range);
    void setPermittedNames(std::vector<std::wstring> validObstacleNames, std::vector<std::wstring> validNpcNames);
    void addNpcNameInReader(const std::wstring& entName);
    uintptr_t getValidDropAddrsUsingName(const std::wstring& dropName);
    uintptr_t getValidEntityAddrsUsingName(const std::wstring& entName);

    ObjectRenderReader* getReaderManager() const { return ptrObjectRenderReader; }

private:

    std::chrono::steady_clock::time_point badRenderizedEntiyCheck;
    ObjectRenderReader* ptrObjectRenderReader;
};

#endif // !STAGERADAR_H



