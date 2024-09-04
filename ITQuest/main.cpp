#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "firstscreen.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ITQuest_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    QPixmap originalPixmap("C:\\Users\\Julia\\Downloads\\89354451.jpg");
    QPixmap pixmap = originalPixmap.scaled(400, 400);
    //Создание и отображение анимированной заставки
    FirstScreen splash(pixmap, 6000); // 3000 мс = 3 секунды
    splash.show();
    splash.startAnimation();

    MainWindow w;
    QObject::connect(&splash, &FirstScreen::finished, [&]() {
        w.showMaximized();
        splash.finish(&w);
    });
    return a.exec();
}
