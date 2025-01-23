
#include "playerReader.h"


void PlayerReader::readPlayerAddr() {
    Player playerBuffer;
    SIZE_T bytesRead;

    if (ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle() == nullptr) {
        std::cerr << "HANDLE do processo é inválido!" << std::endl;
    }

    // Lê os dados do player, sem tentar copiar ponteiros diretamente
    if (!ReadProcessMemory(ptrReaderObjectRender->getPtrProcessReader()->getProcessHandle()
        , reinterpret_cast<LPCVOID>(ptrReaderObjectRender->getRootPlayerAddress()),
        &playerBuffer, sizeof(Player), &bytesRead) || bytesRead != sizeof(Player)) {
        std::cerr << "Erro ao ler o jogador no endereço de memória fornecido." << std::endl;
    }

    // Retorne o objeto configurado
    this->player = playerBuffer;

}

void PlayerReader::infoDataPlayer() {
    Player player;
    while (true) {
        ptrReaderObjectRender->getPtrPlayerReader()->readPlayerAddr();
        player = ptrReaderObjectRender->getPtrPlayerReader()->player;

        system("CLS");

        // Exibindo as informações do jogador
        std::wcout << "Vida do personagem: " << player.life << std::endl;
        std::wcout << "Mana do personagem: " << player.mana << std::endl;
        std::wcout << "Posição X do personagem: " << player.X << std::endl;
        std::wcout << "Posição Y do personagem: " << player.Y << std::endl;
        std::wcout << "Vetor X: " << player.VectorX << std::endl;
        std::wcout << "Vetor Y: " << player.VectorY << std::endl;


        // Adicione um intervalo para evitar que o terminal seja limpado rapidamente
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}