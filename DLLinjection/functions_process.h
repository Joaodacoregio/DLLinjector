#ifndef	FUNCTIONS_PROCESS_H
#define FUNCTIONS_PROCESS_H

#include "include.h"

void HandleListProcess(wxTextCtrl* txtProcessInputMain);
void ListProcesses(wxListBox* listBox, wxTextCtrl* txtProcessInput);
void OnProcessSelected(wxListBox* listBox, wxTextCtrl* txtProcessInput, wxTextCtrl* txtProcessInputMain);

#endif // FUNCTIONS_PROCESS_H
