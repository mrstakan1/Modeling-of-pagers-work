#ifndef OPERATOR_H
#define OPERATOR_H
#include "Base.h"

class Operator : public Base
{
private:

public:
	Operator(Base* pHeadObject, string objectName);
	void GetInputSignal(string& command);
	string GetInputHandler(string info);
};

#endif