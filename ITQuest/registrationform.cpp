#include "registrationform.h"
#include <QDebug>
#include "ui_registrationform.h"
RegistrationForm::RegistrationForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistrationForm)
{
    ui->setupUi(this);
}

RegistrationForm::~RegistrationForm()
{
    delete ui;
}

void RegistrationForm::setCurrentIndex(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void RegistrationForm::closeEvent(QCloseEvent *event)
{
    (ui->stackedWidget)->setCurrentIndex(0);
    QWidget::closeEvent(event);
}

void RegistrationForm::on_teacherBtn_clicked()
{
    (ui->stackedWidget)->setCurrentIndex(2);
}


void RegistrationForm::on_teacherLoginBtn_clicked()
{
    qDebug() << "on_teacherLoginBtn_clicked";
    (ui->stackedWidget)->setCurrentIndex(5);
}


void RegistrationForm::on_confirmLoginBtn_clicked()
{
    QString id = ui->lineEdit_4->text();
    QString password=ui->lineEdit_5->text();
    QString toServer="{\"signal\":\"loginTeacher\",\"id\":"+id+",\"password\":\""+password+"\"}";

    emit sendToServer(toServer);
}


void RegistrationForm::on_pupilBtn_clicked()
{
    (ui->stackedWidget)->setCurrentIndex(3);
}

void RegistrationForm::on_teacherRegistBtn_clicked()
{
    (ui->stackedWidget)->setCurrentIndex(4);
}

void RegistrationForm::on_registrBtnP_clicked()
{
    (ui->stackedWidget)->setCurrentIndex(0);
    this->hide();
    QString classCode = ui->lineEdit_7->text();
    QString name = ui->lineEdit_8->text();
    QString password = ui->lineEdit_6->text();
    QString toServer = "{\"signal\":\"registrPupil\",\"classCode\":" + classCode + ",\"name\":\""
                       + name + "\",\"password\":\"" + password + "\"}";
    emit sendToServer(toServer);
    // (ui->stackedWidget)->setCurrentIndex(0);
    // this->hide();
}

void RegistrationForm::on_registrBtnT_clicked()
{
    (ui->stackedWidget)->setCurrentIndex(0);
    this->hide();
    QString password = ui->lineEdit_9->text();
    QString name = ui->lineEdit_10->text();
    emit sendToServer("{\"signal\":\"registrTeacher\",\"password\":\"" + password + "\",\"name\":\""
                      + name + "\"}");
}

void RegistrationForm::on_pupilLoginBtn_clicked()
{
    (ui->stackedWidget)->setCurrentIndex(1);
}

void RegistrationForm::on_confirmLoginBtnP_clicked()
{
    QString id = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    QString toServer = "{\"signal\":\"loginPupil\",\"id\":" + id + ",\"password\":\"" + password
                       + "\"}";
    emit sendToServer(toServer);
    // (ui->stackedWidget)->setCurrentIndex(0);
    // this->hide();
}

void RegistrationForm::on_pupilRegistBtn_2_clicked()
{
    (ui->stackedWidget)->setCurrentIndex(6);
}
