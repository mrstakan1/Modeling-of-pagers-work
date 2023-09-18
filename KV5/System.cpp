#include "System.h"

System::System(Base* pHeadObject, string objectName) : Base(pHeadObject, objectName) {};

void System::Start()
{
	Base* pRootObject = this;
	string pagerName;
	Pager* pPager;
	int id;
	//Создание объекта оператора
	Base* Reader = new Operator(pRootObject, "Reader");

	//Создание объекта пульта управления
	Base* Controller = new ControlPanel(pRootObject, "Controller");

	//Создание объекта вывода
	Base* Console = new Display(pRootObject, "Console");

	//Настройка системы (ввод номеров пейджеров)
	do
	{
		getline(cin, pagerName);

		if (pagerName == "End of information about pagers")
			break;
		else
		{
			id = stoi(pagerName);

			if (IsPagerIdExist(id) == false)
			{
				pPager = new Pager(Reader, "Pager_" + pagerName, id);
				pagers.push_back(pPager);
			}
		}
	} while (true);
	SortPagers();

	//Установка необходимых связей
	Reader->SetConnect("ReadCommand", GetSignal("GetInputSignal"), Controller, GetHandler("CommandProcessingHandler"));
	pRootObject->SetConnect("GetSystemStatus", GetSignal("GetSystemStatusSignal"), Console, GetHandler("PrintHandler"));
	pRootObject->SetConnect("PrintOut", nullptr, Console, GetHandler("PrintHandler"));


	Controller->SetConnect("PrintOut", nullptr, Console, GetHandler("PrintHandler"));
	Controller->SetConnect("TickStep", nullptr, pRootObject, GetHandler("TickStepHandler"));
	Controller->SetConnect("GetCurrentTick", nullptr, pRootObject, GetHandler("GetCurrentTickHandler"));

	Controller->SetConnect("SetProcessingMessages", nullptr, pRootObject, GetHandler("SetProcessingMessagesHandler"));
	Controller->SetConnect("EditProcessedMessages", nullptr, pRootObject, GetHandler("EditProcessedMessagesHandler"));

	pRootObject->SetConnect("SendPagerNumbers", GetSignal("SentPagersNumbersSignal"), Controller, GetHandler("SetPagersNumbersHandler"));
	this->EmitSignal("SendPagerNumbers");

	//цикл для установка необходимых связей для всех пейджеров
	for (size_t i = 0; i < pagers.size(); i++)
	{
		pagers[i]->SetConnect("PagerStatus", GetSignal("PagerStatusSignal"), Console, GetHandler("PrintHandler"));
		pagers[i]->SetConnect("SendMessage_" + to_string(pagers[i]->GetId()), GetSignal("SendMessageSignal"), Reader, GetHandler("GetInputHandler"));

		pagers[i]->SetConnect("SetPagerState_" + to_string(pagers[i]->GetId()), nullptr, pagers[i], GetHandler("SetPagerStateHandler"));

		Controller->SetConnect("GetPagerState_" + to_string(pagers[i]->GetId()), nullptr, pagers[i], GetHandler("GetPagerStatusHandler"));
		Reader->SetConnect("ReceiveMessage_" + to_string(pagers[i]->GetId()), nullptr, pagers[i], GetHandler("AddReceivedMessagesHandler"));
	}
}

//Метод запуска программы
int System::ExecApp()
{
	//Получаем указатели на нужные объекты системы
	Base* pRootObj = this;
	Base* Reader = FindObject(pRootObj, "/Reader");
	Base* Controller = FindObject(pRootObj, "/Controller");

	pRootObj->EmitSignal("PrintOut", "Ready to work");

	//строке присваиваем резульатт вызова сигнала у оператора
	string inputReply = Reader->EmitSignal("ReadCommand");

	//цикл работате пока результат вызова сигнала у оператора не равен SHOWTREE или Turn off the system
	while (inputReply != "SHOWTREE" && inputReply != "Turn off the system")
		inputReply = Reader->EmitSignal("ReadCommand");

	if (inputReply == "SHOWTREE") // если цикл завершился по вводу SHOWTREE, то выводим дерево иерархии объектов
		pRootObj->PrintTree();

	return 0;
}

//метод выдающий сигнал с состоянием системы
void System::GetSystemStatusSignal(string& signal)
{
	signal = "";

	//цикл нужен для построения информацию о пейджерах
	for (int i = 0; i < pagers.size(); i++)
	{
		signal += "\nPager " + to_string(pagers[i]->GetId()) + " "
			+ to_string(pagers[i]->GetSentMessagesCount()) + " "
			+ to_string(pagers[i]->GetReceivedMessagesCount());
	}

	//к сигналу добавляем информацию об операторе
	signal += "\nOperator " + to_string(processedSignals) + " "
		+ to_string(processingSignals);
}

//Метод проверки дубляжа имен пейджеров
bool System::IsPagerIdExist(int id)
{
	//цикл для прохождение по уже существующим пейджерам
	for (int i = 0; i < pagers.size(); i++)
	{
		if (pagers[i]->GetId() == id) // если такой пейдер уже существует - возвращаем значение истина
			return true;
	}

	return false;
}

//метод сортировки пейджеров по имени
void System::SortPagers()
{
	//запуск двойного цикла for для выполнения пузырьковой сортировки
	for (int i = 0; i < pagers.size(); i++)
	{
		for (int j = 0; j < pagers.size() - (i + 1); j++)
		{
			if (pagers[j]->GetId() > pagers[j + 1]->GetId())
			{
				swap(pagers[j], pagers[j + 1]);
			}
		}
	}
}

//метод выдающий сигнал с номерами пейджеров
void System::SentPagersNumbersSignal(string& signal)
{
	signal = "";

	//цикл нужен для получения всех номеров пейджеров
	for (int i = 0; i < pagers.size(); i++)
	{
		signal += to_string(pagers[i]->GetId()) + " ";
	}
}

//метод возвращающий указатель на метод сигнала класса по названию
TYPE_SIGNAL System::GetSignal(string methodName)
{
	if (methodName == "GetSystemStatusSignal")
		return SIGNAL_D(System::GetSystemStatusSignal);

	if (methodName == "GetInputSignal")
		return SIGNAL_D(Operator::GetInputSignal);

	if (methodName == "PagerStatusSignal")
		return SIGNAL_D(Pager::PagerStatusSignal);

	if (methodName == "SentPagersNumbersSignal")
		return SIGNAL_D(System::SentPagersNumbersSignal);

	if (methodName == "SendMessageSignal")
		return SIGNAL_D(Pager::SendMessageSignal);

	return nullptr;
}

//метод возвращающий указатель на метод обработчика класса по названию
TYPE_HANDLER System::GetHandler(string methodName)
{
	if (methodName == "PrintHandler")
		return HANDLER_D(Display::PrintHandler);

	if (methodName == "CommandProcessingHandler")
		return HANDLER_D(ControlPanel::CommandProcessingHandler);

	if (methodName == "GetInputHandler")
		return HANDLER_D(Operator::GetInputHandler);

	if (methodName == "TickStepHandler")
		return HANDLER_D(System::TickStepHandler);

	if (methodName == "GetCurrentTickHandler")
		return HANDLER_D(System::GetCurrentTickHandler);

	if (methodName == "SetPagerStateHandler")
		return HANDLER_D(Pager::SetPagerStateHandler);

	if (methodName == "GetPagerStatusHandler")
		return HANDLER_D(Pager::GetPagerState);

	if (methodName == "AddReceivedMessagesHandler")
		return HANDLER_D(Pager::AddReceivedMessageHandler);

	if (methodName == "SetProcessingMessagesHandler")
		return HANDLER_D(System::AddProcessingMessagesHandler);

	if (methodName == "EditProcessedMessagesHandler")
		return HANDLER_D(System::EditProcessedMessagesHandler);

	if (methodName == "SetPagersNumbersHandler")
		return HANDLER_D(ControlPanel::SetIds);

	return nullptr;
}


//Метод который делает новый такт
string System::TickStepHandler(string command)
{
	tick++;
	return "";
}

//Метод возвращающий текущий такт.
string System::GetCurrentTickHandler(string info)
{
	return to_string(tick);
}

//Метод, который получает сигнал о том, что оператор обработал сообщение
string System::EditProcessedMessagesHandler(string info)
{
	processedSignals++;
	processingSignals--;

	return "";
}

//Метод, который получает сигнал о том, что оператор получил сообщение в очередь
string System::AddProcessingMessagesHandler(string info)
{
	processingSignals++;

	return "";
}
