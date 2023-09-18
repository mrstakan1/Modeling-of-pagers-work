#include "Pager.h"

//Конструктор класса
Pager::Pager(Base* pHeadObject, string objectName, int id) : Base(pHeadObject, objectName)
{
	_iD = id;
}

//Метод возвращающий id пейджера
int Pager::GetId()
{
	return _iD;
}

//Метод выдающий информацию о пейджере сигналом
void Pager::PagerStatusSignal(string& signal)
{
	signal = "\nThe list of messages sent by the pager " + to_string(_iD);

	//цикл конструирующий информацию об отправленных сообщениях
	for (int i = 0; i < sentMessages.size(); i++)
	{
		signal += "\n" + to_string(sentMessages[i].tickMadeSent) + " "
			+ to_string(sentMessages[i].idTo) + " "
			+ sentMessages[i].message;
	}

	signal += "\nList of received messages by the pager " + to_string(_iD);

	//цикл конструирующий информацию об полученных сообщениях
	for (int i = 0; i < receivedMessages.size(); i++)
	{
		signal += "\n" + to_string(receivedMessages[i].tickMadeSent) + " "
			+ to_string(receivedMessages[i].idFrom) + " "
			+ receivedMessages[i].message;
	}

}

//метод возвращающий кол-во отправленных сообщений пейджером
int Pager::GetSentMessagesCount()
{
	return sentMessages.size();
}

//метод возвращающий кол-во полученных сообщений пейджером
int Pager::GetReceivedMessagesCount()
{
	return receivedMessages.size();
}

//метод устанавливающий состояние пейджера
string Pager::SetPagerStateHandler(string state)
{
	if (state == "busy")
		isBusy = true;

	if (state == "is not busy")
		isBusy = false;

	return "";
}

//метод возвращающий состояние пейджера
string Pager::GetPagerState(string state)
{
	return (isBusy == true) ? "busy" : "is not busy";
}


//метод который сохраняет полученные сообщения
string Pager::AddReceivedMessageHandler(string message)
{
	Message receivedMessage;
	string tempMessage = message;

	//получаем id отправителя
	receivedMessage.idFrom = stoi(tempMessage.substr(0, tempMessage.find(" ") + 1));
	tempMessage.erase(0, tempMessage.find(" ") + 1);

	//получаем id получателя
	receivedMessage.idTo = stoi(tempMessage.substr(0, tempMessage.find(" ") + 1));
	tempMessage = tempMessage.erase(0, tempMessage.find(" ") + 1);

	//получаем такт отправления
	receivedMessage.tickMadeSent = stoi(tempMessage.substr(0, tempMessage.find(" ") + 1));
	tempMessage.erase(0, tempMessage.find(" ") + 1);

	//получаем само сообщение
	receivedMessage.message = tempMessage;

	receivedMessages.push_back(receivedMessage);

	SortMessages(); // вызываем метод сортировки сообщений
	return "";
}

//метод который сохраняет отправленные сообщения и отправляет сигнал с сообщением
void Pager::SendMessageSignal(string& message)
{
	Message sentMessage;
	string tempMessage = message;

	//получаем id отправителя
	sentMessage.idFrom = stoi(tempMessage.substr(0, tempMessage.find(" ") + 1));
	tempMessage.erase(0, tempMessage.find(" ") + 1);

	//получаем id получателя
	sentMessage.idTo = stoi(tempMessage.substr(0, tempMessage.find(" ") + 1));
	tempMessage = tempMessage.erase(0, tempMessage.find(" ") + 1);

	//получаем id такт отправления
	sentMessage.tickMadeSent = stoi(tempMessage.substr(0, tempMessage.find(" ") + 1));
	tempMessage.erase(0, tempMessage.find(" ") + 1);

	//получаем сообщение
	sentMessage.message = tempMessage;

	sentMessages.push_back(sentMessage);

	SortMessages(); // вызываем метод сортировки сообщений
}

//метод сортировки сообщений
void Pager::SortMessages()
{
	if (sentMessages.size() > 1) // если отправленных сообщений больше 1, запускаем цикл для пузырьковой сортировки
	{
		for (int i = 0; i < sentMessages.size(); i++)
		{
			for (int j = 0; j < sentMessages.size() - (i + 1); j++)
				if (sentMessages[j].idFrom > sentMessages[j + 1].idFrom)
					swap(sentMessages[j], sentMessages[j + 1]);
		}
	}

	if (receivedMessages.size() > 1) // если полученных сообщений больше 1, запускаем цикл для пузырьковой сортировки
	{
		for (int i = 0; i < receivedMessages.size(); i++)
		{
			for (int j = 0; j < receivedMessages.size() - (i + 1); j++)
				if (receivedMessages[j].idFrom > receivedMessages[j + 1].idFrom)
					swap(receivedMessages[j], receivedMessages[j + 1]);
		}
	}
}
