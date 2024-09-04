#ifndef PLAYWIDGET_H
#define PLAYWIDGET_H

#include <QFont>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsPixMapItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>

#define MAP_IMG_HEIGHT 720.
#define MAP_IMG_WIDTH 1280.

#define CHARACTER_SIZE 15.0f

namespace Ui {
class PlayWidget;
}
/**
 * @brief The PlayWidget class represents a widget for playing a game.
 *
 * This widget includes UI elements such as labels and graphics scenes
 * for displaying game content and interaction.
 */
class PlayWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a PlayWidget object.
     * @param parent The parent widget (if any).
     */
    explicit PlayWidget(QWidget *parent = nullptr);

    /**
     * @brief Destroys the PlayWidget object.
     */
    ~PlayWidget();

    void resizeEvent(QResizeEvent *event) override {}

    /**
 * @brief Sets the background image for the play widget.
 * 
 * Loads the specified image file as the background image for the widget,
 * converts it to QImage format, and updates the scene. Additionally,
 * draws the character image and adds any necessary hints to the scene.
 * 
 * @param backgroundImageF The file path of the background image to load.
 */
    void setBackgroundImage(QString backgroundImageF);

    /**
 * @brief Sets the x-coordinate of the correct answer position.
 * 
 * Sets the x-coordinate position where the correct answer should be displayed
 * on the widget. This information is typically used to place UI elements or
 * indicators related to the correct answer within the widget's graphical scene.
 * 
 * @param x The x-coordinate value where the correct answer should be positioned.
 */
    void setRightAnswerX(int x);

    /**
 * @brief Sets up answer labels with provided texts and adjusts their positions.
 * 
 * Initializes three answer labels with texts from the provided list `ans`.
 * The labels are positioned based on predefined positions stored in `pos`.
 * The method also adjusts the size of each label to fit its text and centers
 * them horizontally relative to their respective positions on the widget.
 * 
 * @param ans A QStringList containing three strings representing answer texts.
 */
    void makeAnsLabels(const QStringList &ans);

    bool ifPaint;         /**< Flag indicating if painting is enabled. */
    bool enable_keypress; /**< Flag indicating if key press events are enabled. */
signals:
    void openMenu();
    void gameEnd(int mark);
    void callQuestion();

public slots:
    /**
 * @brief Emits a signal to indicate that a question should be called.
 * 
 * This method emits the signal `callQuestion()`, which is typically connected
 * to a slot or function elsewhere in the application. It serves as a trigger
 * to initiate the process of displaying or handling a question-related action.
 * 
 * @note This method assumes that `callQuestion()` signal has been properly defined
 *       and connected to perform specific actions in response to a hint or user interaction.
 */
    void onClickHint(){emit callQuestion();}

private slots:
    /**
 * @brief Handles key press events for controlling the character movement and menu interaction.
 * 
 * This method is overridden from the base QWidget class to handle specific key press events
 * related to controlling the character's movement (up, down, left, right) and menu interaction.
 * 
 * @param event A pointer to the QKeyEvent object containing information about the key press event.
 * 
 * @note The method checks if key presses are enabled (`enable_keypress` is true). If enabled:
 * - W key moves the character up if `enableMove('u')` returns true.
 * - S key moves the character down if `enableMove('d')` returns true.
 * - A key moves the character left if `enableMove('l')` returns true.
 * - D key moves the character right if `enableMove('r')` returns true.
 * - Escape key (Qt::Key_Escape) disables key presses and emits the signal `openMenu()`.
 * - Other key presses are forwarded to the base QWidget implementation for default handling.
 * 
 * @see enable_keypress
 * @see enableMove
 * @see openMenu
 */
    void keyPressEvent(QKeyEvent *event);

private:
    Ui::PlayWidget *ui;    /**< The user interface object for the PlayWidget. */
    QPixmap charImage;     /**< The QPixmap object for loading character image. */
    QGraphicsScene *scene; /**< The QGraphicsScene object for displaying game scenes. */
    QLabel *answer1;       /**< QLabel object for displaying answer 1. */
    QLabel *answer2;       /**< QLabel object for displaying answer 2. */
    QLabel *answer3;       /**< QLabel object for displaying answer 3. */

    QPixmap backgroundImage; ///< The background image of the game scene.
    int rightAnswer;         ///< The X-axis position of the correct answer.
    QPushButton *button;     ///< Pointer to the hint button.
    int mark;                ///< Current score or mark in the game.
    const int pos[3] = {170,
                        650,
                        1110}; ///< Array of X-axis positions where answer labels can be placed.
    QGraphicsPixmapItem *characterItem; ///< The QGraphicsPixmapItem representing the character.

    double width_k;  ///< Scaling factor for width relative to the map image dimensions.
    double height_k; ///< Scaling factor for height relative to the map image dimensions.
    QImage rgbImage; ///< RGB image data for collision detection.

    /**
 * @brief Draws the background scene with the loaded background image.
 * 
 * This method creates a QGraphicsPixmapItem using the loaded background image scaled
 * to fit the size of the widget's graphics view (ui->graphicsView). The item is added
 * to the QGraphicsScene (scene) associated with the widget, setting it as the background
 * for the widget's graphics view.
 * 
 * @note This method is typically called after loading a background image via setBackgroundImage().
 * 
 * @see setBackgroundImage
 */
    void drawScene();

    /**
 * @brief Draws the character (avatar) on the QGraphicsScene.
 * 
 * This method creates a QGraphicsPixmapItem using the loaded character image scaled
 * to a size relative to the widget's dimensions. The character item is positioned on
 * the QGraphicsScene (scene) at a calculated initial position.
 * 
 * @note This method is typically called after loading a character image and setting up
 *       the background scene in the widget.
 * 
 * @see drawScene, setBackgroundImage
 */
    void drawChar();

    /**
 * @brief Checks if the character can move in the specified direction.
 * 
 * This method determines if the character can move in the given direction based on
 * the color of the pixels surrounding its current position on the background image.
 * It calculates pixel positions in the specified direction and checks the red component
 * of the color at those positions. If the red component is less than 200, indicating
 * an acceptable movement area, the method returns true; otherwise, it returns false.
 * 
 * @param direction The direction in which to check movement ('u' for up, 'd' for down,
 *                  'l' for left, 'r' for right).
 * @return true if movement in the specified direction is enabled (color is acceptable),
 *         false otherwise.
 * 
 * @note This method relies on the current position of the characterItem and the loaded
 *       background image (backgroundImage) with its corresponding rgbImage.
 * 
 * @see drawChar, setBackgroundImage
 */
    bool enableMove(char direction);

    /**
 * @brief Checks if the character reaches the victory point or answers incorrectly.
 *
 * This method checks the position of the character on the game scene to determine
 * if it has reached the victory point or if it has given a wrong answer. If the character's
 * position is within the victory area (y <= 80 pixels from the top and within x_ans - 50
 * to x_ans + 50 pixels horizontally), the game ends successfully. If only the vertical condition
 * is met (y <= 80 pixels from the top) but not the horizontal range, it indicates a wrong answer
 * and decreases the player's mark. In both cases, appropriate signals are emitted to handle game
 * completion or continue gameplay.
 *
 * @param pos The current position of the character item.
 * @param x_ans The x-coordinate of the correct answer point on the game scene.
 *
 * @see enableMove, gameEnd
 */
    void checkVictory(QPointF pos, int x_ans);

    /**
 * @brief Adds a hint button to the game scene.
 *
 * This method creates a QPushButton labeled with "?" and adds it to the QGraphicsScene.
 * The button is styled with a fixed size of 30x30 pixels, a specific font ("Courier New" with size 26),
 * and a background color of RGB(233, 191, 159). The button's position is adjusted relative to the 
 * game scene dimensions (width_k and height_k) to ensure proper placement.
 * 
 * When the hint button is clicked, it emits a signal `onClickHint()` which is connected to a slot in 
 * the parent widget to handle the hint functionality.
 * 
 * @see onClickHint
 */
    void addHint();
};

#endif // PLAYWIDGET_H
