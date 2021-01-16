#ifndef MULTISWITCH_H
#define MULTISWITCH_H

#include <QSharedMemory>
#include <QBitArray>

class MultiSwitch
{
public:
    MultiSwitch(const QString &ids, const QString &readed_ba);//  конструкторы для карты положений
    MultiSwitch();
    ~MultiSwitch();

    bool setPos(int pos); //  меняет позицию по аргументу

    void setArr(const QVector<QBitArray> arr) { this->m_arr = arr; } // задать готовую карту положений
    void setArr(const QBitArray arr, int pos) { this->m_arr[pos] = arr; } // задать положения выключателей для определенной позиции
    void setID(const QVector<QString> ids);
    void setID(const QString id, int position);
    //void initialize(const QString &ids, const QString &readed_ba); // инициализировать массив

    QVarLengthArray<QSharedMemory*>& getID() {return mem_interfaces; }

    //int getPosition() { return position;}
    int getPositions() { return m_arr.size(); } // количество позиций переключателя
    QVector<QBitArray> getArr() { return m_arr; } // возвращает карту положений
    QBitArray getArr(int pos) { return m_arr.at(pos); } // возвращает положения выключателей для определенной позиции

private:

    QVarLengthArray<QSharedMemory*> mem_interfaces;

    QVector<QBitArray> m_arr;

};

#endif // MULTISWITCH_H
