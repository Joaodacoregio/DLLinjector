#include "stageRadar.h"


std::vector<uintptr_t> StageRadar::getRangeEntitiesAddrs() {
    return ptrObjectRenderReader->getPtrEntityReader()->getRangeEntitiesAddrs();
}

Player StageRadar::getPlayer() {
    return  ptrObjectRenderReader->getPtrPlayerReader()->getPlayer();
}


float StageRadar::getMana() {
    return   ptrObjectRenderReader->getPtrPlayerReader()->getPlayer().mana;
}


void StageRadar::readPlayerAddrs() {
    ptrObjectRenderReader->getPtrPlayerReader()->readPlayerAddr();
}

Entity StageRadar::readEntityAddrs(uintptr_t nearestRenderObjAddrs) {
    return ptrObjectRenderReader->getPtrEntityReader()
        ->readEntityAddr(nearestRenderObjAddrs);
}

Drop StageRadar::readDropAddr(uintptr_t nearestRenderObjAddrs) {
    return ptrObjectRenderReader->getPtrDropReader()
        ->readDropAddr(nearestRenderObjAddrs);

}
Obstacle StageRadar::readObstacleAddr(uintptr_t nearestRenderObjAddrs) {
    return ptrObjectRenderReader->getPtrObstacleReader()
        ->readStageObjAddr(nearestRenderObjAddrs);
}

bool StageRadar::isEntityDead(StageRange range, uintptr_t nearestEntAddrs) {
    //atualiza as entidades
    scanAllObjRenderInRange(range);
    std::vector<uintptr_t> validEntitiesAddrs = ptrObjectRenderReader->getPtrEntityReader()->getEntitiesAddrs();

    if (validEntitiesAddrs.empty() ||
        std::find(validEntitiesAddrs.begin(), validEntitiesAddrs.end(), nearestEntAddrs) == validEntitiesAddrs.end())
    {
        std::wcout << "Entidade morta" << std::endl; //PS: Pode bugar e parar de aparecer cout
        return true;
    }
    return false;
}

bool StageRadar::isDestroyedObstacle(StageRange range, uintptr_t nearestObstacleAddrs) {
    scanAllObjRenderInRange(range);
    std::vector<uintptr_t> validObstacleAddrs = ptrObjectRenderReader->getPtrObstacleReader()->getStageObjectsAddrs();

    if (validObstacleAddrs.empty() ||
        std::find(validObstacleAddrs.begin(), validObstacleAddrs.end(), nearestObstacleAddrs) == validObstacleAddrs.end())
    {
        Obstacle obstacle = readObstacleAddr(nearestObstacleAddrs);
        std::wcout << "Obstaculo destruido!" << std::endl; //PS: bug que n�o da para ler o nome do obstaculo
        return true;
    }
    return false;
}

bool StageRadar::isCollectDrop(uintptr_t nearestDropAddrs, StageRange range) {
    scanAllObjRenderInRange(range);
    std::vector<uintptr_t> validDropAddrs = ptrObjectRenderReader->getPtrDropReader()->getRangeDropsAddrs();

    if (validDropAddrs.empty() ||
        std::find(validDropAddrs.begin(), validDropAddrs.end(), nearestDropAddrs) == validDropAddrs.end())
    {
        Drop drop = readDropAddr(nearestDropAddrs);
        std::wcout << "Drop coletado!" << std::endl;
        return true;
    }
    return false;
}


void StageRadar::scanAllObjRenderInRange(StageRange range) {
    ptrObjectRenderReader->getPtrPlayerReader()->readPlayerAddr();
    ptrObjectRenderReader->getPtrEntityReader()->validEntitiesAddr(range);
    ptrObjectRenderReader->getPtrObstacleReader()->validObstacleAddr(range);
    ptrObjectRenderReader->getPtrDropReader()->validDropsAddr(range);
}

uintptr_t StageRadar::getNearestRenderObject(StageRange range) {
    // Inicializa as vari�veis para os endere�os
    uintptr_t nearestEntAddrs = 0;
    uintptr_t stageObjAddrs = 0;
    uintptr_t nearestDropAddrs = 0;

    // Inicializa os valores m�nimos de dist�ncia com o m�ximo poss�vel
    double minDistance = DEFAULT_MIN_DISTANCE;
    uintptr_t nearestObjectAddr = 0;

    /*
        A principio se voc� der preferencia nessa ordem da tudo certo
        caso o target fique preso no mob e um obstaculo trancado o persogem sempre tem skill + 30 distancia
    */
    const float preferenceEntValue = 10.0f;
    const float preferenceDropValue = 0.0f;
    const float preferenceObstacleValue = 0.0f;

    Player player = ptrObjectRenderReader->getPtrPlayerReader()->getPlayer();
    nearestEntAddrs = ptrObjectRenderReader->getPtrEntityReader()->getNearestEntityAddrs(player);
    stageObjAddrs = ptrObjectRenderReader->getPtrObstacleReader()->getNearestStageObjectsAddr(player);
    nearestDropAddrs = ptrObjectRenderReader->getPtrDropReader()->getNearestDropAddr(player);

    // Verifica a entidade mais pr�xima
    if (nearestEntAddrs != 0) {
        Entity nearestEnt = ptrObjectRenderReader->getPtrEntityReader()->readEntityAddr(nearestEntAddrs);
        double entDist = distanceTo(player.X, player.Y, nearestEnt.X, nearestEnt.Y);
        entDist -= preferenceEntValue;
        if (entDist < minDistance) {
            minDistance = entDist;
            nearestObjectAddr = nearestEntAddrs;
        }
    }

    // Verifica o objeto de est�gio mais pr�ximo
    if (stageObjAddrs != 0) {
        Obstacle nearestObj = ptrObjectRenderReader->getPtrObstacleReader()->readStageObjAddr(stageObjAddrs);
        double stageObjDist = distanceTo(player.X, player.Y, nearestObj.X, nearestObj.Y);
        stageObjDist -= preferenceObstacleValue;

        if (stageObjDist < minDistance) {
            minDistance = stageObjDist;
            nearestObjectAddr = stageObjAddrs;
        }
    }

    // Verifica o drop mais pr�ximo
    if (nearestDropAddrs != 0) {
        Drop nearestDrop = ptrObjectRenderReader->getPtrDropReader()->readDropAddr(nearestDropAddrs);
        double dropDist = distanceTo(player.X, player.Y, nearestDrop.X, nearestDrop.Y);
        dropDist -= preferenceDropValue;

        if (dropDist < minDistance) {
            minDistance = dropDist;
            nearestObjectAddr = nearestDropAddrs;
        }
    }

    // Retorna o endere�o do objeto mais pr�ximo
    return nearestObjectAddr;
}


std::wstring StageRadar::getRenderObjectName(uintptr_t nearestObjRenderAddrs) {
    if (nearestObjRenderAddrs == 0) {
        std::wcout << L"Objeto inv�lido" << std::endl;
        return L"Objeto Inv�lido";  // Caso o endere�o seja nulo
    }

    // Primeiro tenta ler como uma Entity
    Entity nearestEnt = ptrObjectRenderReader->getPtrEntityReader()->readEntityAddr(nearestObjRenderAddrs);
    if (ptrObjectRenderReader->isAddrsReable(nearestEnt.nameEntityPtr, nearestEnt)) { // Verifica se o ponteiro para o nome � v�lido
        std::wstring entityName = ptrObjectRenderReader->getPtrEntityReader()->getEntityName(nearestEnt);
        if (!entityName.empty()) {
            return entityName;  // Retorna o nome da entidade, se dispon�vel
        }
    }


    // Caso falhe, tenta ler como um StageObject (Obstacle)
    Obstacle nearestObj = ptrObjectRenderReader->getPtrObstacleReader()->readStageObjAddr(nearestObjRenderAddrs);
    if (ptrObjectRenderReader->isAddrsReable(nearestObj.namePtr, nearestObj)) { // Verifica se o ponteiro para o nome � v�lido
        std::wstring stageObjName = ptrObjectRenderReader->getPtrObstacleReader()->getObstacleName(nearestObj);
        if (!stageObjName.empty()) {
            return stageObjName;  // Retorna o nome do objeto de est�gio, se dispon�vel
        }
    }


    // Caso falhe, tenta ler como um Drop
    Drop nearestDrop = ptrObjectRenderReader->getPtrDropReader()->readDropAddr(nearestObjRenderAddrs);
    if (ptrObjectRenderReader->isAddrsReable(nearestDrop.namePtr, nearestDrop)) { // Verifica se o ponteiro para o nome � v�lido
        std::wstring dropName = ptrObjectRenderReader->getPtrDropReader()->getDropName(nearestDrop);
        if (!dropName.empty()) {
            return dropName;  // Retorna o nome do drop, se dispon�vel
        }
    }

    // Caso n�o consiga encontrar nenhum nome
    std::wcout << L"Nome Desconhecido para o endere�o: " << nearestObjRenderAddrs << std::endl;
    return L"Nome Desconhecido";
}



bool StageRadar::isRealyNearestObjRender(
    uintptr_t nearestObjRender,
    StageRange range
)
{
    uintptr_t newNearestObjRender = 0;

    scanAllObjRenderInRange(range);


    newNearestObjRender = getNearestRenderObject(range);

    if (nearestObjRender != newNearestObjRender) {
        std::wcout << "Nova entidade proxima!" << std::endl;
        return false;
    }
    return true;

}

//Retorna true se todos os objetos renderizaveis forem mortos
bool StageRadar::isSubStageClear(StageRange range)
{
    //Scan no stage
    scanAllObjRenderInRange(range);

    return (ptrObjectRenderReader->getPtrEntityReader()->getRangeEntitiesAddrs().empty() &&
        ptrObjectRenderReader->getPtrObstacleReader()->getRangeStageObjectsAddrs().empty() &&
        ptrObjectRenderReader->getPtrDropReader()->getRangeDropsAddrs().empty());
}

//TODO:Se for necessario faz um detectObjRenderCurrent action
std::wstring StageRadar::detectEntityCurrentAction(Entity entity) {
    return ptrObjectRenderReader->getPtrEntityReader()->getCurrentSkill(entity);
}

void StageRadar::setPermittedNames(
    std::vector<std::wstring> validObstacleNames,
    std::vector<std::wstring> validNpcNames
) {
    ptrObjectRenderReader->getPtrEntityReader()->setNpcNames(validNpcNames);
    ptrObjectRenderReader->getPtrObstacleReader()->setValidStageStuffNames(validObstacleNames);
}

void StageRadar::addNpcNameInReader(const std::wstring& entName) {
    ptrObjectRenderReader->getPtrEntityReader()->addNpcName(entName);
}



uintptr_t StageRadar::getValidEntityAddrsUsingName(const std::wstring& entName) {
    uintptr_t entAddrs = ptrObjectRenderReader->getPtrEntityReader()->
        getEntityAddrsUsingName(entName);
    return entAddrs;
}

uintptr_t StageRadar::getValidDropAddrsUsingName(const std::wstring& dropName) {
    uintptr_t dropAddrs = ptrObjectRenderReader->getPtrDropReader()->
        getValidDropAddrsUsingName(dropName);
    return dropAddrs;
}