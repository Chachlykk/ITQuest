#include "addlevel.h"
#include "ui_addlevel.h"

addLevel::addLevel(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addLevel)
{
    ui->setupUi(this);
    QComboBox *comboBox = new QComboBox();
    comboBox->addItems(QStringList() << "1"
                                     << "2"
                                     << "3");
    ui->tableWidget_2->setCellWidget(0, 4, comboBox);
}

addLevel::~addLevel()
{
    delete ui;
}

void addLevel::on_pushButton_2_clicked()
{
    this->hide();
}

void addLevel::on_pushButton_clicked()
{
    QStringList rowData;
    int row = ui->tableWidget_2->currentRow();
    // Получение данных из таблицы
    int columnCount = ui->tableWidget_2->columnCount();
    for (int i = 0; i < columnCount - 1; ++i) {
        QTableWidgetItem *item = ui->tableWidget_2->item(row, i);
        if (item)
            rowData << item->text();
        else
            rowData << "";
    }
    QWidget *widget = ui->tableWidget_2->cellWidget(row, columnCount - 1);
    QComboBox *comboBox = qobject_cast<QComboBox *>(widget);
    rowData << comboBox->currentText();
    this->close();
    emit addLevelMain(rowData);
}
