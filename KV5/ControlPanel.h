#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H
#include "Base.h"

class ControlPanel : public Base
{
private:
	vector<int> iDs;
	vector<Message> messagesQueue;
	vector<Message> typingMessages;
	bool IsLastCommandWasSend = false;
	void ProceedQueue();
	void ProceedTypingList();
	bool CheckNumber(int number);
	void TrySendAMessage(string message);
	bool CheckNumberWithSignal(int number);
public:
	ControlPanel(Base* pHeadObject, string objectName);
	string CommandProcessingHandler(string command);
	string SetIds(string numbers);
};

#endif // !CONTROL_PANEL_H

