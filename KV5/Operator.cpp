#include "Operator.h"

Operator::Operator(Base* pHeadObject, string objectName) : Base(pHeadObject, objectName) {}

//метод выдающий сигнал с веденной командой
void Operator::GetInputSignal(string& info)
{
	string input;
	getline(cin, input); // считываем ввод
	info = input;
}

//метод обработчик
string Operator::GetInputHandler(string info)
{
	return info;
}
