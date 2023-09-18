#ifndef PAGER_H
#define PAGER_H
#include "Base.h"

class Pager : public Base
{
private:
	int _iD;
	bool isBusy = false;
	vector<Message> sentMessages;
	vector<Message> receivedMessages;
	void SortMessages();
public:
	Pager(Base* pHeadObject, string objectName, int id);
	virtual int GetId();
	void PagerStatusSignal(string& signal);
	void Handler(string handler);
	string SetPagerStateHandler(string state);
	string GetPagerState(string state);
	string AddSentMessageHandler(string message);
	string AddReceivedMessageHandler(string message);
	string IncrementSent(string message);
	int GetSentMessagesCount();
	int GetReceivedMessagesCount();
	void SendMessageSignal(string& message);
};

#endif