/*
 * Main Send File Transfer Window
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include <TelepathyQt/AccountManager>
#include <KTp/contact.h>


namespace Ui {
    class MainWindow;
}

namespace KTp {
class ContactGridWidget;
class ContactsListModel;
}

class KFileItem;
class KPixmapSequenceOverlayPainter;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(const QList<QUrl> &urls, QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void onAccountManagerReady();
    void onDialogAccepted();
    void onPreviewLoaded(const KFileItem &item, const QPixmap &preview);
    void onPreviewFailed(const KFileItem &item);
    void onContactSelectionChanged(Tp::AccountPtr account, KTp::ContactPtr contact);
    void slotFileTransferFinished(Tp::PendingOperation *op);

private:
    Ui::MainWindow *ui;
    QList<QUrl> m_urls;
    Tp::AccountManagerPtr m_accountManager;
    KTp::ContactsListModel *m_contactsModel;
    KTp::ContactGridWidget *m_contactGridWidget;
    KPixmapSequenceOverlayPainter *m_busyOverlay;
};

#endif // MAINWINDOW_H
