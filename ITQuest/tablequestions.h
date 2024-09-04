#ifndef TABLEQUESTIONS_H
#define TABLEQUESTIONS_H

#include <QTableWidget>

class tableQuestions: public QTableWidget
{
    Q_OBJECT
public:
    explicit tableQuestions(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event) override;
};

#endif // TABLEQUESTIONS_H
