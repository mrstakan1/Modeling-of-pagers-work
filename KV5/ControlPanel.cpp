#include "ControlPanel.h"

ControlPanel::ControlPanel(Base* pHeadObject, string objectName) : Base(pHeadObject, objectName) {};

string ControlPanel::CommandProcessingHandler(string command)
{
	//Получаем указатели на объекты дерева
	Base* Controller = this;
	Base* Reader = FindObject(Controller->GetHead(), "/Reader");
	Base* System = Controller->GetHead();
	string reply;
	int currentTick = stoi(Controller->EmitSignal("GetCurrentTick"));


	if (command == "Turn off the system") // если команда Turn off the system - выключаем систему
	{
		Controller->EmitSignal("PrintOut", "\nTurn off the ATM");
		return command;
	}

	//Создаем копию команды, полученной параметром
	string commandClone = command;
	string tempCommand;
	tempCommand = commandClone.substr(0, 15);

	if (tempCommand == "Send a message ") // если команда Send a message - пробуем отправить сообщение оператору
	{
		if (IsLastCommandWasSend == false) // если до этого была команда не send a message - значит наступил новый такт и выполняется работа элементов системы
		{
			Controller->EmitSignal("TickStep");
			ProceedTypingList();
			ProceedQueue();
		}
		Base* Controller = this;

		//Устанавливаем, что последний
		IsLastCommandWasSend = true;
		string tempCommand = commandClone.substr(15);

		TrySendAMessage(tempCommand);
	}
	else
	{
		//устанавливаем полю, следящим за тем, была ли последняя команда = send a message значение false
		IsLastCommandWasSend = false;
	}


	// Новый такт если у нас не повторое send a message
	if (IsLastCommandWasSend == false)
	{
		Controller->EmitSignal("TickStep");
		currentTick = stoi(Controller->EmitSignal("GetCurrentTick"));

		//=================================
		if (command == "Display the system status")
		{
			System->EmitSignal("GetSystemStatus"); // вызов сигнала который выводит на экран информацию о системе
		}

		//=================================
		if (command.substr(0, 25) == "Display the pager status ") // если команда Display the pager status - вызов сигнала который выводи информацию о нужном пейджере, если он не в наборе
		{
			int pagerId = stoi(command.substr(25));
			Base* pPager = FindObject(this->GetHead(), "/Reader/Pager_" + to_string(pagerId));
			bool isPagerBusy = (Controller->EmitSignal("GetPagerState_" + to_string(pagerId)) == "busy" ? true : false);

			if (isPagerBusy == false)
				pPager->EmitSignal("PagerStatus");
		}

		//Вызов методов, которые выполняют работу элементов системы
		ProceedTypingList();
		ProceedQueue();
	}

	return "";
}

//Метод обрабатывающий очередь сообщений оператора
void ControlPanel::ProceedQueue()
{
	//получаем указатели на нужные объекты системы и получаем информацию о текущем такте
	Base* Controller = this;
	Base* Reader = FindObject(this->GetHead(), "Reader");
	int currentTick = stoi(Controller->EmitSignal("GetCurrentTick"));


	//если очередь не пустая - то отправляем сообщение
	if (messagesQueue.empty() == false)
	{
		Message message = messagesQueue.back();

		if (message.createTick == currentTick) // если сообщение начало набор в этом такте - он еще не закончился
			return;

		//конструируем сообщение обратно из полей структуры сообщения
		string newMessage = to_string(message.idFrom) + " " + to_string(message.idTo) +
			" " + to_string(message.tickMadeSent) + " " + message.message;

		if (CheckNumber(message.idTo) == true) // проверяем, если получатель существует - отправляем ему сообщение
		{
			this->EmitSignal("EditProcessedMessages");
			Reader->EmitSignal("ReceiveMessage_" + to_string(message.idTo), newMessage);
		}

		messagesQueue.pop_back();
	}
}

//Метод который конструирует многопользовательский ввод сообщений пользователями
void ControlPanel::ProceedTypingList()
{
	//получаем указатели на нужные объекты системы и значение текущего такта
	Base* Controller = this;
	int currentTick = stoi(Controller->EmitSignal("GetCurrentTick"));


	//запуск цикла для перебора всех сообщений которые находятся в наборе
	for (auto it = typingMessages.rbegin(); it != typingMessages.rend(); )
	{
		if (it->ticksLeft == 1) // сообщение наберется в этом такте
		{
			//вызов методов которые проверяют существование пейджеров отправителя и получателя - если нет, будет выведет соответстующий сигнал
			CheckNumberWithSignal(it->idFrom);
			CheckNumberWithSignal(it->idTo);

			//устанавливаем такт, в который будет отправлено сообщение
			it->tickMadeSent = currentTick + messagesQueue.size();
			it->createTick = currentTick;

			//получаем указатели на объекты нужных пейджеров
			Base* senderPager = FindObject(this->GetHead(), "/Reader/Pager_" + to_string(it->idFrom));
			Base* receiverPager = FindObject(this->GetHead(), "/Reader/Pager_" + to_string(it->idTo));

			//конструируем сообщение обратно из полей структуры сообщения
			string newMessage = to_string(it->idFrom) + " " + to_string(it->idTo) +
				" " + to_string(it->tickMadeSent) + " " + it->message;

			messagesQueue.push_back(*it);

			//Вызов сигналов которые убирают статус "занят" у пейджера и отправка сообщения пейджером
			senderPager->EmitSignal("SetPagerState_" + to_string(it->idFrom), "is not busy");
			senderPager->EmitSignal("SendMessage_" + to_string(it->idFrom), newMessage);

			//если получатель существует, то оператор начал обрабатывать это сообщение
			if (CheckNumber(it->idTo) == true)
				Controller->EmitSignal("SetProcessingMessages");

			it = decltype(it)(typingMessages.erase(next(it).base())); // удаляем текущее сообщние набора из списка набираемых сообщений
		}
		else
		{
			it->ticksLeft--; // прошел такт, пользователь набрал еще 10 символов сообщения
			it++;
		}
	}
}

//метод для создания списка номеров пейджеров, список получен сигналом от системы
string ControlPanel::SetIds(string numbers)
{
	//цикл нужен для получения номеров пейджеров
	while (numbers.find(" ") != string::npos)
	{
		//к списку пейджеров добавляем новый номер
		iDs.push_back(stoi(numbers.substr(0, numbers.find(" ") + 1)));
		numbers.erase(0, numbers.find(" ") + 1);
	}

	return "";
}

//метод проверяющий существование номера пейджера и выдающий соответствующий сигнал
bool ControlPanel::CheckNumberWithSignal(int number)
{
	//цикл нужен для прохода по всем номерам пейджеров
	for (int i = 0; i < iDs.size(); i++)
	{
		//если пейджер существует, возвращаем значение истина
		if (number == iDs[i])
			return true;
	}

	//Вызов сигнала вывода на экран с информацией о том, что пейджер не существует
	this->EmitSignal("PrintOut", "\nSubscriber " + to_string(number) + " not found");
	return false;
}

//метод проверяющий существование номера пейджера и выдающий соответствующий сигнал
bool ControlPanel::CheckNumber(int number)
{
	//цикл нужен для прохода по всем номерам пейджеров
	for (int i = 0; i < iDs.size(); i++)
	{

		if (number == iDs[i]) //если пейджер существует, возвращаем значение истина
			return true;
	}


	return false;
}

//Метод отправки сообщения пейджером
void ControlPanel::TrySendAMessage(string message)
{
	Base* Controller = this;
	string tempCommand = message;

	bool isSenderExist, isReceieverExist;

	//---------------------------Разбиение сообщения на части
	int senderId = stoi(tempCommand.substr(0, tempCommand.find(" ") + 1));
	tempCommand = tempCommand.erase(0, tempCommand.find(" ") + 1);

	int receiverId = stoi(tempCommand.substr(0, tempCommand.find(" ") + 1));
	tempCommand = tempCommand.erase(0, tempCommand.find(" ") + 1);
	string messageText = tempCommand;
	//-----------------------------

	//Получаем указатели на нужные пейджеры
	Base* senderPager = FindObject(this->GetHead(), "/Reader/Pager_" + to_string(senderId));
	Base* receiverPager = FindObject(this->GetHead(), "/Reader/Pager_" + to_string(receiverId));

	//Проверяем занят ли пейджер для отправки сообщения
	bool isSenderPagerBusy = (Controller->EmitSignal("GetPagerState_" + to_string(senderId)) == "busy" ? true : false);


	if (isSenderPagerBusy == false) // если пейджер не в наборе, отправляем сообщение
	{
		if (messageText.size() >= 1 && messageText.size() <= 30) // если сообщение допустимого размера
		{
			Message newMessage;

			//структурируем сообщение в структуру Message
			newMessage.message = messageText;
			newMessage.idFrom = senderId;
			newMessage.idTo = receiverId;
			int ticksLeft = messageText.size() / 10 + 1;
			newMessage.ticksLeft = ticksLeft;

			//Получаем указатель на объект пейджера, который отправляет сообщение
			Base* pPager = FindObject(this->GetHead(), "/Reader/Pager_" + to_string(senderId));

			if (messageText.size() > 10) // если сообщение длинной больше 10, то пейджер занят для набора
			{
				pPager->EmitSignal("SetPagerState_" + to_string(senderId), "busy");
			}

			//Отправляем данное сообщение в "набор"
			typingMessages.push_back(newMessage);
		}
	}

	//попытка отправить сообщение
	//TrySendAMessage(commandClone);
}