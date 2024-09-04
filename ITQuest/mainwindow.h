#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QComboBox>
#include <QJsonArray>
#include <QKeyEvent>
#include <QMainWindow>
#include <QProgressBar>
#include <QStackedWidget>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QWidget>
#include "addclass.h"
#include "addlevel.h"
#include "askquestion.h"
#include "clienttcp.h"
#include "messageform.h"
#include "registrationform.h"
#include <algorithm>
#include <iterator>
#include <vector>

#include "global_vars.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void clearClasses();
    void deleteClass(int class_id);
    void add_class_to_list(QString className, int class_code);
private slots:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);
    void onTimeSecond();

    void on_addClassBtn_clicked();

    void on_playBtn_clicked();

    void on_registrationBtn_clicked();

    void on_backMenuBtn_clicked();

    void on_deleteClassBtn_clicked();

    void on_addLevels_clicked();

    void on_accBtn_clicked();

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_classesComboBox_currentIndexChanged(int index);

public slots:
    void openMenu();
    void gameEnd(int mark);
    void add_class(QJsonDocument &data);
    void addLevelMain(const QStringList &row);
    void registrTeacherID(int id);
    void loginCorrectTeacher(QString status, int id);
    void loginCorrectPupil(QString status, int id);
    void registrPupilID(QJsonDocument &data);
    void setLevel(QJsonDocument &data);
    void showPupils(QVector<Pupil> &pupils);
    void showLevels(QJsonObject &data);
    void getPupilsPage(QJsonObject &data);
    void showPupilsRate(QVector<Pupil> &pupils);
    void showClasses(QJsonObject &data);
    void sendQuestion(QString question);
    void showPupilQuestions(QVector<Question> &questions, QString className);
    void sentQuestionStatus(bool status);

private:
    RegistrationForm registration;
    AddClass addClass;
    addLevel add_level;
    AskQuestion askQuestion;
    messageForm question;
    QComboBox *classesComboBox;
    //PlayView *playWidget;
    Ui::MainWindow *ui;
    int time_of_game;
    ClientTCP* socket;
    std::vector<int> classCodes;

    void openQuestion()
    {
        question.show();
        question.raise();
    }
    void changeRegistrButton();
    void clearTeacherPage();
    void clearClassData();
    void clearPupilPage();
};
#endif // MAINWINDOW_H
