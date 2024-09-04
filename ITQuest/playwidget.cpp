#include "playwidget.h"
#include "ui_playwidget.h"

PlayWidget::PlayWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayWidget)
{
    ui->setupUi(this);
    charImage.load(":/map/character.png");
    scene = new QGraphicsScene;
    ifPaint=true;
    enable_keypress = false;
    answer1 = new QLabel("", this);
    answer1->setStyleSheet("QLabel { color : black; font: 15pt; font-weight:700; }");

    answer2 = new QLabel("", this);
    answer2->setStyleSheet("QLabel { color : black; font: 15pt; font-weight:700; }");

    answer3 = new QLabel("", this);
    answer3->setStyleSheet("QLabel { color : black; font: 15pt; font-weight:700;}");
}

PlayWidget::~PlayWidget()
{
    delete ui;
}

void PlayWidget::setBackgroundImage(QString backgroundImageF)
{
    backgroundImage.load(backgroundImageF);
    rgbImage = backgroundImage.toImage();
    drawScene();
    drawChar();
    addHint();
}


void PlayWidget::setRightAnswerX(int x)
{
    rightAnswer=x;
}

void PlayWidget::makeAnsLabels(const QStringList &ans)
{
    answer1->setText(ans[0]);
    answer2->setText(ans[1]);
    answer3->setText(ans[2]);

    answer1->adjustSize();
    answer2->adjustSize();
    answer3->adjustSize();

    answer1->move((this->width() / MAP_IMG_WIDTH) * pos[0] - answer1->width() / 2, 30);
    answer2->move((this->width() / MAP_IMG_WIDTH) * pos[1] - answer2->width() / 2, 30);
    answer3->move((this->width() / MAP_IMG_WIDTH) * pos[2] - answer3->width() / 2, 30);
    mark = 3;
}

void PlayWidget::keyPressEvent(QKeyEvent *event)
{
    if (enable_keypress) {
        QPointF currentPosChar = characterItem->pos();
        if (event->key() == Qt::Key_Escape) {
            enable_keypress = false;
            emit openMenu();
        } else if ((event->key() == Qt::Key_W) && enableMove('u')) {
            characterItem->setPos(
                QPointF(currentPosChar.x(), currentPosChar.y() - (int) (CHARACTER_SIZE * 0.3)));
            checkVictory(characterItem->pos(), rightAnswer * width_k);
        } else if ((event->key() == Qt::Key_S) && enableMove('d')) {
            characterItem->setPos(
                QPointF(currentPosChar.x(), currentPosChar.y() + (int) (CHARACTER_SIZE * 0.3)));
            checkVictory(characterItem->pos(), rightAnswer * width_k);

        } else if ((event->key() == Qt::Key_A) && enableMove('l')) {
            characterItem->setPos(
                QPointF(currentPosChar.x() - (int) (CHARACTER_SIZE * 0.3), currentPosChar.y()));
            checkVictory(characterItem->pos(), rightAnswer * width_k);

        } else if ((event->key() == Qt::Key_D) && enableMove('r')) {
            characterItem->setPos(
                QPointF(currentPosChar.x() + (int) (CHARACTER_SIZE * 0.3), currentPosChar.y()));
            checkVictory(characterItem->pos(), rightAnswer * width_k);
        } else {
            QWidget::keyPressEvent(event); // Вызов базовой реализации для других клавиш
        }
    }
}

void PlayWidget::drawScene()
{
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(backgroundImage.scaled(this->size(), Qt::IgnoreAspectRatio));
    scene->addItem(pixmapItem);
    ui->graphicsView->setScene(scene);
    ifPaint=false;
}

void PlayWidget::drawChar()
{
    int width_sc=this->width();
    int height_sc=this->height();
    width_k=MAP_IMG_WIDTH/width_sc;
    height_k=MAP_IMG_HEIGHT/height_sc;

    characterItem = new QGraphicsPixmapItem(charImage.scaled(
        QSize(this->width()/50,this->height()/30), Qt::KeepAspectRatio));
    characterItem->setPos(QPointF(width_sc/2+width_k*CHARACTER_SIZE, height_sc-height_k*CHARACTER_SIZE*5)); // Установка начальной позиции персонажа
    scene->addItem(characterItem);
}

bool PlayWidget::enableMove(char direction)
{
    QPointF currentPosChar=characterItem->pos();
    QPoint pixelPosUp=QPoint((int)(currentPosChar.x()*width_k+CHARACTER_SIZE / 2), (int)(currentPosChar.y()*height_k-(CHARACTER_SIZE * 0.1)));
    QPoint pixelPosDown=QPoint((int)(currentPosChar.x()*width_k+CHARACTER_SIZE / 2), (int)(currentPosChar.y()*height_k+(CHARACTER_SIZE)*1.7));
    QPoint pixelPosLeft=QPoint((int)(currentPosChar.x()*width_k - (int)(CHARACTER_SIZE * 0.1)), (int)(currentPosChar.y()*height_k+CHARACTER_SIZE/2));
    QPoint pixelPosRight=QPoint((int)(currentPosChar.x()*width_k + (int)(CHARACTER_SIZE * 1.3)), (int)(currentPosChar.y()*height_k +CHARACTER_SIZE / 2));


    return ((direction == 'u' && (rgbImage.pixelColor(pixelPosUp).red() < 200)) ||
            (direction == 'd' && (rgbImage.pixelColor(pixelPosDown).red() < 200)) ||
            (direction == 'l' && (rgbImage.pixelColor(pixelPosLeft).red() < 200)) ||
            (direction == 'r' && (rgbImage.pixelColor(pixelPosRight).red() < 200)));
}

void PlayWidget::checkVictory(QPointF pos, int x_ans)
{
    if (pos.y() <= 80 && (x_ans - 50 < pos.x() && x_ans + 50 > pos.x())) {
        //QMessageBox::information(this, "Победа", "Вы достигли конечной точки! Поздравляем с победой!");
        ifPaint=true;
        enable_keypress = false;
        emit gameEnd(mark);
    } else if (pos.y() <= 80) {
        qDebug() << x_ans << " " << pos.x();
        QMessageBox::information(this,
                                 "Wrong answer",
                                 "Your answer is wrong. The game continues.\n Good luck!");
        mark--;
    }
}

void PlayWidget::addHint()
{
    QPushButton *button = new QPushButton("?");
    button->setFixedSize(30, 30);

    // Устанавливаем шрифт и размер шрифта
    QFont font("Courier New", 26);
    button->setFont(font);
    button->move(30 / width_k, 30 / height_k);
    button->setStyleSheet("background-color: rgb(233, 191, 159);");
    scene->addWidget(button);
    connect(button, &QPushButton::clicked, this, &PlayWidget::onClickHint);
}
