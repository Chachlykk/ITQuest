#include "tablequestions.h"

tableQuestions::tableQuestions(QWidget *parent) : QTableWidget(parent) {
}

void tableQuestions::resizeEvent(QResizeEvent *event)
{
    int tmp_width=this->width();
    setColumnWidth(1, tmp_width/6);
    setColumnWidth(2, tmp_width/6);
    setColumnWidth(3, tmp_width/6);
    setColumnWidth(4, tmp_width/6);
    setColumnWidth(0, tmp_width-(tmp_width/6)*4-41);
}
