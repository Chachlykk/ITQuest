#include "clienttcp.h"

ClientTCP::ClientTCP(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));
}

ClientTCP::~ClientTCP()
{
    delete socket;
}

void ClientTCP::startConnection()
{
    socket->connectToHost("127.0.0.1",5555);
}
void ClientTCP::sendToServer(QString message)
{
    QByteArray byteArray = message.toUtf8();
    long int dataSize = byteArray.size();
    DataType dataType = JSONData;
    socket->write(reinterpret_cast<const char *>(&dataType), sizeof(dataType));
    socket->write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));
    socket->write(byteArray);
}

void ClientTCP::sendToServerOther(QByteArray data, DataType dataType)
{
    long int dataSize = data.size();
    socket->write(reinterpret_cast<const char *>(&dataType), sizeof(dataType));
    socket->write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));
    socket->write(data);
}

void ClientTCP::sockReady()
{
    int dataType;
    socket->read(reinterpret_cast<char *>(&dataType), sizeof(DataType));
    if (dataType == JSONData) {
        long int dataSize;
        socket->read(reinterpret_cast<char *>(&dataSize), sizeof(dataSize));
        QByteArray data = socket->read(dataSize);
        // Парсинг QByteArray в QJsonDocument
        QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
        // Преобразование QJsonDocument в QJsonObject
        QJsonObject jsonObject = jsonDocument.object();
        QString signal = jsonObject["signal"].toString();
        if (signal == "registrTeacherID")
            emit registrTeacherID(jsonObject["id"].toInt());
        else if (signal == "loginTeacher") {
            emit loginCorrectTeacher(jsonObject["status"].toString(), jsonObject["id"].toInt());
        } else if (signal == "loginPupil") {
            emit loginCorrectPupil(jsonObject["status"].toString(), jsonObject["id"].toInt());
        } else if (signal == "registrPupil")
            emit registrPupilID(jsonDocument);
        else if (signal == "setQuestion") {
                emit setLevel(jsonDocument);
        } else if (signal == "addClass")
            //toClient="{\"signal\":\"addClass\",\"classCode\":"+id+",\"className\":"+jsonObject["className"]+"\"}";
            emit addClass(jsonDocument);
        else if (signal == "getClasses") {
            jsonObject.remove("signal");
            emit showClasses(jsonObject);
        } else if (signal == "getLevels") {
            jsonObject.remove("signal");
            emit showLevels(jsonObject);
        } else if (signal == "getPupilsPage")
            emit getPupilsPage(jsonObject);
        else if (signal == "questionOk")
            emit sentQuestionStatus(jsonObject["status"].toBool());
        if (socket->bytesAvailable() > 0)
            emit socket->readyRead();

    } else if (dataType == PupilsData) {
        long int dataSize;
        socket->read(reinterpret_cast<char *>(&dataSize), sizeof(dataSize));
        // QByteArray data = socket->read(dataSize);
        currentSize = 0;
        QList<Pupil> pupils;
        while (currentSize < dataSize) {
            Pupil pupil;
            QByteArray tmpBytes = socket->read(sizeof(int));
            int id = *reinterpret_cast<const int *>(tmpBytes.constData());
            currentSize += sizeof(int) * 3;

            tmpBytes = socket->read(sizeof(int));
            int tmpSize = *reinterpret_cast<const int *>(tmpBytes.constData());
            tmpBytes = socket->read(tmpSize);
            currentSize += tmpSize;
            QString name = QString::fromUtf8(tmpBytes.constData(), tmpBytes.size());

            tmpBytes = socket->read(sizeof(int));
            tmpSize = *reinterpret_cast<const int *>(tmpBytes.constData());
            QVector<int> results;
            for (int i = 0; i < tmpSize; i++) {
                tmpBytes = socket->read(sizeof(int));
                currentSize += sizeof(int);
                int resultItem = *reinterpret_cast<const int *>(tmpBytes.constData());
                results.append(resultItem);
            }

            pupil.id = id;
            pupil.name = name;
            pupil.results = results;
            pupils.append(pupil);
        }
        emit showPupils(pupils);
        if (socket->bytesAvailable() > 0)
            emit socket->readyRead();
    } else if (dataType == PupilsRate) {
        long int dataSize;
        if (socket->bytesAvailable() > 0)
            socket->read(reinterpret_cast<char *>(&dataSize), sizeof(dataSize));
        // QByteArray data = socket->read(dataSize);
        currentSize = 0;
        QList<Pupil> pupils;
        bool res = currentSize < dataSize;
        while (currentSize < dataSize) {
            Pupil pupil;
            QByteArray tmpBytes = socket->read(sizeof(int));
            currentSize += sizeof(int);
            int tmpSize = *reinterpret_cast<const int *>(tmpBytes.constData());
            tmpBytes = socket->read(tmpSize);
            currentSize += tmpSize;
            QString name = QString::fromUtf8(tmpBytes.constData(), tmpBytes.size());

            tmpBytes = socket->read(sizeof(int));
            currentSize += sizeof(int);
            tmpSize = *reinterpret_cast<const int *>(tmpBytes.constData());
            QVector<int> results;
            for (int i = 0; i < tmpSize; i++) {
                tmpBytes = socket->read(sizeof(int));
                currentSize += sizeof(int);
                int resultItem = *reinterpret_cast<const int *>(tmpBytes.constData());
                results.append(resultItem);
            }

            pupil.name = name;
            pupil.results = results;
            pupils.append(pupil);
        }
        emit showPupilsRate(pupils);
        if (socket->bytesAvailable() > 0)
            emit socket->readyRead();
    } else if (dataType == QuestionsData) {
        long int dataSize;
        socket->read(reinterpret_cast<char *>(&dataSize), sizeof(dataSize));
        // QByteArray data = socket->read(dataSize);
        currentSize = 0;

        QByteArray tmpBytes = socket->read(sizeof(int));
        currentSize += sizeof(int);
        int tmpSize = *reinterpret_cast<const int *>(tmpBytes.constData());
        tmpBytes = socket->read(tmpSize);
        currentSize += tmpSize;
        QString className = QString::fromUtf8(tmpBytes.constData(), tmpBytes.size());

        QList<Question> questions;
        while (currentSize < dataSize) {
            Question question;
            QByteArray tmpBytes;
            int tmpSize;

            tmpBytes = socket->read(sizeof(int));
            currentSize += sizeof(int);
            tmpSize = *reinterpret_cast<const int *>(tmpBytes.constData());
            tmpBytes = socket->read(tmpSize);
            currentSize += tmpSize;
            QString name = QString::fromUtf8(tmpBytes.constData(), tmpBytes.size());

            tmpBytes = socket->read(sizeof(int));
            currentSize += sizeof(int);
            tmpSize = *reinterpret_cast<const int *>(tmpBytes.constData());
            tmpBytes = socket->read(tmpSize);
            currentSize += tmpSize;
            QString quest = QString::fromUtf8(tmpBytes.constData(), tmpBytes.size());

            tmpBytes = socket->read(sizeof(int));
            currentSize += sizeof(int);
            tmpSize = *reinterpret_cast<const int *>(tmpBytes.constData());
            tmpBytes = socket->read(tmpSize);
            currentSize += tmpSize;
            QString date = QString::fromUtf8(tmpBytes.constData(), tmpBytes.size());

            question.name = name;
            question.question = quest;
            question.date = date;
            questions.append(question);
        }
        emit showPupilQuestions(questions, className);
        if (socket->bytesAvailable() > 0)
            emit socket->readyRead();
    }
}

void ClientTCP::sockDisc()
{
    socket->deleteLater();
}
