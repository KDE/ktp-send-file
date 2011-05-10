// KDE
#include <KCmdLineArgs>
#include <KApplication>
#include <KAboutData>
#include <KLocale>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("mainwindow", 0, ki18n("MainWindow"),
                         "0.1", ki18n("Description here"),
                         KAboutData::License_GPL,
                         ki18n("(c) KDE"),
                         KLocalizedString(), "", "kde-devel@kde.org");

    aboutData.addAuthor(ki18n("David Edmundson"), ki18n("Author"), "kde@davidedmundson.co.uk");

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs::parsedArgs();
    KApplication app;

    MainWindow *w = new MainWindow();
    w->show();

    return app.exec();
}

