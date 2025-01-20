// mainGUI.h
#ifndef MAINGUI_H
#define MAINGUI_H

#include <wx/wx.h>


class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    void OnStartBot(wxCommandEvent& event);
    void OnSelectStage(wxCommandEvent& event);

    wxTextCtrl* terminal;  // Terminal onde as mensagens serão exibidas
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