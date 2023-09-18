#ifndef DISPLAY_H
#define DISPLAY_H
#include "Base.h"

class Display : public Base
{
public:
	Display(Base* pHeadObject, string objectName);
	string PrintHandler(string info);
};
#endif // !DISPLAY_H
