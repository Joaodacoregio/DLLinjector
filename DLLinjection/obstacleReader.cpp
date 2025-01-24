#include "includes.h"
#include "obstacleReader.h"

#define MIN_MAGNITUDE_THRESHOLD 1.0f   // Limite m�nimo para valores v�lidos de posi��o
#define MAX_THRESHOLD 10000.0f         // Limite superior de posi��o para valida��o

bool ObstacleReader::stageObjNav(uintptr_t stageAddr) {
	// Verifica se o endere�o do StageObject j� est� na lista
	return std::any_of(stageObjectsAddrs.begin(), stageObjectsAddrs.end(), [&](uintptr_t addr) { return addr == stageAddr; });
}

// Fun��o para coletar StageObjects a partir de uma raiz especificada
void ObstacleReader::collectStageObjects(uintptr_t nextStageObjectAddr)
{
	if (!nextStageObjectAddr) return; // Caso o endere�o da raiz seja inv�lido, sai da fun��o

	Obstacle stageObjectBuffer;
	SIZE_T bytesRead;

	// L� o StageObject da mem�ria do processo
	if (!ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
		reinterpret_cast<LPCVOID>(nextStageObjectAddr), &stageObjectBuffer, sizeof(stageObjectBuffer), &bytesRead) ||
		bytesRead != sizeof(Obstacle)) {
		return;
	}


	// Verifica se o StageObject j� est� na lista
	if (!stageObjNav(nextStageObjectAddr)) {
		stageObjectsAddrs.push_back(nextStageObjectAddr);

		// Recursivamente coleta entidades ptrOne, ptrTwo e ptrThre, se existirem
		if (stageObjectBuffer.ptrOne) {
			collectStageObjects(reinterpret_cast<uintptr_t>(stageObjectBuffer.ptrOne));
		}
		if (stageObjectBuffer.ptrTwo) {
			collectStageObjects(reinterpret_cast<uintptr_t>(stageObjectBuffer.ptrTwo));
		}
		if (stageObjectBuffer.ptrThre) {
			collectStageObjects(reinterpret_cast<uintptr_t>(stageObjectBuffer.ptrThre));
		}
	}
}

// Fun��o principal para buscar e retornar todos os StageObjects encontrados
void ObstacleReader::stageObjBrowser() {
	stageObjectsAddrs.clear();

	// Chama a fun��o `collectStageObjects` para come�ar a coleta a partir do endere�o fornecido
	collectStageObjects(ptrReaderObjectRender->getRootObstacleAddress());
}



void ObstacleReader::readStageObjects() {
	stageObjects.clear();

	for (const auto& stageObjectAddr : stageObjectsAddrs) {
		Obstacle stageObject;
		SIZE_T bytes;

		if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
			reinterpret_cast<LPCVOID>(stageObjectAddr), &stageObject, sizeof(stageObject), &bytes) &&
			bytes == sizeof(stageObject)) {
			stageObjects.push_back(stageObject);
		}
	}
}

Obstacle ObstacleReader::readStageObjAddr(uintptr_t stageObjectAddr) {
	Obstacle stageObject;
	SIZE_T bytes;

	if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle()
		, reinterpret_cast<LPCVOID>(stageObjectAddr), &stageObject, sizeof(stageObject), &bytes) &&
		bytes == sizeof(stageObject)) {
		return stageObject;
	}
}

void ObstacleReader::validObstacleAddr(StageRange range)
{
	validStageObjectsAddrs.clear();
	stageObjBrowser();



	for (const auto& objAddr : stageObjectsAddrs) {
		Obstacle obj;
		SIZE_T bytes;

		if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
			reinterpret_cast<LPCVOID>(objAddr), &obj, sizeof(Obstacle), &bytes) &&
			bytes == sizeof(obj)) {
			std::wstring nameObj = getObstacleName(obj);
			std::pair<float, float> dropCords = std::make_pair(obj.X, obj.Y);
			if ((std::abs(obj.X) >= MIN_MAGNITUDE_THRESHOLD && std::abs(obj.X) <= MAX_THRESHOLD) &&
				(std::abs(obj.Y) >= MIN_MAGNITUDE_THRESHOLD && std::abs(obj.Y) <= MAX_THRESHOLD)) {
				if (isPointInBounds(dropCords, range.p1x, range.p1y, range.p2x, range.p2y) &&
					std::find(validStageStuffNames.begin(), validStageStuffNames.end(), nameObj) != validStageStuffNames.end()) {
					validStageObjectsAddrs.push_back(objAddr);
				}
			}

		}
	}

}


void ObstacleReader::validStageObjects(StageRange range) {
	stageObjBrowser();
	readStageObjects();

	for (const auto& stageObject : stageObjects) {
		// Verifica se X e Y est�o dentro dos limites desejados
		if ((std::abs(stageObject.X) >= MIN_MAGNITUDE_THRESHOLD && std::abs(stageObject.X) <= MAX_THRESHOLD) &&
			(std::abs(stageObject.Y) >= MIN_MAGNITUDE_THRESHOLD && std::abs(stageObject.Y) <= MAX_THRESHOLD)) {
			std::wstring nameObj = getObstacleName(stageObject);
			std::pair<float, float> dropCords = std::make_pair(stageObject.X, stageObject.Y);
			if (isPointInBounds(dropCords, range.p1x, range.p1y, range.p2x, range.p2y) &&
				std::find(validStageStuffNames.begin(), validStageStuffNames.end(), nameObj) != validStageStuffNames.end()) {
				rangeStageObjects.push_back(stageObject);
			}
		}
	}
}

Obstacle ObstacleReader::getNearestStageObjects(Player player) {
	Obstacle nearestStageObject{};
	float minDistance = DEFAULT_MIN_DISTANCE;

	for (const auto& stageObject : stageObjects) {
		// Calcula a dist�ncia euclidiana entre o jogador e o objeto
		float disX = stageObject.X - player.X;
		float disY = stageObject.Y - player.Y;
		float distance = std::sqrt(disX * disX + disY * disY);  // Dist�ncia Euclidiana

		// Verifica se a dist�ncia atual � a menor encontrada at� agora
		if (distance < minDistance) {
			minDistance = distance;
			nearestStageObject = stageObject;  // Atualiza o objeto mais pr�ximo
		}
	}

	return nearestStageObject;
}

uintptr_t ObstacleReader::getNearestStageObjectsAddr(Player player)
{
	uintptr_t nearestStageObjectAddr = 0; // Inicializa com um valor padr�o
	float minDistance = DEFAULT_MIN_DISTANCE;

	for (const auto& validStageObjectAddr : validStageObjectsAddrs) {
		Obstacle stageObject;
		SIZE_T bytes;

		if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
			reinterpret_cast<LPCVOID>(validStageObjectAddr), &stageObject, sizeof(stageObject), &bytes) &&
			bytes == sizeof(stageObject)) {

			// Calcula a dist�ncia euclidiana entre o jogador e o objeto
			float disX = stageObject.X - player.X;
			float disY = stageObject.Y - player.Y;
			float distance = std::sqrt(disX * disX + disY * disY);  // Dist�ncia Euclidiana

			if (distance < minDistance) {
				minDistance = distance;
				nearestStageObjectAddr = validStageObjectAddr;  // Atualiza o endere�o do objeto mais pr�ximo
			}
		}
	}

	return nearestStageObjectAddr; // Retorna o valor (0 se nenhum endere�o foi encontrado)
}


std::wstring ObstacleReader::getObstacleName(const Obstacle& objBuffer) {
	wchar_t nameBuffer[100] = { 0 };
	SIZE_T bytesReadInner = 0;
	if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
		reinterpret_cast<LPCVOID>(objBuffer.namePtr),
		nameBuffer, sizeof(nameBuffer) - sizeof(wchar_t), &bytesReadInner) && bytesReadInner > 0) {

		// Finalizar a string com `\0` para seguran�a
		nameBuffer[bytesReadInner / sizeof(wchar_t)] = L'\0';
		return std::wstring(nameBuffer);
	}
	return L"";
}

std::set<std::wstring> ObstacleReader::getObstacleData() {
	std::set<std::wstring> setNames;
	// Conjunto para armazenar nomes �nicos

	stageObjBrowser();
	auto stufsAddrs = getStageObjectsAddrs();

	for (const auto& stuffAddr : stufsAddrs) {
		// L� os dados da entidade
		auto stuff = readStageObjAddr(stuffAddr);
		std::wstring stuffName = getObstacleName(stuff);
		setNames.insert(stuffName);
	}
	return setNames;
}
