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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <KFileItem>
#include <KPixmapSequence>
#include <KPixmapSequenceOverlayPainter>
#include <KLocalizedString>
#include <KIconLoader>
#include <KIO/PreviewJob>

#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/PendingReady>

#include <KTp/actions.h>
#include <KTp/contact-factory.h>
#include <KTp/Models/contacts-list-model.h>
#include <KTp/Models/contacts-filter-model.h>
#include <KTp/Widgets/contact-grid-widget.h>

MainWindow::MainWindow(const QList<QUrl> &urls, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_urls(urls),
    m_contactsModel(0)
{
    Tp::registerTypes();

    ui->setupUi(this);
    if (urls.size() == 1) {
        setWindowTitle(i18n("Send file - %1", urls.first().fileName()));

        ui->filesInfoLabel->hide();
        ui->fileNameLabel->setText(urls.first().fileName());
    } else {
        QString fileNames;
        Q_FOREACH(const QUrl &file, urls) {
            fileNames += file.fileName() + " ";
        }
        setWindowTitle(i18n("Send files - %1", fileNames.trimmed()));

        ui->messageLabel->setText(i18n("You are about to send these files"));
        ui->filesInfoLabel->setText(i18np("1 file selected", "%1 files selected", urls.count()));
        ui->fileNameLabel->setText(fileNames.replace(" ", "<br />"));
    }

    m_busyOverlay = new KPixmapSequenceOverlayPainter(this);
    m_busyOverlay->setSequence(KPixmapSequence("process-working", 22));
    m_busyOverlay->setWidget(ui->filePreview);
    m_busyOverlay->start();

    if (urls.size() == 1) {
        KFileItem file(urls.first());
        QStringList availablePlugins = KIO::PreviewJob::availablePlugins();
        KIO::PreviewJob* job = KIO::filePreview(KFileItemList() << file, QSize(280, 280), &availablePlugins);
        job->setOverlayIconAlpha(0);
        job->setScaleType(KIO::PreviewJob::Unscaled);
        connect(job, SIGNAL(gotPreview(KFileItem, QPixmap)),
                this, SLOT(onPreviewLoaded(KFileItem, QPixmap)));
        connect(job, SIGNAL(failed(KFileItem)),
                this, SLOT(onPreviewFailed(KFileItem)));
    } else {
        ui->filePreview->setPixmap(QPixmap(DesktopIcon("dialog-information.png", 128)));
        m_busyOverlay->stop();
    }

    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureAvatar
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile
                                                                       << Tp::Account::FeatureCapabilities);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureRosterGroups
                                                                               << Tp::Connection::FeatureRoster
                                                                               << Tp::Connection::FeatureSelfContact);

    Tp::ContactFactoryPtr contactFactory = KTp::ContactFactory::create(Tp::Features()  << Tp::Contact::FeatureAlias
                                                                      << Tp::Contact::FeatureAvatarData
                                                                      << Tp::Contact::FeatureSimplePresence
                                                                      << Tp::Contact::FeatureCapabilities);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    m_accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory,
                                                  contactFactory);

    m_contactsModel = new KTp::ContactsListModel(this);
    connect(m_accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), SLOT(onAccountManagerReady()));


    m_contactGridWidget = new KTp::ContactGridWidget(m_contactsModel, this);
    m_contactGridWidget->contactFilterLineEdit()->setPlaceholderText(i18n("Search in Contacts..."));
    m_contactGridWidget->filter()->setPresenceTypeFilterFlags(KTp::ContactsFilterModel::ShowOnlyConnected);
    m_contactGridWidget->filter()->setCapabilityFilterFlags(KTp::ContactsFilterModel::FilterByFileTransferCapability);
    ui->recipientVLayout->addWidget(m_contactGridWidget);

    connect(m_contactGridWidget,
            SIGNAL(selectionChanged(Tp::AccountPtr,KTp::ContactPtr)),
            SLOT(onContactSelectionChanged(Tp::AccountPtr,KTp::ContactPtr)));

    ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(true);
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onDialogAccepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAccountManagerReady()
{
    m_contactsModel->setAccountManager(m_accountManager);
}

void MainWindow::onDialogAccepted()
{
    // don't do anytghing if no contact has been selected
    if (!m_contactGridWidget->hasSelection()) {
        // show message box?
        return;
    }

    // start sending file
    Q_FOREACH(const QUrl &url, m_urls) {
        Tp::PendingOperation* channelRequest = KTp::Actions::startFileTransfer(m_contactGridWidget->selectedAccount(),
                                                                               m_contactGridWidget->selectedContact(),
                                                                               url);

        connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), SLOT(slotFileTransferFinished(Tp::PendingOperation*)));
    }

    //disable the buttons
    foreach(QAbstractButton* button, ui->buttonBox->buttons()) {
        button->setEnabled(false);
    }
}

void MainWindow::onContactSelectionChanged(Tp::AccountPtr account, KTp::ContactPtr contact)
{
    Q_UNUSED(account)
    Q_UNUSED(contact)

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_contactGridWidget->hasSelection());
}

void MainWindow::slotFileTransferFinished(Tp::PendingOperation* op)
{
    if (op->isError()) {
        //FIXME map to human readable strings.
        QString errorMsg(op->errorName() + ": " + op->errorMessage());
        qWarning() << "ERROR!: " << errorMsg;
        QMessageBox::warning(this, i18n("Failed to send file"), i18n("File Transfer Failed"));
        close();
    } else {
        // now I can close the dialog
        close();
    }
}

void MainWindow::onPreviewLoaded(const KFileItem& item, const QPixmap& preview)
{
    Q_UNUSED(item);
    ui->filePreview->setPixmap(preview);
    m_busyOverlay->stop();
}

void MainWindow::onPreviewFailed(const KFileItem& item)
{
    qWarning() << "Loading thumb failed" << item.name();
    ui->filePreview->setPixmap(KIconLoader::global()->loadIcon(item.iconName(), KIconLoader::Desktop, 128));
    m_busyOverlay->stop();
}
