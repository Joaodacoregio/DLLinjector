// mainGUI.h
#ifndef MAINGUI_H
#define MAINGUI_H

 
#include <thread>
#include <atomic>
#include <wx/wx.h>


class MyFrame : public wxFrame
{
public:
    MyFrame();
    ~MyFrame() {StopBot();}

private:
    void OnStartBot(wxCommandEvent& event);
    void OnSelectStage(wxCommandEvent& event);
    void StopBot();

    wxTextCtrl* terminal;           // Terminal (wxTextCtrl)
    std::atomic<bool> isRunning;   // Flag para controlar o loop da thread
    std::thread botThread;         // Thread do bot

};

enum
{
    ID_StartBot = 1,
    ID_SelectStage
};

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

#endif // MAINGUI_H