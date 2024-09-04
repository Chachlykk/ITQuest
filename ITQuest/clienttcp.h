#ifndef CLIENTTCP_H
#define CLIENTTCP_H
#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include "global_vars.h"

enum DataType { JSONData, PupilsData, PupilsRate, QuestionsData };
class ClientTCP: public QObject
{
    Q_OBJECT
public:
    /**
 * @brief Constructor for the ClientTCP class.
 * @param parent The parent object (optional).
 */
    explicit ClientTCP(QObject *parent = nullptr);

    /**
 * @brief Destructor for the ClientTCP class.
 * Cleans up resources by deleting the socket.
 */
    ~ClientTCP();

    /**
 * @brief Starts the connection to the default server.
 * Connects to the server at 127.0.0.1 on port 5555.
 */
    void startConnection();
    QTcpSocket *socket; ///< The TCP socket used for communication.
    QByteArray Data;    ///< Buffer for storing data received from the server.

    /**
 * @brief Sends a message to the server.
 *
 * This method converts the given message to a UTF-8 encoded QByteArray, calculates the size of the data,
 * and sends the data to the server in the following format:
 * 1. The data type (as JSONData).
 * 2. The size of the data.
 * 3. The actual message data.
 *
 * @param message The message to be sent to the server as a QString.
 */
    void sendToServer(QString message);

    /**
 * @brief Sends arbitrary data to the server with a specified data type.
 *
 * This method calculates the size of the given data, and sends the data to the server 
 * in the following format:
 * 1. The data type (specified by the dataType parameter).
 * 2. The size of the data.
 * 3. The actual data.
 *
 * @param data The data to be sent to the server as a QByteArray.
 * @param dataType The type of data being sent, specified by the DataType enum.
 */
    void sendToServerOther(QByteArray data, DataType dataType);
public slots:
    /**
 * @brief Slot to handle the readyRead signal from the QTcpSocket.
 *
 * This method reads the data type from the socket and processes the incoming data
 * accordingly. Depending on the data type, it parses JSON data or other binary data 
 * related to pupils, pupil rates, or questions, and emits appropriate signals with the 
 * parsed information.
 * 
 * The method supports the following data types:
 * - JSONData: Parses JSON data and emits various signals based on the "signal" field in the JSON object.
 * - PupilsData: Parses pupil data and emits the showPupils signal.
 * - PupilsRate: Parses pupil rate data and emits the showPupilsRate signal.
 * - QuestionsData: Parses question data and emits the showPupilQuestions signal.
 *
 * The method also handles the case where there is still data available in the socket buffer 
 * and recursively calls itself to process the remaining data.
 */
    void sockReady();

    /**
 * @brief Slot to handle the disconnected signal from the QTcpSocket.
 *
 * This method is called when the socket is disconnected. It deletes the socket object
 * by scheduling it for deletion with deleteLater(), ensuring that it is safely removed 
 * once all pending events have been processed.
 */
    void sockDisc();

signals:
    /**
     * @brief Signal emitted when a teacher registration ID is received.
     * @param id The teacher registration ID.
     */
    void registrTeacherID(int id);

    /**
     * @brief Signal emitted when a teacher login status is received.
     * @param status The login status.
     * @param id The teacher ID.
     */
    void loginCorrectTeacher(QString status, int id);

    /**
     * @brief Signal emitted when a pupil login status is received.
     * @param status The login status.
     * @param id The pupil ID.
     */
    void loginCorrectPupil(QString status, int id);

    /**
     * @brief Signal emitted when a pupil registration ID is received.
     * @param data The JSON document containing the pupil registration data.
     */
    void registrPupilID(QJsonDocument &data);

    /**
     * @brief Signal emitted when a level is set.
     * @param data The JSON document containing the level data.
     */
    void setLevel(QJsonDocument &data);

    /**
     * @brief Signal emitted when a class is added.
     * @param data The JSON document containing the class data.
     */
    void addClass(QJsonDocument &data);

    /**
     * @brief Signal emitted to show classes.
     * @param data The JSON object containing the classes data.
     */
    void showClasses(QJsonObject &data);

    /**
     * @brief Signal emitted to show levels.
     * @param data The JSON object containing the levels data.
     */
    void showLevels(QJsonObject &data);

    /**
     * @brief Signal emitted to show pupils.
     * @param pupils The vector containing the pupils data.
     */
    void showPupils(QVector<Pupil> &pupils);

    /**
     * @brief Signal emitted to show pupil questions.
     * @param questions The vector containing the questions data.
     * @param className The name of the class.
     */
    void showPupilQuestions(QVector<Question> &questions, QString className);

    /**
     * @brief Signal emitted when a level does not exist.
     */
    void levelNotExist();

    /**
     * @brief Signal emitted to get pupils page.
     * @param data The JSON object containing the pupils page data.
     */
    void getPupilsPage(QJsonObject &data);

    /**
     * @brief Signal emitted to show pupils rate.
     * @param pupils The vector containing the pupils rate data.
     */
    void showPupilsRate(QVector<Pupil> &pupils);

    /**
     * @brief Signal emitted to show pupils rate.
     * @param pupils The vector containing the pupils rate data.
     */
    void sentQuestionStatus(bool status);

private:
    int size;                         ///< Size of the data.
    int currentSize;                  ///< Current size of the data being processed.
    QJsonDocument jsonDocumentGlobal; ///< Global JSON document.
};

#endif // CLIENTTCP_H
