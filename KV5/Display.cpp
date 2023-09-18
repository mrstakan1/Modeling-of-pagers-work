#include "Display.h"

//Конструктор объекта
Display::Display(Base* pHeadObject, string objectName) : Base(pHeadObject, objectName) {}

//Метод выводящий полученный сигнал на экран
string Display::PrintHandler(string info)
{
	cout << info;
	return "";
}