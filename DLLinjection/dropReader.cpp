#include "includes.h"
#include "dropReader.h"

#define MIN_MAGNITUDE_THRESHOLD 1.0f   // Limite m�nimo para valores v�lidos de posi��o
#define MAX_THRESHOLD 10000.0f         // Limite superior de posi��o para valida��o
#define NAME_BUFFER_SIZE 200

bool DropReader::dropNav(uintptr_t dropAddr) {
	// Verifica se o endere�o do drop j� est� na lista
	return std::any_of(dropsAddrs.begin(), dropsAddrs.end(), [&](uintptr_t addr) { return addr == dropAddr; });
}

// Fun��o para coletar drops a partir de uma raiz especificada
void DropReader::collectDrops(uintptr_t nextDropAddrs)
{
	if (!nextDropAddrs) return; // Caso o endere�o da raiz seja inv�lido, sai da fun��o

	Drop dropBuffer;
	SIZE_T bytesRead;

	// L� o est�gio de drops da mem�ria do processo
	if (!ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
		reinterpret_cast<LPCVOID>(nextDropAddrs), &dropBuffer, sizeof(dropBuffer), &bytesRead) ||
		bytesRead != sizeof(Drop)) {
		// std::cerr << "Erro ao ler a entidade no endere�o: " << std::hex << dropRootAddr << std::endl;
		return;
	}

	// Verifica se n�o existem drops renderizados ao comparar os ponteiros
	if (reinterpret_cast<uintptr_t>(dropBuffer.ptrOne) == nextDropAddrs &&
		reinterpret_cast<uintptr_t>(dropBuffer.ptrTwo) == nextDropAddrs &&
		reinterpret_cast<uintptr_t>(dropBuffer.ptrThre) == nextDropAddrs) {
		return; // Sai da fun��o se n�o houver drops
	}

	// Verifica se o dropRootAddr j� est� na lista
	if (!dropNav(nextDropAddrs)) {
		dropsAddrs.push_back(nextDropAddrs);

		// Recursivamente coleta entidades ptrOne, ptrTwo e ptrThre, se existirem
		if (dropBuffer.ptrOne) {
			collectDrops(reinterpret_cast<uintptr_t>(dropBuffer.ptrOne));
		}
		if (dropBuffer.ptrTwo) {
			collectDrops(reinterpret_cast<uintptr_t>(dropBuffer.ptrTwo));
		}
		if (dropBuffer.ptrThre) {
			collectDrops(reinterpret_cast<uintptr_t>(dropBuffer.ptrThre));
		}
	}
}

// Fun��o principal para buscar e retornar todos os drops encontrados
void  DropReader::dropBrowser() {
	dropsAddrs.clear();


	// Chama a fun��o `collectDrops` para come�ar a coleta a partir do endere�o fornecido
	collectDrops(ptrReaderObjectRender->getRootDropAddress());
}


void DropReader::validDropsAddr(StageRange range) {
	rangeDropsAddrs.clear();
	dropBrowser();

	for (const auto& dropAddr : dropsAddrs) {
		Drop drop;
		SIZE_T bytes;

		if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle()
			, reinterpret_cast<LPCVOID>(dropAddr), &drop, sizeof(drop), &bytes) &&
			bytes == sizeof(drop)) {
			std::pair<float, float> dropCords = std::make_pair(drop.X, drop.Y);
			if ((std::abs(drop.X) >= MIN_MAGNITUDE_THRESHOLD && std::abs(drop.X) <= MAX_THRESHOLD) &&
				(std::abs(drop.Y) >= MIN_MAGNITUDE_THRESHOLD && std::abs(drop.Y) <= MAX_THRESHOLD)) {
				if (isPointInBounds(dropCords, range.p1x, range.p1y, range.p2x, range.p2y)) {
					rangeDropsAddrs.push_back(dropAddr);
				}
			}

		}
	}
}

void DropReader::readDrops() {
	drops.clear();

	for (const auto& dropAddr : dropsAddrs) {
		Drop drop;
		SIZE_T bytes;

		if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle()
			, reinterpret_cast<LPCVOID>(dropAddr), &drop, sizeof(drop), &bytes) &&
			bytes == sizeof(drop)) {
			drops.push_back(drop);
		}
	}
}

Drop DropReader::readDropAddr(uintptr_t dropAddrs) {
	Drop drop;
	SIZE_T bytes;

	if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle()
		, reinterpret_cast<LPCVOID>(dropAddrs), &drop, sizeof(drop), &bytes) &&
		bytes == sizeof(drop)) {
		return drop;
	}
}



void DropReader::validDrops(StageRange range) {


	dropBrowser();
	readDrops();

	for (const auto& drop : drops) {
		// Verifica se X e Y est�o dentro dos limites desejados
		if ((std::abs(drop.X) >= MIN_MAGNITUDE_THRESHOLD && std::abs(drop.X) <= MAX_THRESHOLD) &&
			(std::abs(drop.Y) >= MIN_MAGNITUDE_THRESHOLD && std::abs(drop.Y) <= MAX_THRESHOLD)) {
			std::pair<float, float> dropCords = std::make_pair(drop.X, drop.Y);
			if (isPointInBounds(dropCords, range.p1x, range.p1y, range.p2x, range.p2y)) {
				rangeDrops.push_back(drop);
			}
		}
	}


}


Drop DropReader::getNearestDrop(Player player) {
	Drop nearestDrop{};
	float minDistance = DEFAULT_MIN_DISTANCE;

	for (const auto& drop : drops) {
		// Calcula a dist�ncia euclidiana entre o player e a entidade
		float disX = drop.X - player.X;
		float disY = drop.Y - player.Y;
		float distance = std::sqrt(disX * disX + disY * disY);  // Dist�ncia Euclidiana

		// Verifica se a dist�ncia atual � a menor encontrada at� agora
		if (distance < minDistance) {
			minDistance = distance;
			nearestDrop = drop;  // Atualiza a entidade mais pr�xima
		}
	}

	return nearestDrop;
}


uintptr_t DropReader::getNearestDropAddr(Player player)
{
	uintptr_t nearestDropAddrs = 0; // Inicializa com um valor padr�o
	float minDistance = DEFAULT_MIN_DISTANCE;

	for (const auto& validDropAddr : rangeDropsAddrs) {
		Drop drop;
		SIZE_T bytes;
		if (ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle()
			, reinterpret_cast<LPCVOID>(validDropAddr), &drop, sizeof(drop), &bytes) &&
			bytes == sizeof(drop)) {

			// Calcula a dist�ncia euclidiana entre o player e a entidade
			float disX = drop.X - player.X;
			float disY = drop.Y - player.Y;
			float distance = std::sqrt(disX * disX + disY * disY);  // Dist�ncia Euclidiana

			if (distance < minDistance) {
				minDistance = distance;
				nearestDropAddrs = validDropAddr;  // Atualiza para o endere�o do drop mais pr�ximo
			}
		}
	}

	return nearestDropAddrs; // Retorna o valor (0 se nenhum endere�o foi encontrado)
}

std::set<std::wstring> DropReader::getDropData() {
	std::set<std::wstring> setNames;
	// Conjunto para armazenar nomes �nicos

	dropBrowser();
	auto dropAddrs = getDropsAddrs();

	for (const auto& dropAddr : dropAddrs) {
		// L� os dados da entidade
		auto stuff = readDropAddr(dropAddr);
		std::wstring dropName = getDropName(stuff);
		setNames.insert(dropName);
	}
	return setNames;
}


std::wstring DropReader::getDropName(Drop& dropBuffer) {
	unsigned char namePointerBuffer[NAME_BUFFER_SIZE] = { 0 };
	SIZE_T bytesRead = 0;

	// Ler o ponteiro do nome da entidade a partir do endere�o do buffer da entidade
	if (ReadProcessMemory(
		ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle(),
		reinterpret_cast<LPCVOID>(dropBuffer.namePtr),
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

				// Finalizar a string com `\0` para seguran�a
				nameBuffer[bytesReadInner / sizeof(wchar_t)] = L'\0';
				return std::wstring(nameBuffer);
			}

			// Caso falhe, tentar ler como texto unicode puro no deslocamento 0x08
			wchar_t* unicodeTextPtr = reinterpret_cast<wchar_t*>(namePointerBuffer + 0x08);
			std::wstring unicodeText(unicodeTextPtr, NAME_BUFFER_SIZE / sizeof(wchar_t));
			size_t nullTerminatorPos = unicodeText.find(L'\0');
			if (nullTerminatorPos != std::wstring::npos) {
				unicodeText = unicodeText.substr(0, nullTerminatorPos); // Remover dados al�m do terminador nulo
				return unicodeText;
			}
		}
	}

	return L"UnnamedDrop"; // Retorno vazio em caso de falha
}

uintptr_t DropReader::getValidDropAddrsUsingName(const std::wstring& name) {
	Drop drop;
	std::wstring dropName;


	for (const auto& validAddrs : rangeDropsAddrs) {
		drop = readDropAddr(validAddrs);
		dropName = getDropName(drop);

		if (dropName == name) {
			return validAddrs;
		}
	}
	return 0;
}