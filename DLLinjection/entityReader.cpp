 
#include "entityReader.h"
#include "includes.h"


// Definições de limites e parâmetros
#define NAME_BUFFER_SIZE 200       // Tamanho do buffer de leitura para o nome da entidade
#define NAME_OFFSET 0x28           // Offset do nome dentro do buffer da entidade
#define NAME_SKILL_OFFSET 0x10
#define MIN_MAGNITUDE_THRESHOLD 1.0f   // Limite mínimo para valores válidos de posição
#define MAX_THRESHOLD 10000.0f         // Limite superior de posição para validação
#define ENTITY_ITERATION_COUNT 10      // Número de iterações para garantir coleta completa de entidades

std::wstring EntityReader::getEntityName(const Entity& entityBuffer) {
    unsigned char namePointerBuffer[NAME_BUFFER_SIZE] = { 0 };
    SIZE_T bytesRead = 0;

    // Ler o ponteiro do nome da entidade a partir do endereço do buffer da entidade
    if (ReadProcessMemory(
        ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
        reinterpret_cast<LPCVOID>(entityBuffer.nameEntityPtr),
        &namePointerBuffer, sizeof(namePointerBuffer),
        &bytesRead) && bytesRead == sizeof(namePointerBuffer))
    {
        if (namePointerBuffer) {
            uint64_t* nameAddress = reinterpret_cast<uint64_t*>(namePointerBuffer + 0x08);
            wchar_t nameBuffer[100] = { 0 };
            SIZE_T bytesReadInner = 0;

            // Tentar ler como ponteiro
            if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
                reinterpret_cast<LPCVOID>(*nameAddress),
                nameBuffer, sizeof(nameBuffer) - sizeof(wchar_t),
                &bytesReadInner) && bytesReadInner > 0) {

                // Finalizar a string com `\0` para segurança
                nameBuffer[bytesReadInner / sizeof(wchar_t)] = L'\0';
                return std::wstring(nameBuffer);
            }

            // Caso falhe, tentar ler como texto unicode puro no deslocamento 0x08
            wchar_t* unicodeTextPtr = reinterpret_cast<wchar_t*>(namePointerBuffer + 0x08);
            std::wstring unicodeText(unicodeTextPtr, NAME_BUFFER_SIZE / sizeof(wchar_t));
            size_t nullTerminatorPos = unicodeText.find(L'\0');
            if (nullTerminatorPos != std::wstring::npos) {
                unicodeText = unicodeText.substr(0, nullTerminatorPos); // Remover dados além do terminador nulo
                return unicodeText;
            }
        }
    }

    return L"UnnamedEntity"; // Retorno vazio em caso de falha
}




 

bool EntityReader::EntityNav(uintptr_t entityAddr) {
    // Verifica se o endereço da entidade já está na lista
    return std::any_of(entitiesAddrs.begin(), entitiesAddrs.end(), [&](uintptr_t addr) { return addr == entityAddr; });
}

bool EntityReader::EspecieNav(uintptr_t especieAddr) {
    // Verifica se o endereço da espécie já está na lista
    return std::any_of(especiesAddrs.begin(), especiesAddrs.end(), [&](uintptr_t addr) { return addr == especieAddr; });
}

void EntityReader::collectEntities(uintptr_t nextEntity) {
    if (!nextEntity) return;

    Entity entityBuffer;
    SIZE_T bytesRead;

    if (!ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
        reinterpret_cast<LPCVOID>(nextEntity), &entityBuffer, sizeof(Entity), &bytesRead) ||
        bytesRead != sizeof(Entity)) {
        // std::cerr << "Erro ao ler a entidade no endereço: " << std::hex << entityAddr << std::endl;
        return;
    }

    uintptr_t especieAddr = reinterpret_cast<uintptr_t>(entityBuffer.PtrEspecie);
    if (entityBuffer.PtrEspecie && !EspecieNav(especieAddr)) {
        especiesAddrs.push_back(especieAddr);
    }

    if (!EntityNav(nextEntity)) {
        entitiesAddrs.push_back(nextEntity);

        // Recursivamente coleta entidades pai e filho, se existirem
        if (entityBuffer.PtrPai) {
            collectEntities(reinterpret_cast<uintptr_t>(entityBuffer.PtrPai));
        }
        if (entityBuffer.PtrFilho) {
            collectEntities(reinterpret_cast<uintptr_t>(entityBuffer.PtrFilho));
        }
    }
}

void EntityReader::entityBrowser() {

    //Limpa o vetor para que endereços de entidades mortas não fiquem no vetor 
    //TODO: Confirmar, Teoricamente isso vai funcionar mas não confirmei a teoria ainda
    //na pratica esta funcionando. Acho que o jogo ainda deixa alguns endereços mortos em memoria
    entitiesAddrs.clear();
    especiesAddrs.clear();

    collectEntities(ptrReaderObjectRender->getRootEntityAddress());
    // "Desemperar" a árvore para garantir coleta completa de todas as entidades
    for (size_t t = 0; t < ENTITY_ITERATION_COUNT; ++t) {
        for (const auto& especieAddr : especiesAddrs) {
            Entity especieEntity;
            SIZE_T bytesRead;

            if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
                reinterpret_cast<LPCVOID>(especieAddr), &especieEntity, sizeof(Entity), &bytesRead) &&
                bytesRead == sizeof(Entity)) {
                collectEntities(especieAddr);
            }
        }
    }
}

void  EntityReader::ReadEntities() {
    //Limpa o vetor para que entidades mortas não fiquem na memoria do vetor

    entities.clear();
    for (const auto& entityAddr : entitiesAddrs) {
        Entity entity;
        SIZE_T bytes;

        if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle()
            , reinterpret_cast<LPCVOID>(entityAddr), &entity, sizeof(Entity), &bytes) &&
            bytes == sizeof(Entity)) {
            entities.push_back(entity);
        }
    }
}
 

void EntityReader::validEntitiesAddr(StageRange range)
{
    rangeEntitiesAddrs.clear();
    entityBrowser();

    for (const auto& entityAddr : entitiesAddrs) {
        Entity entity;
        SIZE_T bytes;

        if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
            reinterpret_cast<LPCVOID>(entityAddr), &entity, sizeof(Entity), &bytes) &&
            bytes == sizeof(Entity)) {

            std::wstring entName = getEntityName(entity);

            // Verifica se o nome da entidade está na lista de NPCs.
            if (std::find(npcNames.begin(), npcNames.end(), entName) != npcNames.end()) {
                continue; // Pula para a próxima entidade, pois esta é um NPC.
            }

            if ((std::abs(entity.X) >= MIN_MAGNITUDE_THRESHOLD && std::abs(entity.X) <= MAX_THRESHOLD) &&
                (std::abs(entity.Y) >= MIN_MAGNITUDE_THRESHOLD && std::abs(entity.Y) <= MAX_THRESHOLD)) {

                std::pair<float, float> entCords = std::make_pair(entity.X, entity.Y);

                if (isPointInBounds(entCords, range.p1x, range.p1y, range.p2x, range.p2y)) {
                    rangeEntitiesAddrs.push_back(entityAddr);
                }
            }
        }
    }
}


Entity EntityReader::readEntityAddr(uintptr_t entityAddr) {
    Entity entity;
    SIZE_T bytes;

    if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle()
        , reinterpret_cast<LPCVOID>(entityAddr), &entity, sizeof(Entity), &bytes) &&
        bytes == sizeof(Entity)) {
        return entity;
    }

}

void  EntityReader::validEntities(StageRange range) {

    rangeEntities.clear();
    entityBrowser();
    ReadEntities();

    for (const auto& entity : entities) {
        // Verifica se X e Y estão dentro dos limites desejados
        if ((std::abs(entity.X) >= MIN_MAGNITUDE_THRESHOLD && std::abs(entity.X) <= MAX_THRESHOLD) &&
            (std::abs(entity.Y) >= MIN_MAGNITUDE_THRESHOLD && std::abs(entity.Y) <= MAX_THRESHOLD)) {
            std::pair<float, float> entCords = std::make_pair(entity.X, entity.Y);
            if (isPointInBounds(entCords, range.p1x, range.p1y, range.p2x, range.p2y)) {
                rangeEntities.push_back(entity);
            }
        }
    }
}


uintptr_t EntityReader::getNearestEntityAddrs(Player player)
{
    uintptr_t nearestEntityAddr = 0; // Inicializa com um valor padrão
    float minDistance = DEFAULT_MIN_DISTANCE;

    for (const auto& validEntityAddr : rangeEntitiesAddrs) {
        Entity entity;
        SIZE_T bytes;
        if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle()
            , reinterpret_cast<LPCVOID>(validEntityAddr), &entity, sizeof(Entity), &bytes) &&
            bytes == sizeof(Entity)) {

            // Calcula a distância euclidiana entre o player e a entidade
            float disX = entity.X - player.X;
            float disY = entity.Y - player.Y;
            float distance = std::sqrt(disX * disX + disY * disY);  // Distância Euclidiana

            if (distance < minDistance) {
                minDistance = distance;
                nearestEntityAddr = validEntityAddr;
            }
        }
    }

    return nearestEntityAddr; // Retorna o valor (0 se nenhum endereço foi encontrado)
}

Entity EntityReader::getNearestEntity(Player player) {
    Entity nearestEntity;
    float minDistance = DEFAULT_MIN_DISTANCE;

    for (const auto& entity : rangeEntities) {
        // Calcula a distância euclidiana entre o player e a entidade
        float disX = entity.X - player.X;
        float disY = entity.Y - player.Y;
        float distance = std::sqrt(disX * disX + disY * disY);  // Distância Euclidiana

        // Verifica se a distância atual é a menor encontrada até agora
        if (distance < minDistance) {
            minDistance = distance;
            nearestEntity = entity;  // Atualiza a entidade mais próxima
        }
    }
    return nearestEntity;
}


uintptr_t EntityReader::realNearestEntity(uintptr_t nearestEntityAddr, StageRange range, Player player) {

    uintptr_t newNearestEntityAddr = 0;

    validEntitiesAddr(range);


    newNearestEntityAddr = getNearestEntityAddrs(player);
    if (nearestEntityAddr != newNearestEntityAddr) {
        std::wcout << "Nova entidade proxima!" << std::endl;
        return newNearestEntityAddr;
    }
    return newNearestEntityAddr;

}

// Função principal
std::set<std::wstring> EntityReader::getEntityData() {
    std::set<std::wstring> setNames; // Conjunto para armazenar nomes únicos de entidades

    entityBrowser();
    auto entityAddrs = getEntitiesAddrs();
    // Itera sobre os endereços das entidades
    for (const auto& entityAddr : entityAddrs) {
        // Lê os dados da entidade
        Entity entity = readEntityAddr(entityAddr);

        // Obtém o nome da entidade
        std::wstring entityName = getEntityName(entity);

        // Insere o nome da entidade no conjunto (remove duplicatas automaticamente)
        setNames.insert(entityName);
    }
    return setNames;
}

uintptr_t EntityReader::getEntityAddrsUsingName(const std::wstring& name) {
    Entity ent;
    std::wstring entName;


    for (const auto& validAddrs : rangeEntitiesAddrs) {
        ent = readEntityAddr(validAddrs);
        entName = getEntityName(ent);

        if (entName == name) {
            return validAddrs;
        }
    }
}



std::wstring EntityReader::getCurrentSkill(const Entity& entityBuffer) {
    unsigned char namePointerBuffer[NAME_BUFFER_SIZE] = { 0 };
    SIZE_T bytesRead = 0;

    // Ler o ponteiro do nome da entidade a partir do endereço do buffer da entidade
    if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
        reinterpret_cast<LPCVOID>(entityBuffer.currentSkillPtr),
        &namePointerBuffer, sizeof(namePointerBuffer), &bytesRead) && bytesRead == sizeof(namePointerBuffer)) {

        if (namePointerBuffer) {
            uint64_t* nameAddress = reinterpret_cast<uint64_t*>(namePointerBuffer + NAME_SKILL_OFFSET);
            wchar_t nameBuffer[100] = { 0 };
            SIZE_T bytesReadInner = 0;

            if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
                reinterpret_cast<LPCVOID>(*nameAddress),
                nameBuffer, sizeof(nameBuffer) - sizeof(wchar_t), &bytesReadInner) && bytesReadInner > 0) {

                // Finalizar a string com `\0` para segurança
                nameBuffer[bytesReadInner / sizeof(wchar_t)] = L'\0';
                return std::wstring(nameBuffer);
            }
        }
    }
    return L""; // Retorno vazio em caso de falha
}