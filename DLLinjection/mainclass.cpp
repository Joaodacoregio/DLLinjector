// mainclass.cpp
#include "mainclass.h"

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "DLL INJECTOR", wxDefaultPosition, wxSize(300, 250))
{
    // Painel principal
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Layouts
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* middleSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* footSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Botões da parte superior
    wxButton* btnSelectDll = new wxButton(panel, wxID_ANY, "Selecionar DLL");
    wxButton* btnSelectProcess = new wxButton(panel, wxID_ANY, "Selecionar Processo");

    btnSelectDll->SetMinSize(wxSize(125, 50));
    btnSelectProcess->SetMinSize(wxSize(125, 50));

    topSizer->Add(btnSelectDll, 0, wxALL | wxALIGN_LEFT, 10);
    topSizer->Add(btnSelectProcess, 0, wxALL | wxALIGN_RIGHT, 10);

    // Botão e barra de progresso no centro
    wxButton* btnInjectDll = new wxButton(panel, wxID_ANY, "Injetar DLL");
    btnInjectDll->SetMinSize(wxSize(125, 50));

    // Barra de progresso
    wxGauge* progressBar = new wxGauge(panel, wxID_ANY, 100, wxDefaultPosition, wxSize(250, 25));

    // Adiciona a barra de progresso e o botão ao middleSizer
    middleSizer->AddStretchSpacer(1); // Espaço flexível à esquerda
    middleSizer->Add(progressBar, 0, wxALL | wxALIGN_CENTER, 10);
    middleSizer->AddStretchSpacer(1); // Espaço flexível à direita

    footSizer->Add(btnInjectDll, 0, wxALL | wxALIGN_CENTER, 10);

    // Adicionando sizers ao painel
    mainSizer->Add(topSizer, 0, wxEXPAND);
    mainSizer->AddStretchSpacer(1);  // Espaço flexível entre o topSizer e o middleSizer
    mainSizer->Add(middleSizer, 0, wxALIGN_CENTER);
    mainSizer->AddStretchSpacer(1);  // Espaço flexível entre o middleSizer e o footSizer
    mainSizer->Add(footSizer, 0, wxALIGN_CENTER);

    panel->SetSizer(mainSizer);
    this->Center();
}
