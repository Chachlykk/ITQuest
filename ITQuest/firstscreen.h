#ifndef FIRSTSCREEN_H
#define FIRSTSCREEN_H

#include <QPixmap>
#include <QScreen>
#include <QSplashScreen>
#include <QTimer>

class FirstScreen : public QSplashScreen
{
    Q_OBJECT

public:
    /**
 * @brief Constructs a FirstScreen object.
 * 
 * This constructor initializes a splash screen with the given pixmap, sets its duration, 
 * and positions it in the center of the primary screen.
 * 
 * @param pixmap The pixmap to be displayed in the splash screen.
 * @param duration The duration for which the splash screen will be displayed.
 * @param parent The parent widget.
 */
    FirstScreen(const QPixmap &pixmap, int duration, QWidget *parent = nullptr);

    /**
 * @brief Starts the splash screen animation.
 * 
 * This method starts the timer for the animation, schedules a stop for the timer after the specified duration,
 * and emits a `finished` signal when the animation is complete. It also sets `annimationFinished` to `true` 
 * shortly before the animation stops.
 */
    void startAnimation();

signals:
    /**
 * @brief Signal emitted when the animation or processing is finished.
 * 
 * This signal is emitted to indicate that the animation or processing
 * managed by the class has completed its execution.
 * 
 * Connect to this signal to receive notifications when the FirstScreen
 * object has completed its intended operation, such as finishing an animation.
 * 
 * @note This signal is typically used to synchronize with the completion
 *       of tasks or animations performed by the FirstScreen widget.
 */
    void finished();

protected:
    /**
 * @brief Overridden method to draw the contents of the splash screen.
 * 
 * @param painter A pointer to the QPainter object used for drawing.
 * 
 * This method clears the screen with a transparent fill, then draws the rotated pixmap at the center of the widget.
 * The rotation angle is determined by the member variable `m_angle`.
 */
    void drawContents(QPainter *painter) override;

private slots:
    /**
 * @brief Updates the pixmap with a rotated version and triggers a repaint if animation is ongoing.
 * 
 * This method rotates the original pixmap by a certain angle (`m_angle`) on each update.
 * It uses smooth pixmap transformation and centers the rotation at the pixmap's center.
 * The rotated pixmap is stored in `m_rotatedPixmap` and used for drawing.
 * If the animation is not finished (`annimationFinished` is false), it triggers a repaint of the widget.
 */
    void updatePixmap();

private:
    QTimer m_timer;           /**< Timer for controlling animation updates. */
    QPixmap m_originalPixmap; /**< Original pixmap used for animation. */
    QPixmap m_rotatedPixmap;  /**< Rotated pixmap based on current angle. */
    qreal m_angle;            /**< Current rotation angle for the pixmap. */
    int m_duration;           /**< Duration of the animation in milliseconds. */
    bool annimationFinished;  /**< Flag indicating if animation has finished. */
};

#endif // FIRSTSCREEN_H
