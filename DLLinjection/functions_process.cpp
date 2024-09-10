#include "functions_process.h"

void HandleListProcess(wxTextCtrl* txtProcessInputMain) {
    wxDialog* dlg = new wxDialog(wxTheApp->GetTopWindow(), wxID_ANY, "Selecionar Processo", wxDefaultPosition, wxSize(300, 400));
    wxBoxSizer* dlgSizer = new wxBoxSizer(wxVERTICAL);
    wxListBox* listBox = new wxListBox(dlg, wxID_ANY);
    wxTextCtrl* txtProcessInput = new wxTextCtrl(dlg, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1), wxTE_READONLY);
    wxButton* btnSelect = new wxButton(dlg, wxID_OK, "Selecionar");

    dlgSizer->Add(listBox, 1, wxEXPAND | wxALL, 10);
    dlgSizer->Add(txtProcessInput, 0, wxEXPAND | wxALL, 10);
    dlgSizer->Add(btnSelect, 0, wxALIGN_CENTER | wxALL, 10);

    dlg->SetSizer(dlgSizer);

    // Listar processos
    ListProcesses(listBox, txtProcessInput);

    // Atualizar a função de seleção do processo
    listBox->Bind(wxEVT_LISTBOX, [listBox, txtProcessInput, txtProcessInputMain](wxCommandEvent&) {
        OnProcessSelected(listBox, txtProcessInput, txtProcessInputMain);
        });

    if (dlg->ShowModal() == wxID_OK) {
        // Atualizar o campo de entrada com o nome do processo selecionado
        txtProcessInput->SetValue(txtProcessInput->GetValue());
    }

    dlg->Destroy();
}

void ListProcesses(wxListBox* listBox, wxTextCtrl* txtProcessInput) {
    HANDLE hSnapshot;
    PROCESSENTRY32 pe32;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        wxMessageBox("Não foi possível criar um snapshot de processos.", "Erro", wxOK | wxICON_ERROR);
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe32)) {
        wxMessageBox("Não foi possível obter informações do primeiro processo.", "Erro", wxOK | wxICON_ERROR);
        CloseHandle(hSnapshot);
        return;
    }

    listBox->Clear();

    do {
        if (wcscmp(pe32.szExeFile, L"System Idle Process") != 0 && wcscmp(pe32.szExeFile, L"System") != 0) {
            wxString processInfo;
            processInfo.Printf("%d: %s", pe32.th32ProcessID, pe32.szExeFile);
            listBox->Append(processInfo);
            listBox->SetClientData(listBox->GetCount() - 1, (void*)pe32.th32ProcessID);  // Armazenar PID
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
}

void OnProcessSelected(wxListBox* listBox, wxTextCtrl* txtProcessInput, wxTextCtrl* txtProcessInputMain) {
    // Obter o índice selecionado na lista
    int selection = listBox->GetSelection();
    if (selection != wxNOT_FOUND) {
        // Obter o texto do processo selecionado
        wxString selectedProcess = listBox->GetString(selection);
        // Atualizar o campo de entrada com o nome do processo
        txtProcessInput->SetValue(selectedProcess);
        // Atualizar o campo de entrada principal com o nome do processo
        txtProcessInputMain->SetValue(selectedProcess);
    }
}
