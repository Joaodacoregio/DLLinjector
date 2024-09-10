// mainclass.cpp
#include "mainclass.h"

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "DLL INJECTOR", wxDefaultPosition, wxSize(500, 300))
{
    // Painel principal
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Layouts
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* middleSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* footSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Rótulo "Selecionar Processo"
    wxStaticText* lblSelectProcess = new wxStaticText(panel, wxID_ANY, "Selecionar Processo:");

    // Alterando a fonte do rótulo para +2 no tamanho e em negrito
    wxFont font = lblSelectProcess->GetFont();
    font.SetPointSize(font.GetPointSize() + 2);  // Aumentar a fonte em 2
    font.MakeBold();  // Negrito
    lblSelectProcess->SetFont(font);

    // Caixa de entrada (input box)
    wxTextCtrl* txtProcessInput = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1), wxTE_READONLY);

    // Botão "Selecionar Processo"
    wxButton* btnSelectProcess = new wxButton(panel, wxID_ANY, "Selecionar");

    // Adicionando rótulo, caixa de entrada e botão ao topSizer
    topSizer->Add(lblSelectProcess, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);  // Rótulo com espaçamento à direita
    topSizer->Add(txtProcessInput, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);  // Caixa de entrada
    topSizer->Add(btnSelectProcess, 0, wxALIGN_CENTER_VERTICAL);  // Botão

    // Criando a StaticBox com o botão "Adicionar DLL" e a lista de DLLs
    wxStaticBox* staticBox = new wxStaticBox(panel, wxID_ANY, "DLLs");  // Caixa com borda e título
    wxStaticBoxSizer* boxSizer = new wxStaticBoxSizer(staticBox, wxVERTICAL);

    // Layout para o botão "Adicionar DLL" dentro da box
    wxBoxSizer* boxButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* btnAddDll = new wxButton(panel, wxID_ANY, "Adicionar DLL");

    // Adicionando o botão "Adicionar DLL" ao canto superior direito da box
    boxButtonSizer->AddStretchSpacer(1);  // Espaço flexível à esquerda
    boxButtonSizer->Add(btnAddDll, 0, wxALIGN_RIGHT | wxTOP | wxRIGHT, 5);  // Alinhar o botão no canto superior direito

    // Lista de DLLs
    wxListBox* listDlls = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(200, 100), 0, nullptr, wxLB_SINGLE);

    // Criando o botão para injetar DLL
    wxButton* btnInjectDLL = new wxButton(panel, wxID_ANY, "Injetar DLL\s");
    btnInjectDLL->SetMinSize(wxSize(100,25));

    // Adicionando o botão e a lista ao boxSizer
    boxSizer->Add(boxButtonSizer, 0, wxEXPAND);
    boxSizer->Add(listDlls, 1, wxEXPAND | wxALL, 5);  // Lista expandível
    boxSizer->Add(btnInjectDLL, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);  // Botão de injeção no final

    // Adicionando o StaticBoxSizer ao middleSizer
    middleSizer->Add(boxSizer, 1, wxEXPAND | wxALL, 10);  // Caixa com borda no rodapé

    // Adicionando sizers ao painel
    mainSizer->Add(topSizer, 0, wxEXPAND | wxALL, 10);  // Espaço ao redor
    mainSizer->Add(middleSizer, 1, wxEXPAND);  // Rodapé expandido

    panel->SetSizer(mainSizer);
    this->Center();
}
