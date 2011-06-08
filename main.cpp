// KDE
#include <KCmdLineArgs>
#include <KApplication>
#include <KAboutData>
#include <KLocale>

#include <QDebug>

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
    options.add("+file", ki18n("A required argument 'file'"));
    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    KApplication app;

    if (args->count() != 1) {
        KCmdLineArgs::usageError(i18n("You must supply a file argument"));
    }

    MainWindow *w = new MainWindow();
    w->show();
    return app.exec();

}

