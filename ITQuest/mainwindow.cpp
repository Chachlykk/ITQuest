#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "iostream"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    currentUser.id = -1;
    currentUser.userType = "none";
    time_of_game=0;
    ui->setupUi(this);
    ui->accBtn->setVisible(false);
    socket=new ClientTCP(this);
    ui->tableClasses->setEditTriggers(QTableWidget::NoEditTriggers);
    ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
    connect(&registration, &RegistrationForm::sendToServer, socket, &ClientTCP::sendToServer);
    connect(&addClass, &AddClass::sendToServer, socket, &ClientTCP::sendToServer);
    connect(socket, &ClientTCP::registrTeacherID, this, &MainWindow::registrTeacherID);
    connect(socket, &ClientTCP::loginCorrectTeacher, this, &MainWindow::loginCorrectTeacher);
    connect(socket, &ClientTCP::loginCorrectPupil, this, &MainWindow::loginCorrectPupil);
    connect(socket, &ClientTCP::registrPupilID, this, &MainWindow::registrPupilID);
    connect(socket, &ClientTCP::setLevel, this, &MainWindow::setLevel);
    connect(socket, &ClientTCP::addClass, this, &MainWindow::add_class);
    connect(socket, &ClientTCP::showClasses, this, &MainWindow::showClasses);
    connect(socket, &ClientTCP::showPupils, this, &MainWindow::showPupils);
    connect(socket, &ClientTCP::showLevels, this, &MainWindow::showLevels);
    connect(socket, &ClientTCP::getPupilsPage, this, &MainWindow::getPupilsPage);
    connect(socket, &ClientTCP::showPupilsRate, this, &MainWindow::showPupilsRate);
    connect(socket, &ClientTCP::showPupilQuestions, this, &MainWindow::showPupilQuestions);
    connect(socket, &ClientTCP::sentQuestionStatus, this, &MainWindow::sentQuestionStatus);

    connect(&askQuestion, &AskQuestion::sendQuestion, this, &MainWindow::sendQuestion);
    connect(&add_level, &addLevel::addLevelMain, this, &MainWindow::addLevelMain);

    connect(ui->playWidget, &PlayWidget::openMenu, this, &MainWindow::openMenu);

    connect(ui->playWidget, &PlayWidget::gameEnd, this, &MainWindow::gameEnd);
    connect(ui->playWidget, &PlayWidget::callQuestion, this, &MainWindow::openQuestion);
    connect(ui->backMenuBtn_2, &QPushButton::clicked, this, &MainWindow::on_backMenuBtn_clicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);
    connect(ui->backMenuBtn_3, &QPushButton::clicked, this, &MainWindow::on_backMenuBtn_clicked);

    socket->startConnection();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape){
        ui->menuWidget->setVisible(true);
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        QWidget::keyPressEvent(event); // Вызов базовой реализации для других клавиш
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // close all dialog windows
    registration.close();
    event->accept();
}

void MainWindow::onTimeSecond()
{
    time_of_game+=1;
}

void MainWindow::loginCorrectTeacher(QString status, int id)
{
    if(status=="y"){
        registration.setCurrentIndex(0);
        registration.hide();
        currentUser.userType = "teacher";
        currentUser.id = id;
        clearTeacherPage();

        QString toServer = "{\"signal\":\"getClasses\",\"teacherId\":"
                           + QString::number(currentUser.id) + "}";
        socket->sendToServer(toServer);

        ui->stackedWidget->setCurrentIndex(1);
        QString message = QString("Login successful");
        QMessageBox::information(&registration, "Login", message);
        changeRegistrButton();

    }else{
        QString message = QString("Invalid login data");
        QMessageBox::information(&registration, "Login", message);
    }
}

void MainWindow::loginCorrectPupil(QString status, int id)
{
    if (status == "y") {
        registration.setCurrentIndex(0);
        registration.hide();
        currentUser.userType = "pupil";
        currentUser.id = id;
        clearPupilPage();
        QString toServer = "{\"signal\":\"getPupilsPage\",\"id\":" + QString::number(currentUser.id)
                           + "}";
        socket->sendToServer(toServer);
        toServer = "{\"signal\":\"getPupilsRate\",\"id\":" + QString::number(currentUser.id) + "}";
        socket->sendToServer(toServer);
        ui->stackedWidget->setCurrentIndex(2);
        QString message = QString("Login successful");
        QMessageBox::information(&registration, "Login", message);
        changeRegistrButton();
    } else {
        QString message = QString("Invalid login data");
        QMessageBox::information(&registration, "Login", message);
    }
    ui->playWidget->ifPaint = true;
}

void MainWindow::registrPupilID(QJsonDocument &data)
{
    QJsonObject jsonObject = data.object();
    if (jsonObject["status"].toString() == "y") {
        registration.setCurrentIndex(0);
        registration.hide();
        QString message = QString("Successfull registration");
        currentUser.userType = "pupil";
        currentUser.id = jsonObject["id"].toInt();
        QMessageBox::information(&registration, "Login", message);
        changeRegistrButton();
        QString toServer = "{\"signal\":\"getPupilsPage\",\"id\":" + QString::number(currentUser.id)
                           + "}";
        socket->sendToServer(toServer);
        toServer = "{\"signal\":\"getPupilsRate\",\"id\":" + QString::number(currentUser.id) + "}";
        socket->sendToServer(toServer);
        clearPupilPage();
        ui->stackedWidget->setCurrentIndex(2);
    } else {
        QString message = QString("Невірні дані для входу ");
        QMessageBox::information(&registration, "Login", message);
    }
}

void MainWindow::setLevel(QJsonDocument &data)
{
    if (data["levelExists"].toBool()) {
        ui->playWidget->setBackgroundImage(":/map/map_level1.jpg");
        QList<QString> answers;
        answers << data["answer1"].toString() << data["answer2"].toString()
                << data["answer3"].toString();
        ui->playWidget->makeAnsLabels(QStringList(answers));
        ui->playWidget->setRightAnswerX(data["rightAnswerPos"].toInt());
        (ui->menuWidget)->setVisible(false);
        registration.hide();
        question.setQuestion(data["question"].toString());
        question.show();
        ui->playWidget->enable_keypress = true;
    } else {
        QMessageBox::information(this,
                                 "Levels ended",
                                 "You have passed all the levels.\nCongratulations!");
    }
}

void MainWindow::showPupils(QVector<Pupil> &pupils)
{
    emit clearClassData();
    ui->tableClasses->setColumnCount(3);
    QStringList headers;
    headers << "pupil's code"
            << "pupil`s name"
            << "coefficient";
    ui->tableClasses->setHorizontalHeaderLabels(headers);

    int countPupils = pupils.length();
    if (countPupils != 0) {
        int countLevels = pupils[0].results.length();
        ui->tableClasses->setColumnCount(countLevels + 3);
        QStringList headers;
        headers << "pupil's code"
                << "pupil`s name"
                << "coefficient";
        for (int i = 3; i <= countLevels + 3; ++i) {
            headers << "Level" + QString::number(i - 2);
        }
        ui->tableClasses->setHorizontalHeaderLabels(headers);
        for (int i = 0; i < countPupils; i++) {
            ui->tableClasses->insertRow(i);
            ui->tableClasses->setItem(i, 0, new QTableWidgetItem(QString::number(pupils[i].id)));
            ui->tableClasses->setItem(i, 1, new QTableWidgetItem(pupils[i].name));

            int rightAnswers = 0;
            for (int j = 0; j < countLevels; j++) { //показати рівні
                int result = pupils[i].results[j];
                rightAnswers += result;
                ui->tableClasses->setItem(i, j + 3, new QTableWidgetItem(QString::number(result)));
            }
            QString coefficient = QString::number(rightAnswers) + "/"
                                  + QString::number(countLevels * 3);
            ui->tableClasses->setItem(i, 2, new QTableWidgetItem(coefficient));
        }
    }
}

void MainWindow::showLevels(QJsonObject &data)
{
    QList<QString> keys = data.keys();
    // Сортировка списка по возрастанию чисел с использованием lambda-функции
    std::sort(keys.begin(), keys.end(), [](const QString &s1, const QString &s2) {
        return s1.toInt() < s2.toInt(); // Сортировка как целых чисел
    });
    // Перебираем ключи и получаем значения по каждому ключу
    int i = 0;
    for (const QString &key : keys) {
        QJsonValue jsonValue = data.value(key);
        QJsonArray jsonArray = jsonValue.toArray();
        ui->tableWidget->insertRow(i);
        for (int j = 0; j < 5; j++) {
            QString questItem = jsonArray[j].toString();
            ui->tableWidget->setItem(i, j, new QTableWidgetItem(questItem));
        }
        i++;
    }
}

void MainWindow::getPupilsPage(QJsonObject &data)
{
    ui->label_6->setText(data["name"].toString());
    ui->label_7->setText(data["classCode"].toString());
    ui->label_5->setText(data["teacherName"].toString());
}

void MainWindow::showPupilsRate(QVector<Pupil> &pupils)
{
    int rowCount = ui->tableWidget_2->rowCount();
    ui->tableWidget_2->clearContents(); // Очистка содержимого ячеек
    for (int i = rowCount - 1; i >= 0; --i) {
        ui->tableWidget_2->removeRow(i); // Удаление строк
    }

    int countPupils = pupils.length();
    int tableWidth = ui->tableWidget_2->width();
    int progressBarWidth = tableWidth / 2;
    if (countPupils != 0) {
        int countLevels = pupils[0].results.length();
        ui->tableWidget_2->setColumnCount(countLevels + 2);
        ui->tableWidget_2->insertRow(0);
        ui->tableWidget_2->setItem(0, 0, new QTableWidgetItem("name"));
        ui->tableWidget_2->setItem(0, 1, new QTableWidgetItem("progress"));
        for (int i = 1; i <= countPupils; i++) {
            ui->tableWidget_2->insertRow(i);
            ui->tableWidget_2->setItem(i, 0, new QTableWidgetItem(pupils[i - 1].name));

            int rightAnswers = 0;
            for (int j = 0; j < countLevels; j++) { //показати рівні
                int result = pupils[i - 1].results[j];
                rightAnswers += result;
            }
            QProgressBar *progressBar = new QProgressBar();
            progressBar->setFixedHeight(30);              // Устанавливаем высоту
            progressBar->setFixedWidth(progressBarWidth); // Устанавливаем ширину
            // Настраиваем стили прогресс-бара с помощью CSS
            progressBar->setStyleSheet("QProgressBar {"
                                       "margin-top: 15px"
                                       "border: 1px solid grey;"
                                       "border-radius: 5px;"
                                       "background-color: pink;"
                                       "}"
                                       "QProgressBar::chunk {"
                                       "background-color: green;"
                                       "width: 20px;" // Минимальная ширина каждой части прогресса
                                       "}");
            progressBar->setRange(0,
                                  countLevels
                                      * 3); // Устанавливаем диапазон от 0 до общего количества уровней
            progressBar->setValue(
                rightAnswers); // Устанавливаем значение на количество пройденных уровней
            progressBar->setTextVisible(
                false); // Опционально: скрыть текст прогресса на прогресс-баре
            QWidget *container = new QWidget();
            QVBoxLayout *layout = new QVBoxLayout(container);
            layout->addWidget(progressBar);
            layout->setAlignment(progressBar, Qt::AlignCenter); // Выровнять прогресс-бар по центру
            layout->setContentsMargins(0, 0, 0, 0); // Убираем отступы внутри контейнера
            container->setStyleSheet("QWidget {"
                                     "background-color: transparent;"
                                     "}");
            // Вставляем контейнер с прогресс-баром в таблицу на определенную позицию (например, 0-я строка, 1-й столбец)
            ui->tableWidget_2->setCellWidget(i, 1, container);
        }
    }
    QFont font("Courier New", 16, QFont::Bold);
    font.setItalic(true);
    for (int column = 0; column < ui->tableWidget_2->columnCount(); ++column) {
        QTableWidgetItem *item = ui->tableWidget_2->item(0, column); // Первая строка, все столбцы
        if (item) {
            item->setFont(font);
        }
    }
    for (int column = 1; column < ui->tableWidget_2->columnCount(); ++column) {
        for (int row = 0; row < ui->tableWidget_2->rowCount(); ++row) {
            QTableWidgetItem *item = ui->tableWidget_2->item(row, column);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    for (int row = 0; row < ui->tableWidget_2->rowCount(); ++row) {
        ui->tableWidget_2->setRowHeight(row, 60);
    }

    ui->tableWidget_2->resizeColumnsToContents();
    ui->tableWidget_2->setColumnWidth(1, tableWidth / 1.8);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void MainWindow::showClasses(QJsonObject &data)
{
    QList<QString> keys = data.keys();
    // Перебираем ключи и получаем значения по каждому ключу
    for (const QString &key : keys) {
        classCodes.push_back(key.toInt()); //додати id

        QJsonValue jsonValue = data.value(key); //додати назву
        ui->classesComboBox->addItem(jsonValue.toString());
    }
}

void MainWindow::sendQuestion(QString question)
{
    QString toServer = "{\"signal\":\"askQuestion\",\"pupilId\":" + QString::number(currentUser.id)
                       + ",\"question\":\"" + question + "\"}";
    socket->sendToServer(toServer);
}

void MainWindow::showPupilQuestions(QVector<Question> &questions, QString className)
{
    int rowCount = ui->tableWidget_3->rowCount();
    ui->tableWidget_3->clearContents(); // Очистка содержимого ячеек
    for (int i = rowCount - 1; i >= 0; --i) {
        ui->tableWidget_3->removeRow(i); // Удаление строк
    }

    QString classId = ui->classCodeLabel->text();
    ui->label_10->setText(className);
    ui->label_12->setText(classId);
    int countQuestions = questions.length();
    ui->tableWidget_3->setColumnCount(3);
    QStringList headers;
    headers << "pupil's name"
            << "question"
            << "date";
    ui->tableWidget_3->setHorizontalHeaderLabels(headers);

    if (countQuestions != 0)
        for (int i = 0; i < countQuestions; i++) {
            ui->tableWidget_3->insertRow(i);
            ui->tableWidget_3->setItem(i, 0, new QTableWidgetItem(questions[i].name));
            ui->tableWidget_3->setItem(i, 1, new QTableWidgetItem(questions[i].question));
            ui->tableWidget_3->setItem(i, 2, new QTableWidgetItem(questions[i].date));
        }
    int tableWidth = ui->tableWidget_3->width();
    ui->tableWidget_3->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_3->setColumnWidth(1, tableWidth / 2.1);
    ui->tableWidget_3->setColumnWidth(2, tableWidth / 4);
    ui->tableWidget_3->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void MainWindow::sentQuestionStatus(bool status)
{
    if (status)
        QMessageBox::information(this, "Ask the teacher", "Sent successfully");
    else
        QMessageBox::information(this, "Ask the teacher", "Error occured while sending. Try again");
}

void MainWindow::changeRegistrButton()
{
    ui->registrationBtn->setVisible(false);
    ui->accBtn->setVisible(true);
}

void MainWindow::clearTeacherPage()
{
    clearClassData();
    ui->classesComboBox->clear();
    classCodes.clear();
    ui->classCodeLabel->setText(" ");
    ui->classesComboBox->addItem("Choose the class...");
}

void MainWindow::clearClassData()
{
    //видалення даних з таблиці з рівнями класу
    int rowCount = ui->tableWidget->rowCount();
    ui->tableWidget->clearContents(); // Очистка содержимого ячеек
    for (int i = rowCount - 1; i >= 0; --i) {
        ui->tableWidget->removeRow(i); // Удаление строк
    }

    //видалення даних з таблиці з учнями
    int columnCount = ui->tableClasses->columnCount();
    for (int i = columnCount - 1; i >= 0; --i) {
        ui->tableClasses->removeColumn(i);
    }

    rowCount = ui->tableClasses->rowCount();
    for (int i = rowCount - 1; i >= 0; --i) {
        ui->tableClasses->removeRow(i); // Удаление строк
    }
}

void MainWindow::clearPupilPage()
{
    //видалення даних з таблиці з рівнями класу
    int rowCount = ui->tableWidget_2->rowCount();
    ui->tableWidget_2->clearContents(); // Очистка содержимого ячеек
    for (int i = rowCount - 1; i >= 0; --i) {
        ui->tableWidget_2->removeRow(i); // Удаление строк
    }
    ui->label_5->setText(" ");
    ui->label_6->setText(" ");
    ui->label_7->setText(" ");
}

void MainWindow::openMenu()
{
    ui->menuWidget->show();
}

void MainWindow::gameEnd(int mark)
{
    ui->menuWidget->show();
    if (mark < 1)
        mark = 1;
    ui->playWidget->ifPaint = true;
    QString toServer = "{\"signal\":\"levelEnd\",\"pupilId\":" + QString::number(currentUser.id)
                       + ",\"result\":" + QString::number(mark) + "}";
    socket->sendToServer(toServer);
    QMessageBox::information(this, "Victory", "Your points: " + QString::number(mark));
    toServer = "{\"signal\":\"getPupilsRate\",\"id\":" + QString::number(currentUser.id) + "}";
    socket->sendToServer(toServer);
}

void MainWindow::add_class(QJsonDocument &data)
{
    classCodes.push_back(data["classCode"].toInt());
    ui->classesComboBox->addItem(data["className"].toString());
    ui->classesComboBox->setCurrentIndex(classCodes.size());
    ui->classCodeLabel->setText(QString::number(data["classCode"].toInt()));
}

void MainWindow::addLevelMain(const QStringList &row)
{
    int rowIdx = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowIdx);
    for (int i = 0; i < row.count() - 1; ++i) {
        QTableWidgetItem *item = new QTableWidgetItem(row.at(i));
        ui->tableWidget->setItem(rowIdx, i, item);
    }
    QTableWidgetItem *item = new QTableWidgetItem(row.at(row.count() - 1));
    ui->tableWidget->setItem(rowIdx, row.count() - 1, item);

    QJsonArray arrayJSON;
    foreach (const QString &str, row) {
        arrayJSON.append(str);
    }
    QJsonDocument doc(arrayJSON);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    // Вставка JSON-строки в вашу строку toServer
    QString toServer = "{\"signal\":\"addLevel\",\"numLevel\":" + QString::number(rowIdx)
                       + ",\"classId\":" + ui->classCodeLabel->text()
                       + QString(",\"level\":%1}").arg(jsonString);
    socket->sendToServer(toServer);
}

void MainWindow::registrTeacherID(int id)
{
    QString message = QString("Your ID: %1.\nRemember it for the next login.").arg(id);
    QMessageBox::information(this, "ID", message);
    currentUser.userType = "teacher";
    currentUser.id = id;
    clearTeacherPage();
    ui->stackedWidget->setCurrentIndex(1);
    changeRegistrButton();
}

void MainWindow::on_addClassBtn_clicked()
{
    addClass.show();
}

void MainWindow::on_playBtn_clicked()
{
    if (ui->playWidget->ifPaint) {
        if (currentUser.userType == "pupil") {
            QString toServer = "{\"signal\":\"getQuestion\",\"pupilId\":"
                               + QString::number(currentUser.id) + "}";
            socket->sendToServer(toServer);
        } else {
            ui->playWidget->setBackgroundImage(":/map/map_level1.jpg");
            ui->playWidget->makeAnsLabels(
                QStringList({QString("MS Word"), QString("MS Excel"), QString("MS PowerPoint")}));
            ui->playWidget->setRightAnswerX(900);
            (ui->menuWidget)->setVisible(false);
            registration.hide();
            question.setQuestion("Which of the following is used to work with tables?");
            question.show();
            ui->playWidget->enable_keypress = true;
        }
        ui->playWidget->ifPaint = false;
    } else {
        registration.hide();
        question.show();
        (ui->menuWidget)->setVisible(false);
        ui->playWidget->enable_keypress = true;
    }
}

void MainWindow::on_registrationBtn_clicked()
{
    registration.show();
    registration.raise();
}

void MainWindow::on_backMenuBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_deleteClassBtn_clicked()
{
    QString classCode = ui->classCodeLabel->text();
    QString toServer = "{\"signal\":\"deleteClass\",\"classCode\":" + classCode + "}";
    socket->sendToServer(toServer);
    //Поиск элемента в векторе classCodes
    auto it = std::find(classCodes.begin(), classCodes.end(), ui->classCodeLabel->text().toInt());

    // Проверка, найден ли элемент
    if (it != classCodes.end()) {
        // Вычисление индекса элемента в векторе
        int ind = std::distance(classCodes.begin(), it);
        // Удаление элемента из вектора
        classCodes.erase(classCodes.begin() + ind);
        ui->classesComboBox->removeItem(ind);
    }
    ui->classCodeLabel->setText(" ");
    ui->classesComboBox->setCurrentIndex(0);
    clearClassData();
    ui->tableClasses->setColumnCount(3);
    QStringList headers;
    headers << "pupil's code"
            << "pupil`s name"
            << "coefficient";
    ui->tableClasses->setHorizontalHeaderLabels(headers);
    ui->deleteClassBtn->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->addLevels->setEnabled(false);
}

void MainWindow::on_addLevels_clicked()
{
    add_level.raise();
    add_level.show();
}

void MainWindow::on_accBtn_clicked()
{
    if (currentUser.userType == "teacher")
        ui->stackedWidget->setCurrentIndex(1);
    else {
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void MainWindow::on_pushButton_clicked()
{
    currentUser.id = -1;
    currentUser.userType = "";
    ui->registrationBtn->setVisible(true);
    ui->accBtn->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_4_clicked()
{
    askQuestion.raise();
    askQuestion.show();
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    QString id = ui->classCodeLabel->text();
    QString toServer = "{\"signal\":\"getPupilsQuestions\",\"classId\":" + id + "}";
    socket->sendToServer(toServer);
}

void MainWindow::on_pushButton_5_clicked()
{
    int rowCount = ui->tableWidget_3->rowCount();
    ui->tableWidget_3->clearContents(); // Очистка содержимого ячеек
    for (int i = rowCount - 1; i >= 0; --i) {
        ui->tableWidget_3->removeRow(i); // Удаление строк
    }
    QString classId = ui->classCodeLabel->text();
    QString toServer = "{\"signal\":\"deleteQuestions\",\"classId\":" + classId + "}";
    socket->sendToServer(toServer);
}

void MainWindow::on_classesComboBox_currentIndexChanged(int index)
{
    QString id;
    if (index == 0 || index == -1) {
        ui->deleteClassBtn->setEnabled(false);
        ui->addLevels->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        id = " ";
    } else {
        ui->deleteClassBtn->setEnabled(true);
        ui->addLevels->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        id = QString::number(classCodes[index - 1]);
        QString toServer = "{\"signal\":\"getPupils\",\"classId\":" + id + "}";
        socket->sendToServer(toServer);
        toServer = "{\"signal\":\"getLevels\",\"classId\":" + id + "}";
        socket->sendToServer(toServer);
    }
    ui->classCodeLabel->setText(id);
}
