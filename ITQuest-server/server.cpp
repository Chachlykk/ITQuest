#include "server.h"

void Server::startServer(
    int host, QString hostName, QString databaseName, QString username, QString password)
{
    /**
 * @brief Starts the server and connects to the specified PostgreSQL database.
 *
 * This method starts the server on any available IP address using the specified host port.
 * If the server starts successfully, it outputs "Listening...". Otherwise, it outputs "Not listening...".
 * Then, the method connects to a PostgreSQL database with the provided connection parameters:
 * - Host name
 * - Database name
 * - Username
 * - Password
 * If the database connection fails, an error message is outputted.
 *
 * @param host The port number on which the server will listen.
 * @param hostName The host name or IP address of the PostgreSQL server.
 * @param databaseName The name of the PostgreSQL database to connect to.
 * @param username The username used to authenticate with the PostgreSQL server.
 * @param password The password used to authenticate with the PostgreSQL server.
 *
 * Example usage:
 * @code
 * Server server;
 * server.startServer(5555, "127.0.0.1", "ITQuest", "postgres", "78789898");
 * @endcode
 */
    if (this->listen(QHostAddress::Any, host))
        qDebug() << "Listening...";
    else
        qDebug()<<"Not listening...";
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(hostName);
    db.setDatabaseName(databaseName);
    db.setUserName(username);
    db.setPassword(password);
    if (!db.open())
        qDebug() << "Error while openning database:" << db.lastError().text();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    /**
 * @brief Handles incoming client connections.
 *
 * This method is automatically called by Qt when a new client connects to the server.
 * It creates a new QTcpSocket for the incoming connection using the provided socket descriptor.
 * Signals for data readiness (`readyRead`) and disconnection (`disconnected`) are connected
 * to respective slots (`sockReady` and `sockDisc`) in the Server class.
 * Additionally, a debug message indicating the client's connection via the socket descriptor is outputted.
 *
 * @param socketDescriptor The socket descriptor associated with the incoming connection.
 *
 * Example usage:
 * Typically, this method is not directly called by user code, but is automatically invoked
 * when a client connects to the server.
 *
 * @see Server::sockReady
 * @see Server::sockDisc
 */
    socket=new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket,&QTcpSocket::readyRead,this,&Server::sockReady);
    connect(socket,&QTcpSocket::disconnected,this,&Server::sockDisc);
    qDebug()<<"client connected: "<<socketDescriptor;
}

void Server::sockReady()
{
    /**
 * @brief Handles incoming data from the client socket.
 *
 * This method is connected to the `readyRead` signal of a QTcpSocket instance.
 * It reads the incoming data from the socket and processes it based on the specified data type.
 * If the data type is JSONData, it parses the JSON data received from the client,
 * performs various operations based on the received signals, and sends responses back to the client.
 * Supported signals include registering and logging in teachers and pupils, managing classes and levels,
 * retrieving questions, and handling various queries related to pupils.
 * If the data type is PupilsData, it indicates a different type of data handling.
 * After processing the data, if there are remaining bytes available in the socket,
 * the method emits the `readyRead` signal to continue reading.
 *
 * @see sendToClient
 * @see sendQuestionToClient
 * @see sendClasses
 * @see sendPupils
 * @see sendLevels
 * @see setPupilResult
 * @see checkPupilLogin
 * @see sendPupilPage
 * @see sendPupilRate
 * @see addPupilQuestion
 * @see sendPupilsQuestions
 *
 * Example usage:
 * Typically, this method is connected to the `readyRead` signal of a QTcpSocket instance
 * to handle incoming data from clients in a server application.
 */
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
        QString toClient;
        if (signal == "registrTeacher") {
            QString id = QString::number(createTeacherId(jsonObject));

            QString toClient = "{\"signal\":\"registrTeacherID\",\"id\":" + id + "}";
            sendToClient(toClient);

        } else if (signal == "loginTeacher") {
            QString password = getTeacherPassword(jsonObject["id"].toInt());

            if (jsonObject["password"].toString() == password)
                toClient = "{\"signal\":\"loginTeacher\",\"status\":\"y\",\"id\":"
                           + QString::number(jsonObject["id"].toInt()) + "}";
            else
                toClient = "{\"signal\":\"loginTeacher\",\"status\":\"n\"}";
            sendToClient(toClient);
        } else if (signal == "registrPupil") {
            bool found = ifClassExist(jsonObject);

            if (found) {
                int id = createPupilId(jsonObject);
                toClient = "{\"signal\":\"registrPupil\",\"status\":\"y\",\"id\":"
                           + QString::number(id) + "}";
            } else
                toClient = "{\"signal\":\"registrPupil\",\"status\":\"n\"}";
            sendToClient(toClient);
        } else if (signal == "getQuestion") {
            sendQuestionToClient(jsonObject);
        } else if (signal == "addClass") {
            QString id = QString::number(createClassId(jsonObject));

            toClient = "{\"signal\":\"addClass\",\"classCode\":" + id + ",\"className\":\""
                       + jsonObject["className"].toString() + "\"}";
            sendToClient(toClient);
        } else if (signal == "deleteClass") {
            deleteClass(jsonObject);
        } else if (signal == "addLevel") {
            addLevel(jsonObject);
        } else if (signal == "getClasses") {
            sendClasses(jsonObject["teacherId"].toInt());
        } else if (signal == "getPupils") {
            sendPupils(jsonObject["classId"].toInt());
        } else if (signal == "getLevels") {
            sendLevels(jsonObject["classId"].toInt());
        } else if (signal == "levelEnd") {
            setPupilResult(jsonObject);
        } else if (signal == "loginPupil") {
            checkPupilLogin(jsonObject);
        } else if (signal == "getPupilsPage") {
            sendPupilPage(jsonObject);
        } else if (signal == "getPupilsRate") {
            sendPupilRate(jsonObject);
        } else if (signal == "askQuestion") {
            addPupilQuestion(jsonObject);
        } else if (signal == "getPupilsQuestions") {
            sendPupilsQuestions(jsonObject);
        } else if (signal == "deleteQuestions") {
            QSqlQuery query;
            query.prepare("DELETE FROM questions WHERE class_id = :classID");
            query.bindValue(":classID", jsonObject["classId"].toInt());
            if (!query.exec())
                qDebug() << query.lastError().text();
        }

        if (socket->bytesAvailable() > 0)
            emit socket->readyRead();
    } else if (dataType == PupilsData) {
        qDebug() << "PupilsData";
        if (socket->bytesAvailable() > 0)
            emit socket->readyRead();
    }
}

void Server::sendToClient(QString message)
{
    /**
 * @brief Sends a JSON message to the connected client.
 *
 * This method prepares and sends a JSON message to the client connected to the server via TCP socket.
 * The message is converted to a QByteArray using UTF-8 encoding.
 * The size of the message and its data type (JSONData) are sent before the actual message content.
 *
 * @param message The JSON message to send to the client.
 *
 * Example usage:
 * @code
 * QString response = "{\"signal\":\"registrTeacherID\",\"id\":123}";
 * server.sendToClient(response);
 * @endcode
 *
 * @see Server::sockReady
 *
 * @note This method assumes that a client is currently connected and the socket (`socket`) is valid.
 *       Ensure proper error handling and socket management to prevent crashes or undefined behavior.
 */

    QByteArray byteArray = message.toUtf8();
    long int dataSize = byteArray.size();
    DataType dataType = JSONData;
    socket->write(reinterpret_cast<const char *>(&dataType), sizeof(dataType));
    socket->write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));
    socket->write(byteArray);
}

void Server::sendToClientOther(QByteArray data, DataType dataType)
{
    /**
 * @brief Sends data of a specified type to the connected client.
 *
 * This method prepares and sends data of a specified type to the client connected
 * to the server via TCP socket. The data includes the type (e.g., JSONData, OtherData),
 * the size of the data, and the data itself.
 *
 * @param data The QByteArray containing the data to send to the client.
 * @param dataType The type of data being sent (e.g., JSONData, OtherData).
 *
 * Example usage:
 * @code
 * QByteArray imageData = // Load image data into a QByteArray
 * server.sendToClientOther(imageData, ImageData);
 * @endcode
 *
 * @see Server::sockReady
 *
 * @note This method assumes that a client is currently connected and the socket (`socket`) is valid.
 *       Ensure proper error handling and socket management to prevent crashes or undefined behavior.
 */
    long int dataSize = data.size();
    socket->write(reinterpret_cast<const char *>(&dataType), sizeof(dataType));
    socket->write(reinterpret_cast<const char *>(&dataSize), sizeof(dataSize));
    socket->write(data);
}
void Server::sockDisc()
{
    /**
 * @brief Handles disconnection of the client socket.
 *
 * This method is connected to the `disconnected` signal of a QTcpSocket instance.
 * It prints a debug message indicating that the client has disconnected.
 * The method then schedules the socket object for deletion using `deleteLater()`.
 *
 * Example usage:
 * Typically, this method is automatically invoked when a client disconnects from the server.
 * It ensures proper cleanup of resources associated with the client socket.
 *
 * @see Server::sockReady
 *
 * @note This method assumes that the socket (`socket`) is valid and connected.
 *       Ensure proper error handling and socket management to prevent crashes or undefined behavior.
 */
    qDebug()<<"Disconnected";
    socket->deleteLater();
}

int Server::createTeacherId(QJsonObject &jsonObject)
{
    /**
 * @brief Creates a new teacher entry in the database and returns the generated ID.
 *
 * This method inserts a new record into the 'teachers' table of the database,
 * with the provided password and name from the given JSON object.
 * If the insertion is successful, it retrieves and returns the generated ID of the new record.
 *
 * @param jsonObject A reference to a QJsonObject containing 'password' and 'name' fields
 *                   required to create a new teacher entry.
 *
 * @return The generated ID of the newly created teacher, or NaN (Not-a-Number) if creation fails.
 *
 * Example usage:
 * @code
 * QJsonObject teacherData;
 * teacherData["password"] = "securepassword";
 * teacherData["name"] = "John Doe";
 * int teacherId = server.createTeacherId(teacherData);
 * if (teacherId != NAN) {
 *     qDebug() << "New teacher ID:" << teacherId;
 * } else {
 *     qDebug() << "Failed to create teacher.";
 * }
 * @endcode
 *
 * @see Server::sockReady
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */
    int id = NAN;
    QSqlQuery query;
    if (query.exec("INSERT INTO teachers(password, name) VALUES('"
                   + jsonObject["password"].toString() + "','" + jsonObject["name"].toString()
                   + "') RETURNING id")) {
        if (query.next())
            id = query.value(0).toInt();
    }
    return id;
}

QString Server::getTeacherPassword(int id)
{
    /**
 * @brief Retrieves the password of a teacher from the database based on their ID.
 *
 * This method executes a SQL SELECT query to retrieve the password of a teacher
 * from the 'teachers' table in the database, based on the provided teacher ID.
 * If the query is successful and returns a result, the password is extracted from the query result.
 *
 * @param id The ID of the teacher whose password is to be retrieved.
 * @return The password of the teacher as a QString, or an empty string if no password is found.
 *
 * Example usage:
 * @code
 * int teacherId = 123; // Example teacher ID
 * QString password = server.getTeacherPassword(teacherId);
 * if (!password.isEmpty()) {
 *     qDebug() << "Teacher's password:" << password;
 * } else {
 *     qDebug() << "Teacher not found or password not retrieved.";
 * }
 * @endcode
 *
 * @see Server::createTeacherId
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */
    QString password;
    QSqlQuery query;
    if (query.exec("SELECT password FROM teachers WHERE id = " + QString::number(id))) {
        if (query.next())
            password = query.value(0).toString();
    }
    return password;
}

bool Server::ifClassExist(QJsonObject &jsonObject)
{
    /**
 * @brief Checks if a class exists in the database based on the provided class code.
 *
 * Executes a SQL SELECT query to count the number of records in the 'classes' table
 * where the 'id' matches the class code provided in the JSON object.
 *
 * @param jsonObject A reference to a QJsonObject containing the 'classCode' field,
 *                   representing the class code to check for existence.
 * @return True if the class exists in the database, false otherwise.
 */
    bool found = false;
    QSqlQuery query;
    if (query.exec("SELECT COUNT(*) FROM classes WHERE id = "
                   + QString::number(jsonObject["classCode"].toInt()))) {
        if (query.next())
            found = query.value(0).toInt() > 0;
    }
    return found;
}

int Server::createPupilId(QJsonObject &jsonObject)
{
    /**
 * @brief Creates a new pupil entry in the database and returns the generated ID.
 *
 * This method calculates the number of levels for a given class, creates an array
 * with initial level results, and inserts a new record into the 'pupils' table
 * of the database. The pupil's name, class ID, initial level results, and password
 * are taken from the provided JSON object.
 *
 * @param jsonObject A reference to a QJsonObject containing 'name', 'classCode', and 'password'
 *                   fields required to create a new pupil entry.
 * @return The generated ID of the newly created pupil, or NaN (Not-a-Number) if creation fails.
 *
 * Example usage:
 * @code
 * QJsonObject pupilData;
 * pupilData["name"] = "Alice";
 * pupilData["classCode"] = 123; // Example class code
 * pupilData["password"] = "securepassword";
 * int pupilId = server.createPupilId(pupilData);
 * if (pupilId != NAN) {
 *     qDebug() << "New pupil ID:" << pupilId;
 * } else {
 *     qDebug() << "Failed to create pupil.";
 * }
 * @endcode
 *
 * @see Server::ifClassExist
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */
    int classLevels = 0;
    QSqlQuery query1;
    if (query1.exec("SELECT COUNT(*) FROM levels WHERE class_id = "
                    + QString::number(jsonObject["classCode"].toInt()))) {
        if (query1.next())
            classLevels = query1.value(0).toInt();
    }
    QString arrayLevels = "{";
    for (int i = 0; i < classLevels - 1; i++)
        arrayLevels += "0,";
    arrayLevels += "0}";

    QSqlQuery query2;
    query2.prepare("INSERT INTO pupils(name,class_id,levels_result,password) "
                   "VALUES(:name,:classId, :array,:password) RETURNING id");

    query2.bindValue(":name", jsonObject["name"].toString());
    query2.bindValue(":classId", jsonObject["classCode"].toInt());
    query2.bindValue(":array", arrayLevels);
    query2.bindValue(":password", jsonObject["password"].toString());
    int id = NAN;
    if (query2.exec() && query2.next())
        id = query2.value(0).toInt();
    return id;
}

void Server::sendQuestionToClient(QJsonObject &jsonObject)
{
    /**
 * @brief Retrieves a question from the database based on pupil ID and sends it to the client.
 *
 * This method queries the database to retrieve the current level of a pupil identified by
 * `pupilId`. It then retrieves a question related to the current level from the 'levels' table
 * and constructs a JSON object containing the question details. Finally, it sends the JSON string
 * to the connected client.
 *
 * @param jsonObject A reference to a QJsonObject containing 'pupilId', representing the ID of the pupil
 *                   for whom the question is being retrieved.
 *
 * @see Server::sendToClient
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    QSqlQuery query;
    query.prepare("SELECT class_id, levels_result FROM pupils WHERE id = :pupilId");
    query.bindValue(":pupilId", jsonObject["pupilId"].toInt());
    if (query.exec()) {
        if (query.next()) {
            int classId = query.value("class_id").toInt();
            QString levelsResultStr = query.value("levels_result").toString();
            QStringList levelsResult = getListFromDB(levelsResultStr);
            int currentLevel = findFirstZeroIndex(levelsResult);
            QSqlQuery query2;
            query2.prepare("SELECT * FROM levels WHERE class_id = :classId AND num_level = "
                           ":currentLevel");
            query2.bindValue(":classId", classId);
            query2.bindValue(":currentLevel", currentLevel);

            if (query2.exec()) {
                QJsonObject jsonObjectTmp;
                jsonObjectTmp["signal"] = "setQuestion";
                if (query2.next()) {
                    // Формируем JSON-строку с текстом
                    jsonObjectTmp["levelExists"] = true;
                    jsonObjectTmp["answer1"] = query2.value("answer1").toString();
                    jsonObjectTmp["answer2"] = query2.value("answer2").toString();
                    jsonObjectTmp["answer3"] = query2.value("answer3").toString();
                    jsonObjectTmp["rightAnswerPos"]
                        = answersPos[query2.value("right_answer").toInt() - 1];
                    jsonObjectTmp["question"] = query2.value("question").toString();

                } else
                    jsonObjectTmp["levelExists"] = false;

                QJsonDocument jsonDocument(jsonObjectTmp);
                QString toClient = jsonDocument.toJson();
                sendToClient(toClient);
            }
        }
    }
}

int Server::createClassId(QJsonObject &jsonObject)
{
    /**
 * @brief Creates a new class entry in the database and returns the generated ID.
 *
 * This method inserts a new record into the 'classes' table of the database
 * with the provided class name (`className`) and teacher ID (`teacherID`).
 * It initializes the number of levels (`num_levels`) to zero for the new class entry.
 * Upon successful insertion, it retrieves and returns the generated ID of the new class.
 *
 * @param jsonObject A reference to a QJsonObject containing 'className' and 'teacherID' fields,
 *                   representing the name of the class and the ID of the teacher who owns the class.
 * @return The generated ID of the newly created class, or NaN (Not-a-Number) if creation fails.
 *
 * Example usage:
 * @code
 * QJsonObject classData;
 * classData["className"] = "Physics";
 * classData["teacherID"] = 456; // Example teacher ID
 * int classId = server.createClassId(classData);
 * if (classId != NAN) {
 *     qDebug() << "New class ID:" << classId;
 * } else {
 *     qDebug() << "Failed to create class.";
 * }
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */
    int id = NAN;
    QSqlQuery query;
    if (query.exec("INSERT INTO classes(name, id_teacher, num_levels) VALUES('"
                   + jsonObject["className"].toString() + "',"
                   + QString::number(jsonObject["teacherID"].toInt()) + ",0) RETURNING id")) {
        if (query.next())
            id = query.value(0).toInt(); // Получение значения id из результата запроса
    }
    return id;
}

void Server::deleteClass(QJsonObject &jsonObject)
{
    /**
 * @brief Deletes a class and its related data from the database based on the provided class code.
 *
 * This method deletes the class identified by `classCode` from the 'classes' table
 * and its associated records from the 'pupils' and 'levels' tables in the database.
 * It executes SQL DELETE statements for each table, targeting rows where the 'class_id'
 * matches the provided `classCode`.
 *
 * @param jsonObject A reference to a QJsonObject containing 'classCode', representing the ID of the class
 *                   to be deleted from the database.
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    int classId = jsonObject["classCode"].toInt();
    QSqlQuery query;
    query.prepare(QString("DELETE FROM classes WHERE id = :class_id"));
    query.bindValue(":class_id", classId);

    if (!query.exec())
        qWarning() << "Failed to delete from table classes: " << query.lastError().text();

    QStringList tables = {"pupils", "levels"};

    foreach (const QString &table, tables) {
        query.prepare(QString("DELETE FROM %1 WHERE class_id = :class_id").arg(table));
        query.bindValue(":class_id", classId);

        if (!query.exec())
            qWarning() << "Failed to delete from table" << table << ":" << query.lastError().text();
    }
}

void Server::addLevel(QJsonObject &jsonObject)
{
    /**
 * @brief Adds a new level with questions and answers to the database for a specific class.
 *
 * This method inserts a new level entry into the 'levels' table of the database
 * for the class identified by `classId` and `numLevel`. It retrieves questions and answers
 * from the 'level' array in the provided `jsonObject` and constructs an SQL INSERT statement
 * dynamically. Upon execution, the method adds the level data to the database.
 *
 * @param jsonObject A reference to a QJsonObject containing 'classId', 'numLevel', and 'level' fields.
 *                   - 'classId': The ID of the class to which the level belongs.
 *                   - 'numLevel': The level number.
 *                   - 'level': A QJsonArray containing questions and answers for the level.
 *
 * Example usage:
 * @code
 * QJsonObject levelData;
 * levelData["classId"] = 123; // Example class ID
 * levelData["numLevel"] = 1; // Example level number
 * QJsonArray levelArray;
 * levelArray.append("What is the capital of France?");
 * levelArray.append("Paris");
 * levelArray.append("Berlin");
 * levelArray.append("London");
 * levelArray.append(2); // Index of the correct answer
 * levelData["level"] = levelArray;
 *
 * server.addLevel(levelData);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */
    QStringList levelList;
    QJsonArray jsonArray = jsonObject["level"].toArray();
    foreach (const QJsonValue &val, jsonArray) {
        QString str = val.toString(); // Преобразование QJsonValue в QString
        levelList.append(str);
    }
    QString queryText = "INSERT INTO "
                        "levels(class_id,num_level,question,answer1,answer2,"
                        "answer3,right_answer) "
                        "VALUES("
                        + QString::number(jsonObject["classId"].toInt()) + ", "
                        + QString::number(jsonObject["numLevel"].toInt()) + " ";

    int i;
    for (i = 0; i < levelList.length() - 1; i++) {
        queryText += ", '" + levelList[i] + "'";
    }
    queryText += "," + levelList[i] + ")";
    QSqlQuery query;
    if (!query.exec(queryText))
        qDebug() << query.lastError().text();
}

void Server::sendClasses(int id)
{
    /**
 * @brief Retrieves and sends class information associated with a teacher ID to the client.
 *
 * This method queries the 'classes' table in the database to retrieve class names and IDs
 * belonging to the teacher identified by the given `id`. It constructs a JSON object (`jsonObject`)
 * containing the retrieved class information and sends it to the client using the `sendToClient` method.
 *
 * @param id The ID of the teacher whose classes are to be retrieved and sent to the client.
 *
 * Example usage:
 * @code
 * int teacherId = 789; // Example teacher ID
 * server.sendClasses(teacherId);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */
    QSqlQuery query;
    query.prepare("SELECT * FROM classes WHERE id_teacher = :teacherID");
    query.bindValue(":teacherID", id);
    if (!query.exec())
        qDebug() << query.lastError().text();
    QJsonObject jsonObject;
    jsonObject["signal"] = "getClasses";
    while (query.next()) {
        pupilClass item;
        item.name = query.value("name").toString();
        item.id = query.value("id").toInt();
        jsonObject[QString::number(item.id)] = item.name;
    }

    QJsonDocument jsonDocument(jsonObject);
    QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);
    sendToClient(jsonString);
}

void Server::sendPupils(int classId)
{
    /**
 * @brief Retrieves and sends pupil information for a specific class to the client.
 *
 * This method queries the 'pupils' table in the database to retrieve pupil names, IDs,
 * and their level results for the class identified by `classId`. It constructs a binary data
 * packet (`dataToSend`) containing the retrieved information and sends it to the client
 * using the `sendToClientOther` method with the `PupilsData` data type.
 *
 * @param classId The ID of the class for which pupil information is to be retrieved and sent.
 *
 * Example usage:
 * @code
 * int classId = 456; // Example class ID
 * server.sendPupils(classId);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    QSqlQuery query;
    query.prepare("SELECT * FROM pupils WHERE class_id = :classID");
    query.bindValue(":classID", classId);
    if (!query.exec())
        qDebug() << query.lastError().text();
    QByteArray dataToSend;

    while (query.next()) {
        QString name = query.value("name").toString();
        int id = query.value("id").toInt();

        QString resultsVariant = query.value("levels_result").toString();

        // Разбиваем строку по запятым и преобразуем каждый элемент в число
        QStringList resultsList = getListFromDB(resultsVariant);

        dataToSend.append(reinterpret_cast<const char *>(&id), sizeof(id));

        int size = name.toUtf8().size(); // Получение размера строки в байтах в UTF-8
        dataToSend.append(reinterpret_cast<const char *>(&size),
                          sizeof(size)); // Добавление размера в QByteArray
        dataToSend.append(name.toUtf8());
        int resultsLenght = resultsList.length();
        dataToSend.append(reinterpret_cast<const char *>(&resultsLenght), sizeof(resultsLenght));
        for (const QString &result : resultsList) {
            int intResult = result.toInt();
            dataToSend.append(reinterpret_cast<const char *>(&intResult), sizeof(intResult));
        }
    }
    sendToClientOther(dataToSend, PupilsData);
}

void Server::sendLevels(int classId)
{
    /**
 * @brief Retrieves and sends level information for a specific class to the client.
 *
 * This method queries the 'levels' table in the database to retrieve questions,
 * answers, and correct answer indices for levels associated with the given `classId`.
 * It constructs a JSON object (`jsonObject`) containing the retrieved level information
 * and sends it to the client using the `sendToClient` method.
 *
 * @param classId The ID of the class whose levels are to be retrieved and sent to the client.
 *
 * Example usage:
 * @code
 * int classId = 456; // Example class ID
 * server.sendLevels(classId);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    QSqlQuery query;
    query.prepare("SELECT * FROM levels WHERE class_id = :classID");
    query.bindValue(":classID", classId);
    if (!query.exec())
        qDebug() << query.lastError().text();
    QJsonObject jsonObject;
    jsonObject["signal"] = "getLevels";
    while (query.next()) {
        QJsonArray levelInfo;
        levelInfo.append(query.value("question").toString());
        levelInfo.append(query.value("answer1").toString());
        levelInfo.append(query.value("answer2").toString());
        levelInfo.append(query.value("answer3").toString());
        levelInfo.append(QString::number(query.value("right_answer").toInt()));
        jsonObject[QString::number(query.value("num_level").toInt())] = levelInfo;
    }
    QJsonDocument jsonDocument(jsonObject);
    QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);
    sendToClient(jsonString);
}

void Server::setPupilResult(QJsonObject &jsonObject)
{
    /**
 * @brief Updates the result of a pupil for a specific level in the database.
 *
 * This method retrieves the current results of levels for a pupil identified by `pupilId`
 * from the 'pupils' table in the database. It then updates the result of a specific level
 * with the new result provided in `jsonObject`. After updating the result, it updates the
 * database with the modified results array.
 *
 * @param jsonObject A JSON object containing the pupil ID (`pupilId`) and the new result
 *        (`result`) to be updated for the pupil's levels.
 *
 * Example usage:
 * @code
 * QJsonObject jsonObject;
 * jsonObject["pupilId"] = 123; // Example pupil ID
 * jsonObject["result"] = 85;   // Example result to update
 * server.setPupilResult(jsonObject);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    QSqlQuery query;
    query.prepare("SELECT levels_result FROM pupils WHERE id = :pupilID");
    query.bindValue(":pupilID", jsonObject["pupilId"].toInt());
    if (query.exec()) {
        if (query.next()) {
            QString resultsVariant = query.value("levels_result").toString();
            QStringList resultsList = getListFromDB(resultsVariant);
            int ind = findFirstZeroIndex(resultsList);
            resultsList[ind] = QString::number(jsonObject["result"].toInt());

            //створення масиву з результатами рівня -винести в функцію
            QString arrayLevels = "{";
            int i = 0;
            for (i = 0; i < resultsList.length() - 1; i++)
                arrayLevels += resultsList[i] + ",";
            arrayLevels += resultsList[i] + "}";

            QSqlQuery query2;
            query2.prepare("UPDATE pupils SET levels_result = :array WHERE id = :pupilID");
            query2.bindValue(":array", arrayLevels);
            query2.bindValue(":pupilID", jsonObject["pupilId"].toInt());
            if (query2.exec())
                qDebug() << "pupil updated";
        }
    }
}

void Server::checkPupilLogin(QJsonObject &jsonObject)
{
    /**
 * @brief Checks the login credentials of a pupil against the database.
 *
 * This method verifies the login credentials provided in `jsonObject` against the
 * 'pupils' table in the database. It retrieves the password associated with the pupil ID
 * from the database and compares it with the password provided in the JSON object.
 * Based on the comparison, it constructs a response JSON string indicating whether
 * the login was successful or not, and sends it to the client.
 *
 * @param jsonObject A JSON object containing the pupil ID (`id`) and the password
 *        (`password`) to be verified for login.
 *
 * Example usage:
 * @code
 * QJsonObject jsonObject;
 * jsonObject["id"] = 123;               // Example pupil ID
 * jsonObject["password"] = "mypassword"; // Example password
 * server.checkPupilLogin(jsonObject);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    QString toClient;
    QString password;
    QSqlQuery query;
    if (query.exec("SELECT password FROM pupils WHERE id = "
                   + QString::number(jsonObject["id"].toInt()))) {
        if (query.next())
            password = query.value(0).toString(); // Получение значения id из результата запроса
    }
    if (jsonObject["password"].toString() == password)
        toClient = "{\"signal\":\"loginPupil\",\"status\":\"y\",\"id\":"
                   + QString::number(jsonObject["id"].toInt()) + "}";
    else
        toClient = "{\"signal\":\"loginPupil\",\"status\":\"n\"}";
    sendToClient(toClient);
}

void Server::sendPupilPage(QJsonObject &jsonObject)
{
    /**
 * @brief Sends pupil's page information to the client.
 *
 * This method retrieves information about a pupil specified by `jsonObject['id']`
 * from the database. It fetches the pupil's name and their associated class code.
 * Then, it retrieves the teacher's name who teaches the class that the pupil belongs to.
 * The gathered information is formatted into a JSON string and sent to the client
 * using the `sendToClient` method.
 *
 * @param jsonObject A JSON object containing the pupil's ID (`id`).
 *
 * Example usage:
 * @code
 * QJsonObject jsonObject;
 * jsonObject["id"] = 123; // Example pupil ID
 * server.sendPupilPage(jsonObject);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    QString name;
    QString classCode;
    QString teacherName;
    QString teacherId;
    QSqlQuery query;
    if (query.exec("SELECT name, class_id FROM pupils WHERE id = "
                   + QString::number(jsonObject["id"].toInt()))) {
        if (query.next()) {
            name = query.value(0).toString(); // Получение значения name из результата запроса
            classCode = QString::number(query.value(1).toInt());
        }
    }
    QSqlQuery query2;
    if (query2.exec("SELECT id_teacher FROM classes WHERE id = " + classCode)) {
        if (query2.next())
            teacherId = QString::number(query2.value(0).toInt());
    }

    QSqlQuery query3;
    if (query3.exec("SELECT name FROM teachers WHERE id = " + teacherId)) {
        if (query3.next())
            teacherName = query3.value(0).toString();
    }

    QString toClient = "{\"signal\":\"getPupilsPage\",\"name\":\"" + name + "\",\"classCode\":\""
                       + classCode + "\",\"teacherName\":\"" + teacherName + "\"}";
    sendToClient(toClient);
}

void Server::sendPupilRate(QJsonObject &jsonObject)
{
    /**
 * @brief Sends pupil's performance data to the client.
 *
 * This method retrieves the performance data (levels results) of all pupils
 * in a specified class identified by `jsonObject['id']`. It fetches each pupil's
 * name and their respective levels results from the database. The fetched data
 * is formatted into a structured format and sent to the client using the
 * `sendToClientOther` method with `PupilsRate` data type.
 *
 * @param jsonObject A JSON object containing the pupil's ID (`id`).
 *
 * Example usage:
 * @code
 * QJsonObject jsonObject;
 * jsonObject["id"] = 123; // Example pupil ID
 * server.sendPupilRate(jsonObject);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    QString classCode;
    QSqlQuery query;
    if (query.exec("SELECT class_id FROM pupils WHERE id = "
                   + QString::number(jsonObject["id"].toInt()))) {
        if (query.next()) {
            classCode = QString::number(query.value(0).toInt());
        }
    }

    QSqlQuery query2;
    query2.prepare("SELECT * FROM pupils WHERE class_id = :class_id");
    query2.bindValue(":class_id", classCode);
    QByteArray dataToSend;
    if (!query2.exec())
        qDebug() << query.lastError().text();
    while (query2.next()) {
        QString name = query2.value("name").toString();

        QString resultsVariant = query2.value("levels_result").toString();

        // Разбиваем строку по запятым и преобразуем каждый элемент в число
        QStringList resultsList = getListFromDB(resultsVariant);

        int size = name.toUtf8().size(); // Получение размера строки в байтах в UTF-8
        dataToSend.append(reinterpret_cast<const char *>(&size),
                          sizeof(size)); // Добавление размера в QByteArray
        dataToSend.append(name.toUtf8());
        int resultsLenght = resultsList.length();
        dataToSend.append(reinterpret_cast<const char *>(&resultsLenght), sizeof(resultsLenght));
        for (const QString &result : resultsList) {
            int intResult = result.toInt();
            dataToSend.append(reinterpret_cast<const char *>(&intResult), sizeof(intResult));
        }
    }
    sendToClientOther(dataToSend, PupilsRate);
}

void Server::addPupilQuestion(QJsonObject &jsonObject)
{
    /**
 * @brief Adds a question submitted by a pupil to the database and sends a response to the client.
 *
 * This method handles the process of adding a question, along with associated metadata, to the database
 * submitted by a pupil identified by `jsonObject['pupilId']`. It retrieves the pupil's name and class
 * code from the database based on the pupil ID, then inserts the question, pupil's name, class ID, and
 * current date into the `questions` table. After successfully inserting the question, it constructs
 * a response JSON object indicating the status of the operation and sends it to the client.
 *
 * @param jsonObject A JSON object containing the pupil's ID (`pupilId`) and the question (`question`) to be added.
 *
 * Example usage:
 * @code
 * QJsonObject jsonObject;
 * jsonObject["pupilId"] = 123; // Example pupil ID
 * jsonObject["question"] = "What is the capital of France?"; // Example question
 * server.addPupilQuestion(jsonObject);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    QJsonObject jsonObject2;
    jsonObject2["signal"] = "questionOk";
    jsonObject2["status"] = false;
    QString name;
    QString classCode;
    QSqlQuery query;
    if (query.exec("SELECT name, class_id FROM pupils WHERE id = "
                   + QString::number(jsonObject["pupilId"
                                                ""]
                                         .toInt()))) {
        if (query.next()) {
            name = query.value(0).toString(); // Получение значения name из результата запроса
            classCode = QString::number(query.value(1).toInt());
        }
    }
    QDate currentDate = QDate::currentDate();
    QSqlQuery query2;
    query2.prepare("INSERT INTO questions(pupil_name,question,class_id,date) "
                   "VALUES(:pupilName,:question,:class_id,:date);");
    query2.bindValue(":pupilName", name);
    query2.bindValue(":question", jsonObject["question"].toString());
    query2.bindValue(":class_id", classCode);
    query2.bindValue(":date", currentDate);
    if (!query2.exec())
        qDebug() << query.lastError().text();
    else {
        jsonObject2["status"] = true;
        ;
    }
    QJsonDocument jsonDoc(jsonObject2);
    QString toClient = jsonDoc.toJson(QJsonDocument::Compact);
    sendToClient(toClient);
}

void Server::sendPupilsQuestions(QJsonObject &jsonObject)
{
    /**
 * @brief Sends questions submitted by pupils in a specific class to the client.
 *
 * This method retrieves information about questions submitted by pupils in a specified class
 * identified by `jsonObject["classId"]`. It first retrieves the name of the class from the
 * `classes` table and sends it to the client. Then, it retrieves all questions from the
 * `questions` table where the `class_id` matches `jsonObject["classId"]`. For each question,
 * it retrieves the pupil's name (`pupil_name`), the question text (`question`), and the
 * submission date (`date`). Each piece of information is serialized into a QByteArray and
 * sent to the client using the `sendToClientOther` method with the signal `QuestionsData`.
 *
 * @param jsonObject A JSON object containing the `classId` of the class for which questions
 *                  should be sent to the client.
 *
 * Example usage:
 * @code
 * QJsonObject jsonObject;
 * jsonObject["classId"] = 123; // Example class ID
 * server.sendPupilsQuestions(jsonObject);
 * @endcode
 *
 * @note This method assumes that the database connection (`db`) is established and valid.
 *       Ensure proper error handling and database management to prevent crashes or undefined behavior.
 */

    QSqlQuery query;
    QByteArray dataToSend;
    if (query.exec("SELECT name FROM classes WHERE id = "
                   + QString::number(jsonObject["classId"].toInt()))) {
        if (query.next()) {
            QString className = query.value("name").toString();
            int size = className.toUtf8().size(); // Получение размера строки в байтах в UTF-8
            dataToSend.append(reinterpret_cast<const char *>(&size),
                              sizeof(size)); // Добавление размера в QByteArray
            dataToSend.append(className.toUtf8());
        }
    }
    if (query.exec("SELECT * FROM questions WHERE class_id = "
                   + QString::number(jsonObject["classId"].toInt()))) {
        while (query.next()) {
            QString question = query.value("question").toString();
            QString name = query.value("pupil_name").toString();
            QString date = query.value("date").toDate().toString();

            int size = name.toUtf8().size(); // Получение размера строки в байтах в UTF-8
            dataToSend.append(reinterpret_cast<const char *>(&size),
                              sizeof(size)); // Добавление размера в QByteArray
            dataToSend.append(name.toUtf8());

            size = question.toUtf8().size(); // Получение размера строки в байтах в UTF-8
            dataToSend.append(reinterpret_cast<const char *>(&size),
                              sizeof(size)); // Добавление размера в QByteArray
            dataToSend.append(question.toUtf8());

            size = date.toUtf8().size(); // Получение размера строки в байтах в UTF-8
            dataToSend.append(reinterpret_cast<const char *>(&size),
                              sizeof(size)); // Добавление размера в QByteArray
            dataToSend.append(date.toUtf8());
        }
        sendToClientOther(dataToSend, QuestionsData);
    }
}

QStringList Server::getListFromDB(QString str)
{
    /**
 * @brief Converts a string representation of an array into a QStringList.
 *
 * This method takes a QString `str` that represents an array in string format,
 * typically retrieved from a database. The method removes the enclosing curly braces
 * '{' and '}', splits the string by commas, and converts each substring into
 * elements of a QStringList. The resulting QStringList contains each element
 * from the original string array representation.
 *
 * @param str A QString representing an array in string format, e.g., "{1,2,3}".
 *            The string should be formatted with elements separated by commas
 *            and enclosed in curly braces.
 * 
 * @return QStringList containing each element extracted from the input string `str`.
 *
 * Example usage:
 * @code
 * QString arrayString = "{1,2,3,4}";
 * QStringList list = getListFromDB(arrayString);
 * // list now contains ["1", "2", "3", "4"]
 * @endcode
 *
 * @note This method assumes the input string `str` is properly formatted
 *       with elements separated by commas and enclosed in curly braces.
 */
    str.remove(QChar('{'), Qt::CaseInsensitive);
    str.remove(QChar('}'), Qt::CaseInsensitive);
    QStringList resultsList = str.split(',');
    return resultsList;
}

int Server::findFirstZeroIndex(const QStringList &levelsResult)
{
    /**
 * @brief Finds the index of the first occurrence of "0" in the given QStringList.
 *
 * This method searches through the elements of the QStringList `levelsResult`
 * to find the index of the first occurrence of the string "0". If no "0" is found
 * in the list, the method returns -1.
 *
 * @param levelsResult A QStringList containing elements to search through.
 *
 * @return The index of the first occurrence of "0" in the QStringList `levelsResult`,
 *         or -1 if no "0" is found.
 *
 * Example usage:
 * @code
 * QStringList resultsList = {"1", "2", "0", "3", "0"};
 * int index = findFirstZeroIndex(resultsList);
 * // index is now 2 (index of the first occurrence of "0" in resultsList)
 * @endcode
 */

    for (int i = 0; i < levelsResult.size(); ++i) {
        if (levelsResult[i] == "0") {
            return i;
        }
    }
    return -1;
}
Server::~Server()
{
    QSqlDatabase::database().close();
}
