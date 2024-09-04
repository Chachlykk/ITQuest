#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server;
    try {
        server.startServer(5555, "127.0.0.1", "ITQuest", "postgres", "78789898");
    } catch (const std::exception& e) { // Замените "all" на конкретный тип исключения, если он известен
        qDebug() << e.what(); // Вызов функции what() возвращает строку с описанием исключения
    }
    return a.exec();
}
