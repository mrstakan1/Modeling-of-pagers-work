#ifndef SYSTEM_H
#define SYSTEM_H

#include "Base.h"
#include "Pager.h"
#include "Operator.h"
#include "ControlPanel.h"
#include "Display.h"

class System : public Base
{
private:
	vector<Pager*> pagers;
	int tick = 0;
	int processedSignals = 0;
	int processingSignals = 0;
	bool IsPagerIdExist(int id);
	void SortPagers();

public:
	void Start();
	System(Base* pHeadObject, string objectName = "System");
	int ExecApp();
	void GetSystemStatusSignal(string& signal);
	void SentPagersNumbersSignal(string& signal);
	TYPE_SIGNAL GetSignal(string methodName);
	TYPE_HANDLER GetHandler(string methodName);
	string TickStepHandler(string command);
	string GetCurrentTickHandler(string info);
	string EditProcessedMessagesHandler(string info);
	string AddProcessingMessagesHandler(string info);
};


#endif
