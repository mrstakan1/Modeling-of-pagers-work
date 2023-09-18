#ifndef BASE_H
#define BASE_H
#include <iostream>
#include <vector>
#include <signal.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <queue>

using namespace std;

class Base;

typedef void (Base ::* TYPE_SIGNAL) (string&);
typedef string(Base ::* TYPE_HANDLER) (string);
#define SIGNAL_D( signal_f ) ( TYPE_SIGNAL ) ( & signal_f )
#define HANDLER_D( handler_f ) ( TYPE_HANDLER ) ( & handler_f )

struct Connection
{
	string signalName;
	TYPE_SIGNAL pSignal;
	Base* pTarget;
	TYPE_HANDLER pHandler;
};

struct Message
{
	string message;
	int idFrom;
	int idTo;
	int tickMadeSent;
	int ticksLeft;
	int createTick;
};

class Base
{
private:
	string objectName;
	Base* pHeadObject = nullptr;
	vector<Connection*> connections;
	vector<Base*> subordinateObjects;
	int objState = 1;

public:
	int tick = 0;
	void SetHead(Base* pHead);
	void SetName(string name);
	string GetName();
	int GetState();
	void SetState(int state);
	Base* FindObject(Base* currentObject, string objectPath);
	Base* GetHead();
	Base(Base* pHeadObject, string objectName = "Root_Object");
	void SetConnect(string signalName, TYPE_SIGNAL pSignal = nullptr, Base* pTarget = nullptr, TYPE_HANDLER pHandler = nullptr);
	string EmitSignal(string signalName, string command = "null");
	void PrintTree(string tab = "    ");
};

#endif // !BASE

