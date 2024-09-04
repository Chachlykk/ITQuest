#include "messageform.h"
#include "ui_messageform.h"

messageForm::messageForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::messageForm)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setFocusPolicy(Qt::StrongFocus);
    this->adjustSize();
}

messageForm::~messageForm()
{
    delete ui;
}

void messageForm::setQuestion(QString text)
{
    ui->label->setText(text);
}

void messageForm::on_pushButton_clicked()
{
    this->hide();
}
