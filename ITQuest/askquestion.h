/**
 * @file askquestion.h
 * @brief Declaration of the AskQuestion class and its signals.
 */

#ifndef ASKQUESTION_H
#define ASKQUESTION_H

#include <QDialog>

namespace Ui {
class AskQuestion;
}

/**
 * @class AskQuestion
 * @brief Dialog window for asking a question.
 *
 * This class provides a dialog window for asking a question. It includes functionality
 * to input a question and emit signals for sending the question to the server
 */
class AskQuestion : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for the AskQuestion class.
     * @param parent The parent widget (optional).
     */
    explicit AskQuestion(QWidget *parent = nullptr);

    /**
     * @brief Destructor for the AskQuestion class.
     */
    ~AskQuestion();

private slots:
    /**
 * @brief Slot invoked when the OK button is clicked.
 *
 * Retrieves the text entered in the text edit widget (`ui->textEdit`), emits a signal
 * to send the question to the server (`sendQuestion`), and closes the dialog window.
 */
    void on_pushButton_clicked();

signals:
    /**
     * @brief Signal emitted to send the question to the server.
     * @param question The question entered by the user as a QString.
     */
    void sendQuestion(QString question);

private:
    Ui::AskQuestion *ui;
};

#endif // ASKQUESTION_H
