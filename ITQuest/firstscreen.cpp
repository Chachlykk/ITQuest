#include "firstscreen.h"
#include <QApplication>
#include <QPainter>

FirstScreen::FirstScreen(const QPixmap &pixmap, int duration, QWidget *parent)
    : QSplashScreen(pixmap)
    , m_originalPixmap(pixmap)
    , m_angle(0)
    , m_duration(duration)
{
    annimationFinished = false;
    setFixedSize(400, 400);
    setParent(parent);
    m_timer.setInterval(30);
    connect(&m_timer, &QTimer::timeout, this, &FirstScreen::updatePixmap);

    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    QRect availableGeometry = primaryScreen->availableGeometry();

    int x = (availableGeometry.width() - 400) / 2;
    int y = (availableGeometry.height() - 400) / 2;
    move(x, y);
}

void FirstScreen::startAnimation()
{
    m_timer.start();
    QTimer::singleShot(m_duration, this, [this]() {
        m_timer.stop();
        emit finished();
    });
    QTimer::singleShot(m_duration - 1000, this, [this]() { annimationFinished = true; });
}

void FirstScreen::drawContents(QPainter *painter)
{
    painter->fillRect(rect(), Qt::transparent);

    painter->drawPixmap((width() - m_rotatedPixmap.width()) / 2,
                        (height() - m_rotatedPixmap.height()) / 2,
                        m_rotatedPixmap);
}

void FirstScreen::updatePixmap()
{
    m_angle += 2.4;
    if (m_angle >= 360) {
        m_angle -= 360;
    }

    QPixmap rotatedPixmap(m_originalPixmap.size());
    rotatedPixmap.fill(Qt::transparent);

    QPainter painter(&rotatedPixmap);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.translate(m_originalPixmap.width() / 2, m_originalPixmap.height() / 2);
    painter.rotate(m_angle);
    painter.translate(-m_originalPixmap.width() / 2, -m_originalPixmap.height() / 2);
    painter.drawPixmap(0, 0, m_originalPixmap);
    painter.end();

    m_rotatedPixmap = rotatedPixmap;
    if (!annimationFinished)
        repaint();
}
