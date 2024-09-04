#ifndef REGISTRATIONFORM_H
#define REGISTRATIONFORM_H

#include <QMessageBox>
#include <QWidget>
#include "global_vars.h"
namespace Ui {
class RegistrationForm;
}

class RegistrationForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationForm(QWidget *parent = nullptr);
    ~RegistrationForm();
    void setCurrentIndex(int index);

signals:
    void openTeacherPage(int id);//додати передачу id та password
    void registrTeacher(QString password);
    void sendToServer(QString toServer);

private slots:

    void on_teacherBtn_clicked();

    void on_teacherLoginBtn_clicked();

    void on_confirmLoginBtn_clicked();

    void on_pupilBtn_clicked();

    void on_teacherRegistBtn_clicked();

    void on_registrBtnP_clicked();

    void on_pupilLoginBtn_clicked();

    void on_pupilRegistBtn_2_clicked();

    void on_registrBtnT_clicked();

    void on_confirmLoginBtnP_clicked();

private:
    Ui::RegistrationForm *ui;
    void closeEvent(QCloseEvent *event);
};

#endif // REGISTRATIONFORM_H
