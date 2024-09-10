#include <wx/wx.h>

class MyFrame : public wxFrame
{
public:
    MyFrame() : wxFrame(NULL, wxID_ANY, "Minha Janela wxWidgets", wxDefaultPosition, wxSize(400, 200))
    {
        // Painel principal
        wxPanel* panel = new wxPanel(this, wxID_ANY);

        // Layout
        wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Bot�es da parte superior
        wxButton* btnSelectDll = new wxButton(panel, wxID_ANY, "Selecionar DLL");
        wxButton* btnSelectProcess = new wxButton(panel, wxID_ANY, "Selecionar Processo");

        // Define o tamanho m�nimo dos bot�es
        btnSelectDll->SetMinSize(wxSize(150, 30)); // Largura m�nima de 150 pixels e altura m�nima de 30 pixels
        btnSelectProcess->SetMinSize(wxSize(150, 30)); // Largura m�nima de 150 pixels e altura m�nima de 30 pixels

        topSizer->Add(btnSelectDll, 0, wxALL | wxALIGN_LEFT, 10);
        topSizer->Add(btnSelectProcess, 0, wxALL | wxALIGN_RIGHT, 10);

        // Bot�o da parte inferior
        wxButton* btnInjectDll = new wxButton(panel, wxID_ANY, "Injetar DLL");
        bottomSizer->Add(btnInjectDll, 0, wxALL | wxALIGN_CENTER, 10);

        // Adicionando sizers ao painel
        mainSizer->Add(topSizer, 0, wxEXPAND);
        mainSizer->AddStretchSpacer(1);  // Espa�o flex�vel
        mainSizer->Add(bottomSizer, 0, wxALIGN_BOTTOM);

        panel->SetSizer(mainSizer);
        this->Center();
    }
};

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        MyFrame* frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
