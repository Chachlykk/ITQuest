#include "addclass.h"
#include "ui_addclass.h"
#include <iostream>

AddClass::AddClass(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddClass)
{
    ui->setupUi(this);
}

AddClass::~AddClass()
{
    delete ui;
}

void AddClass::on_pushButton_2_clicked()
{
    this->hide();
}


void AddClass::on_pushButton_clicked()
{
    QString className = ui->lineEdit->text();
    QString toServer = "{\"signal\":\"addClass\",\"teacherID\":" + QString::number(currentUser.id)
                       + ",\"className\":\"" + className + "\"}";
    emit sendToServer(toServer);
    // emit add_class(ui->lineEdit->text(),classCode);
    this->close();
}
