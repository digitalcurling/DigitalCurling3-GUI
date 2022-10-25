#include "main_window.hpp"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "model.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // QFont f = a.font();
    // f.setPointSize(11);
    // a.setFont(f);

    QCoreApplication::setOrganizationName("UEC Takeshi Ito Laboratory");
    QCoreApplication::setOrganizationDomain("minerva.cs.uec.ac.jp");
    QCoreApplication::setApplicationName("DigitalCurling3 GUI");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "digitalcurling3_gui_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    digitalcurling3_gui::Model model;  // singleton instance

    digitalcurling3_gui::MainWindow w;
    w.show();
    return a.exec();
}
