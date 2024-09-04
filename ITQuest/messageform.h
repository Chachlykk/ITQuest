#ifndef MESSAGEFORM_H
#define MESSAGEFORM_H

#include <QDialog>

namespace Ui {
class messageForm;
}

class messageForm : public QDialog
{
    Q_OBJECT

public:
    /**
 * @brief The messageForm class provides a dialog window for displaying messages or questions.
 *
 * This dialog is typically used to show informative messages or questions to the user,
 * allowing them to respond or acknowledge the message.
 */
    explicit messageForm(QWidget *parent = nullptr);

    /**
     * @brief Destroys the messageForm object.
     */
    ~messageForm();

    /**
     * @brief Sets the text to display in the messageForm.
     * @param text The text to display.
     */
    void setQuestion(QString text);

private slots:

    /**
     * @brief Slot function called when the pushButton is clicked.
     *
     * Hides the messageForm dialog when the button is clicked.
     */
    void on_pushButton_clicked();

private:
    Ui::messageForm *ui; /**< The user interface object for the messageForm dialog. */
};

#endif // MESSAGEFORM_H
