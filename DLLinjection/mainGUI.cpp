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

void MyFrame::OnStartBot(wxCommandEvent& event)
{
    ObjectRenderReader reader;

    // Simulate logging into the terminal (wxTextCtrl)
    terminal->AppendText("Bot started...\n");
    StageRange sRange = { 0.0f,9999.0f,9999.0f,0.0f };
    Entity entBuffer;

    if (reader.isReadAllRootsAddrs(reader.getPtrProcessReader()->getProcessHandle())) {
        reader.getPtrEntityReader()->validEntitiesAddr(sRange);
    }

    for (const auto& addrs : reader.getPtrEntityReader()->getRangeEntitiesAddrs()) {
        entBuffer = reader.getPtrEntityReader()->readEntityAddr(addrs);
        std::wstring name = reader.getPtrEntityReader()->getEntityName(entBuffer);
        terminal->AppendText(L"Detected entity, your X and Y are->" + std::to_wstring(entBuffer.X) + L" and " + std::to_wstring(entBuffer.Y) + L"\n");

    }
 

}

void MyFrame::OnSelectStage(wxCommandEvent& event)
{
    // Simulate logging into the terminal (wxTextCtrl)
    terminal->AppendText("Stage selected...\n");
}

 