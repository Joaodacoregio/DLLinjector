#include "mainGUI.h"
#include "stageRadar.h"
 

MyFrame* MyFrame::instance = nullptr;


bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "LuniaBot", wxDefaultPosition, wxSize(900, 400))
{
    // main panel
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    // Create StartBot button
    wxButton* startBotButton = new wxButton(panel, ID_StartBot, "StartBot", wxPoint(20, 20), wxSize(120, 40));

    // Create SelectStage button
    wxButton* selectStageButton = new wxButton(panel, ID_SelectStage, "SelectStage", wxPoint(160, 20), wxSize(120, 40));

    // Create the terminal (text box) to simulate a console
    terminal = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(20, 80), wxSize(840, 250), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);

    // Bind the events to their handlers
    Bind(wxEVT_BUTTON, &MyFrame::OnStartBot, this, ID_StartBot);
    Bind(wxEVT_BUTTON, &MyFrame::OnSelectStage, this, ID_SelectStage);

    // Centralize the window
    this->Center();

    // Guardar instância
    instance = this;
}

void MyFrame::OnStartBot(wxCommandEvent& event) {
    if (isRunning) {
        terminal->AppendText("Bot already running!\n");
        return;
    }

    terminal->AppendText("Bot started...\n");
    isRunning = true;

    // Inicia a thread do bot
    botThread = std::thread([this]() {
        StageRange range = { 0.0f,9999.0f , 9999.0f , 0.0f };
        ObjectRenderReader reader;
        reader.isReadAllRootsAddrs(reader.getPtrProcessReader()->getProcessHandle());


        StageRadar radar(&reader);
 

       
            while (isRunning) {
                radar.scanAllObjRenderInRange(range);
                // Simula a leitura dos dados do jogador
                uintptr_t nearestRenderObjAddress = radar.getNearestRenderObject(range);
 
                std::wstring objName = radar.getRenderObjectName(nearestRenderObjAddress);

                terminal->AppendText(objName);

                // Espera 1 segundo
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
 
        });

    botThread.detach(); // Torna a thread independente
}

void MyFrame::OnSelectStage(wxCommandEvent& event)
{
    // Simulate logging into the terminal (wxTextCtrl)
    terminal->AppendText("Stage selected...\n");
}

 


void MyFrame::StopBot() {
    if (isRunning) {
        terminal->AppendText("Stopping bot...\n");
        isRunning = false;
        // Espera a thread terminar se ela não estiver destacada (opcional)
    }
}

void MyFrame::writeTerminal(std::wstring text)
{
    if (instance && instance->terminal)
    {
        instance->terminal->AppendText(text);
    }
}


 