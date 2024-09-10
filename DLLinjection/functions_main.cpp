// functions_main.cpp
#include "functions_main.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/choice.h>
#include <windows.h>
#include <psapi.h>

// Fun��o auxiliar para mostrar um di�logo de sele��o de processo
DWORD ShowProcessSelectionDialog()
{
    // Obt�m a lista de processos
    DWORD processes[1024], needed;
    if (!EnumProcesses(processes, sizeof(processes), &needed))
        return 0;

    // Calcula o n�mero de processos encontrados
    int count = needed / sizeof(DWORD);

    // Cria uma janela para a sele��o
    wxDialog dlg(NULL, wxID_ANY, "Selecionar Processo", wxDefaultPosition, wxSize(100, 100));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Cria uma lista para mostrar os processos
    ewxChoic* choice = new wxChoice(&dlg, wxID_ANY);
    sizer->Add(choice, 1, wxALL | wxEXPAND, 5);

    // Preenche a lista com os processos
    for (int i = 0; i < count; ++i)
    {
        TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
        if (hProcess)
        {
            HMODULE hMod;
            DWORD cbNeeded;
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
            {
                GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
            }
            CloseHandle(hProcess);
        }

        // Ignora processos com nome "<unknown>"
        if (wxStrcmp(szProcessName, TEXT("<unknown>")) != 0)
        {
            choice->Append(wxString(szProcessName) + wxString::Format(wxT(" (PID: %u)"), processes[i]));
        }
    }

    // Ajuste o tamanho do wxChoice ap�s adicionar os itens
    choice->SetSize(choice->GetBestSize()); // Ajuste o tamanho com base no melhor tamanho poss�vel

    // Adiciona um bot�o OK
    wxButton* btnOk = new wxButton(&dlg, wxID_OK, "OK");
    sizer->Add(btnOk, 0, wxALL | wxALIGN_CENTER, 5);

    dlg.SetSizer(sizer);
    dlg.Layout();
    dlg.Centre();
    dlg.ShowModal();

    // Obt�m o PID do processo selecionado
    int selection = choice->GetSelection();
    if (selection != wxNOT_FOUND)
    {
        wxString selected = choice->GetString(selection);
        wxString pidString = selected.AfterLast(wxT(' ')).BeforeLast(wxT(' '));
        return wxAtoi(pidString);
    }

    return 0; // Retorna 0 se nenhum processo for selecionado
}


// Fun��o para manipular o evento de clicar no bot�o "Selecionar Processo"
void OnSelectProcess(wxCommandEvent& event)
{
    DWORD selectedProcessId = ShowProcessSelectionDialog();
    if (selectedProcessId != 0)
    {
        wxString message = wxString::Format("Voc� selecionou o processo com PID: %u", selectedProcessId);
        wxMessageBox(message, "Informa��o", wxOK | wxICON_INFORMATION);
    }
    else
    {
        wxMessageBox("Nenhum processo foi selecionado.", "Informa��o", wxOK | wxICON_INFORMATION);
    }
}

// Outras fun��es
void OnSelectDll(wxCommandEvent& event)
{
    wxFileDialog openFileDialog(NULL, "Escolha uma DLL", "", "", "DLL files (*.dll)|*.dll", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // O usu�rio cancelou a sele��o

    wxString filePath = openFileDialog.GetPath();
    wxMessageBox("Voc� selecionou a DLL: " + filePath, "Informa��o", wxOK | wxICON_INFORMATION);
}

void OnInjectDll(wxCommandEvent& event)
{
    wxMessageBox("Fun��o 'Injetar DLL' ainda n�o implementada.", "Informa��o", wxOK | wxICON_INFORMATION);
}
