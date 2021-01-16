#include "multiswitch.h"
#include <QList>
#include <QVector>
#include <QDebug>
#include <QSharedMemory>


MultiSwitch::MultiSwitch(const QString &ids, const QString &readed_ba)
{
    /*
     * На вход поступают два qstring
     *  ids - ID выключателей через запятую, без пробелов.
     *  readed_ba - прочитанная карта положений, позиции разделены запятой без пробелов (bool array)
     * парсим данные и распихиваем по контейнерам
     */

    QStringList id = ids.split(QLatin1Char(',')); //пользуясь split() текстовую запись массива поместим в список
    QStringList::const_iterator m;
    for (m = id.begin(); m != id.end(); ++m) // пройдемся по списку
    {
        QSharedMemory *sh = new QSharedMemory(*m); // создаем в куче обект класса для работы с общей памятью
        mem_interfaces.append(sh); // перемещаем указатель в массив
    }

    QStringList split_sections = readed_ba.split(QLatin1Char(',')); // предварительно текстовую запись карты положений разбиваем на состояния выключателей

    QStringList::const_iterator i;
    for (i = split_sections.begin(); i != split_sections.end(); ++i) // ходим по списку
    {
        QBitArray bits(mem_interfaces.size()); // массив bool, состояния переключателей на определенной позиции

        if(mem_interfaces.size() == i->size()) // если количество значений равно количеству классов для доступа к ним, то можно продолжать
        {
            for (int b = 0; b < mem_interfaces.size(); ++b)
            {
                QChar digit = i->at(b);
                bits.setBit(b, digit == '1');
                mem_interfaces[b]->attach();
            }
        }
        m_arr.append(bits);
    }
    mem_interfaces.squeeze(); //освобождаем зарезервированную память
}
MultiSwitch::MultiSwitch()
{
    QSharedMemory *sh = new QSharedMemory("0");
    mem_interfaces.append(sh);
    QBitArray bits(1);
    m_arr.append(bits);
    mem_interfaces.squeeze();
}

MultiSwitch::~MultiSwitch(){}

bool MultiSwitch::setPos(int pos)
{
    if (pos >= m_arr.size() || pos < 0)
        return false;
    for (int i = 0; i < mem_interfaces.size(); i++)
    {
        if (!mem_interfaces[i]->lock())
            return false;

        bool *to = (bool*)mem_interfaces[i]->data();
        *to = m_arr.at(pos).at(i);
        mem_interfaces[i]->unlock();
    }
    return true;
}
void MultiSwitch::setID(const QString id, int position)
{
    if(mem_interfaces[position]->isAttached())
        mem_interfaces[position]->detach();
    mem_interfaces[position]->setKey(id);
    mem_interfaces[position]->attach();
}

void MultiSwitch::setID(const QVector<QString> ids)
{
    for (int i = 0; i < ids.size(); ++i)
    {
        setID(ids.at(i), i);
    }
}


