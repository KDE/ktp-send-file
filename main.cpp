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
#include <QFileDialog>
#include <KLocalizedString>
#include <KAboutData>
#include <QApplication>
#include <QPushButton>

#include "mainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("telepathy-kde")));
    KAboutData aboutData("ktp-send-file",
                         i18n("Telepathy Send File"),
                         KTP_SEND_FILE_VERSION);
    aboutData.addAuthor(i18n("David Edmundson"), i18n("Author"), "kde@davidedmundson.co.uk");
    aboutData.setProductName("telepathy/send-file");
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.addPositionalArgument("file", i18n("The files to send"), i18n("[files...]"));
    parser.addHelpOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);


    QList<QUrl> filesToSend;

    if (parser.positionalArguments().isEmpty()) {
        QFileDialog *fileDialog = new QFileDialog(Q_NULLPTR, i18n("Select Files To Send"));
        fileDialog->setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog->exec();
        filesToSend = fileDialog->selectedUrls();
        fileDialog->deleteLater();
    } else {
        Q_FOREACH(const QString &fileToSend, parser.positionalArguments()) {
            filesToSend.append(QUrl::fromUserInput(fileToSend));
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

