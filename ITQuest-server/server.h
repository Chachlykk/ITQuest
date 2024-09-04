#ifndef SERVER_H
#define SERVER_H

#include <QBuffer>
#include <QDebug>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtSql>
#include <algorithm>
#include <vector>

enum DataType { JSONData, PupilsData, PupilsRate, QuestionsData };
const int answersPos[] = {180, 900, 1610};

struct pupilClass
{
    QString name;
    int id;
};

class Server: public QTcpServer
{
    Q_OBJECT
public:
    Server(){}
    ~Server();

    QTcpSocket* socket;
    QByteArray Data;
    void sendToClient(QString message);
    void sendToClientOther(QByteArray data, DataType dataType);

public slots:
    void startServer(
        int host, QString hostName, QString databaseName, QString username, QString password);
    void incomingConnection(qintptr  socketDescriptor) override;
    void sockReady();
    void sockDisc();
private:
    QSqlDatabase db;
    int createTeacherId(QJsonObject &jsonObject);
    QString getTeacherPassword(int id);
    bool ifClassExist(QJsonObject &jsonObject);
    int createPupilId(QJsonObject &jsonObject);
    void sendQuestionToClient(QJsonObject &jsonObject);
    int createClassId(QJsonObject &jsonObject);
    void deleteClass(QJsonObject &jsonObject);
    void addLevel(QJsonObject &jsonObject);
    void sendClasses(int id);
    void sendPupils(int classId);
    void sendLevels(int classId);
    void setPupilResult(QJsonObject &jsonObject);
    void checkPupilLogin(QJsonObject &jsonObject);
    void sendPupilPage(QJsonObject &jsonObject);
    void sendPupilRate(QJsonObject &jsonObject);
    void addPupilQuestion(QJsonObject &jsonObject);
    void sendPupilsQuestions(QJsonObject &jsonObject);
    QStringList getListFromDB(QString str);
    int findFirstZeroIndex(const QStringList &levelsResult);
};

#endif // SERVER_H
