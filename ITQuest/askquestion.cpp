#include "askquestion.h"
#include "ui_askquestion.h"

AskQuestion::AskQuestion(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AskQuestion)
{
    ui->setupUi(this);
}

AskQuestion::~AskQuestion()
{
    delete ui;
}

void AskQuestion::on_pushButton_clicked()
{
    QString question = ui->textEdit->toPlainText();
    this->close();
    emit sendQuestion(question);
}
