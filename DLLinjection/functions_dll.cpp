#include "functions_dll.h"

void HandleSelectDLL(wxListBox* listDlls) {
    wxFileDialog openFileDialog(nullptr, "Selecionar DLL", "", "", "DLL files (*.dll)|*.dll", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return;  // O usuário cancelou a seleção do arquivo
    }

    wxString dllPath = openFileDialog.GetPath();
    listDlls->Append(dllPath);  // Adicione o caminho da DLL à lista
}

int injectSingleDLL(HANDLE hProcess, const wxString& dllPath) {
    size_t dllPathLen = (dllPath.length() + 1) * sizeof(wchar_t);

    // Alocar memória no processo alvo
    LPVOID pRemoteMemory = VirtualAllocEx(hProcess, NULL, dllPathLen, MEM_COMMIT, PAGE_READWRITE);
    if (pRemoteMemory == NULL) {
        wxMessageBox("Erro ao alocar memória no processo alvo.", "Erro", wxICON_ERROR);
        return -1;
    }

    // Escrever o caminho da DLL na memória do processo alvo
    if (!WriteProcessMemory(hProcess, pRemoteMemory, dllPath.wc_str(), dllPathLen, NULL)) {
        wxMessageBox("Erro ao escrever na memória do processo alvo.", "Erro", wxICON_ERROR);
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        return -1;
    }

    // Obter o handle do Kernel32.dll
    HMODULE hKernel32 = GetModuleHandleW(L"Kernel32.dll");
    if (hKernel32 == NULL) {
        wxMessageBox("Erro ao obter o handle do Kernel32.dll.", "Erro", wxICON_ERROR);
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        return -1;
    }

    // Obter o endereço de LoadLibraryW no Kernel32.dll
    LPVOID pLoadLibraryW = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryW");
    if (pLoadLibraryW == NULL) {
        wxMessageBox("Erro ao obter o endereço de LoadLibraryW.", "Erro", wxICON_ERROR);
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        return -1;
    }

    // Criar a thread remota no processo alvo
    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibraryW, pRemoteMemory, 0, NULL);
    if (hRemoteThread == NULL) {
        wxMessageBox(wxString::Format("Erro ao criar thread remota no processo alvo. Código de erro: %lu", GetLastError()), "Erro", wxICON_ERROR);
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        return -1;
    }

    // Esperar a thread remota terminar
    WaitForSingleObject(hRemoteThread, INFINITE);
    CloseHandle(hRemoteThread);
    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);

    return 0;
}

int injectDLL(wxListBox* listDlls, wxTextCtrl* txtProcessInput) {
    // Verificar se algum processo foi selecionado
    if (txtProcessInput->IsEmpty()) {
        wxMessageBox("Nenhum processo selecionado!", "Erro", wxICON_ERROR);
        return -1;
    }

    // Obter o PID do processo
    wxString processInfo = txtProcessInput->GetValue();
    long pid;

    // Supondo que o formato seja "PID: NomeProcesso", extrair o PID
    if (!processInfo.BeforeFirst(':').ToLong(&pid)) {
        wxMessageBox("Falha ao obter o PID do processo!", "Erro", wxICON_ERROR);
        return -1;
    }

    // Verificar se existem DLLs na lista
    if (listDlls->GetCount() == 0) {
        wxMessageBox("Nenhuma DLL adicionada!", "Erro", wxICON_ERROR);
        return -1;
    }

    // Abrir o processo com permissões de injeção
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        wxMessageBox(wxString::Format("Erro ao abrir o processo com PID: %ld", pid), "Erro", wxICON_ERROR);
        return -1;
    }

    // Iterar sobre a lista de DLLs e injetar uma por uma
    for (size_t i = 0; i < listDlls->GetCount(); i++) {
        wxString dllPath = listDlls->GetString(i);

        // Exibir mensagem com o caminho da DLL atual
        wxString message = wxString::Format("Injetando DLL: %s", dllPath);
        wxMessageBox(message, "Injeção de DLL", wxICON_INFORMATION);

        // Injetar a DLL atual
        if (injectSingleDLL(hProcess, dllPath) != 0) {
            wxMessageBox(wxString::Format("Falha ao injetar a DLL: %s", dllPath), "Erro", wxICON_ERROR);
            CloseHandle(hProcess);
            return -1;
        }
    }

    // Fechar o handle do processo
    CloseHandle(hProcess);

    wxMessageBox("Injeção de todas as DLLs concluída com sucesso!", "Sucesso", wxICON_INFORMATION);
    return 0;
}
