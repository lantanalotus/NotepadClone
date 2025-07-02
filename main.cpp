#include "mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QSysInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "WNotepad_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    QString userName =
    #if defined(Q_OS_WIN)
        qEnvironmentVariable("USERNAME");
    #else
        qEnvironmentVariable("USER");
    #endif

    QCoreApplication::setOrganizationName(userName);

    QCoreApplication::setApplicationName("Notepad");

    MainWindow window{};

    window.resize(600, 400);
    window.show();
    return app.exec();
}
