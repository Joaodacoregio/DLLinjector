#include "mainGUI.h"
 
#include "objectRenderReader.h"

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
        ObjectRenderReader reader;
        Player player;

        if (reader.isReadAllRootsAddrs(reader.getPtrProcessReader()->getProcessHandle())) {
            while (isRunning) {
                // Simula a leitura dos dados do jogador
                reader.getPtrPlayerReader()->readPlayerAddr();
                player = reader.getPtrPlayerReader()->getPlayer();

                // Atualiza o terminal de forma thread-safe
                wxTheApp->CallAfter([this, player]() {
                    terminal->AppendText(L"Detected player, your X and Y are -> " +
                        std::to_wstring(player.X) + L" and " +
                        std::to_wstring(player.Y) + L"\n");
                    });

                // Espera 1 segundo
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
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