/*
 * Main
 *
 * Copyright (C) 2011 David Edmundson <kde@davidedmundson.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

// KDE
#include <KCmdLineArgs>
#include <KApplication>
#include <KAboutData>
#include <KLocale>
#include <KUrl>
#include <KFileDialog>
#include <KPushButton>
#include <KDebug>

#include "mainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
    KAboutData aboutData("ktp-send-file", 0,
                         ki18n("Telepathy Send File"),
                         KTP_SEND_FILE_VERSION);
    aboutData.addAuthor(ki18n("David Edmundson"), ki18n("Author"), "kde@davidedmundson.co.uk");
    aboutData.setProductName("telepathy/send-file");
    aboutData.setProgramIconName(QLatin1String("telepathy-kde"));

    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("+file", ki18n("The file to send"));
    KCmdLineArgs::addCmdLineOptions(options);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    KApplication app;

    KUrl::List filesToSend;

    if (args->count() == 0) {
        KFileDialog *fileDialog = new KFileDialog(KUrl("kfiledialog://telepathySendFile"), QString(), 0);
	fileDialog->setOperationMode(KFileDialog::Opening);
	fileDialog->setWindowTitle(i18n("Select Files To Send"));
	fileDialog->okButton()->setText(i18n("Send"));
	fileDialog->exec();
	filesToSend = fileDialog->selectedUrls();
	fileDialog->deleteLater();
    } else {
        Q_FOREACH(QString fileToSend, args->allArguments().mid(1)) {
            filesToSend.append(KUrl(fileToSend));
        }
    }

    if (! filesToSend.isEmpty()) {
        MainWindow *w = new MainWindow(filesToSend);
        w->show();
        return app.exec();
    } else {
        return -1;
    }

}

