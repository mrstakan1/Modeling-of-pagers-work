#include "Base.h"

//Конструктор класса
Base::Base(Base* pHeadObject, string objectName)
{
	//инициализация полей
	this->objectName = objectName;
	this->pHeadObject = pHeadObject;

	if (pHeadObject != nullptr) // головной объект не нулевой указатель - добавляем новый в подчиненные текущего 
		pHeadObject->subordinateObjects.push_back(this);
}

//Метод возвращающий указатель на головной объект
Base* Base::GetHead()
{
	return pHeadObject;
}

//Метод устанавлищающий головной объект
void Base::SetHead(Base* pHead)
{
	pHeadObject = pHead;
}

//Метод устанавливающий имя объекта
void Base::SetName(string name)
{
	objectName = name;
}

//метод возвращающий состояние объекта
int Base::GetState()
{
	return objState;
}

//Метод установки состояния
void Base::SetState(int state)
{
	if (state == 0)
	{
		objState = state;
		//цикл нужен для того, что, если выключаем объект - выключаем всех подчиненных
		for (int i = 0; i < subordinateObjects.size(); i++)
		{
			subordinateObjects[i]->SetState(state);
		}
	}

	if (pHeadObject != nullptr && pHeadObject->objState != 0)
		objState = state;
}

//метод возвращающий имя объекта
string Base::GetName()
{
	return objectName;
}

//метод поиска объекта
Base* Base::FindObject(Base* currentObject, string objectPath)
{
	Base* foundObj = nullptr;

	if (objectPath.length() > 0 && objectPath[0] == '/') // если у объекта в начале /, то ищем по имени которое идет после /
	{
		objectPath.erase(0, 1);

		if (objectPath == "")
			return this;
		else
			return this->FindObject(currentObject, objectPath);
	}

	if (objectPath.length() > 0) // поиск объекта по относительной коориднате (path_1/path_2)
	{
		int slashPos = objectPath.find('/');
		string tempName = objectPath;

		if (slashPos != string::npos) // если не найден слеш, значит мы получили имя объекта, который мы ищем
		{
			tempName.erase(slashPos, objectPath.size());
			objectPath.erase(0, slashPos);
		}
		else
		{
			objectPath = "";
		}

		//цикл нужен для поиска объекта по имени среди подчиненых текущего, полученного в кач параметра
		for (int i = 0; i < currentObject->subordinateObjects.size(); i++) {
			if (currentObject->subordinateObjects[i]->GetName() == tempName) {
				foundObj = currentObject->subordinateObjects[i];
				if (objectPath != "") {
					return foundObj->FindObject(foundObj, objectPath);
				}
				return foundObj;
			}
		}
	}
	return foundObj;
}

//Метод установки связи
void Base::SetConnect(string signalName, TYPE_SIGNAL pSignal, Base* pTarget, TYPE_HANDLER pHandler)
{
	// Цикл для исключения повторного установления связи
	for (int i = 0; i < connections.size(); i++)
	{
		if (connections[i]->signalName == signalName)
			return;
	}

	Connection* bond = new Connection();                 // создание объекта структуры для хранения информации о новой связи
	bond->signalName = signalName;
	bond->pSignal = pSignal;
	bond->pTarget = pTarget;
	bond->pHandler = pHandler;

	connections.push_back(bond);          // добавление новой связи                 
}

//Метод выдачи сигнала
string Base::EmitSignal(string signalName, string command)
{
	string response = "null";

	if (objState == 0)
		return response;

	for (int i = 0; i < connections.size(); i++) // цикл по всем обработчикам
	{
		if (connections[i]->signalName == signalName)      // определение допустимого обработчика
		{
			if (connections[i]->pSignal != nullptr)
			{
				(this->*connections[i]->pSignal)(command);      // вызов метода сигнала
			}

			if (connections[i]->pHandler != nullptr)
			{
				response = (connections[i]->pTarget->*(connections[i]->pHandler))(command);      // вызов метода обработчика
			}

			break;
		}
	}

	return response;
}

//Метод вывода дерева иерархии
void Base::PrintTree(string tab)
{
	if (pHeadObject == nullptr)
		cout << endl << objectName << ((objState != 0) ? " is ready" : "is not ready");

	for (size_t i = 0; i < subordinateObjects.size(); i++)
	{
		cout << endl;
		cout << tab << subordinateObjects[i]->GetName() <<
			((subordinateObjects[i]->objState != 0) ? " is ready" : " is not ready");

		subordinateObjects[i]->PrintTree(tab + "\t");
	}
}