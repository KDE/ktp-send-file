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
#include <KApplication>
#include <KMimeType>
#include <KDebug>
#include <KMessageBox>
#include <KPixmapSequence>
#include <KPixmapSequenceOverlayPainter>
#include <KLineEdit>
#include <KIO/PreviewJob>

#include <QtGui/QPushButton>

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/PendingReady>

#include <KTp/Models/accounts-model.h>
#include <KTp/Models/accounts-filter-model.h>
#include <KTp/Widgets/contact-grid-widget.h>

//FIXME, copy and paste the approver code for loading this from a config file into this, the contact list and the chat handler.
#define PREFERRED_FILETRANSFER_HANDLER "org.freedesktop.Telepathy.Client.KTp.FileTransfer"


MainWindow::MainWindow(const KUrl &url, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_url(url),
    m_accountsModel(0)
{
    Tp::registerTypes();

    ui->setupUi(this);
    setWindowTitle(i18n("Send file - %1", url.fileName()));

    kDebug() << KApplication::arguments();

    ui->fileNameLabel->setText(url.fileName());
    m_busyOverlay = new KPixmapSequenceOverlayPainter(this);
    m_busyOverlay->setSequence(KPixmapSequence("process-working", 22));
    m_busyOverlay->setWidget(ui->filePreview);
    m_busyOverlay->start();

    KFileItem file(KFileItem::Unknown, KFileItem::Unknown, url);
    QStringList availablePlugins = KIO::PreviewJob::availablePlugins();
    KIO::PreviewJob* job = KIO::filePreview(KFileItemList() << file, QSize(280, 280), &availablePlugins);
    job->setOverlayIconAlpha(0);
    job->setScaleType(KIO::PreviewJob::Unscaled);
    connect(job, SIGNAL(gotPreview(KFileItem,QPixmap)),
            this, SLOT(onPreviewLoaded(KFileItem,QPixmap)));
    connect(job, SIGNAL(failed(KFileItem)),
            this, SLOT(onPreviewFailed(KFileItem)));


    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureAvatar
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureRosterGroups
                                                                               << Tp::Connection::FeatureRoster
                                                                               << Tp::Connection::FeatureSelfContact);

    Tp::ContactFactoryPtr contactFactory = Tp::ContactFactory::create(Tp::Features()  << Tp::Contact::FeatureAlias
                                                                      << Tp::Contact::FeatureAvatarData
                                                                      << Tp::Contact::FeatureSimplePresence
                                                                      << Tp::Contact::FeatureCapabilities);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    m_accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory,
                                                  contactFactory);

    m_accountsModel = new AccountsModel(this);
    connect(m_accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), SLOT(onAccountManagerReady()));


    m_contactGridWidget = new KTp::ContactGridWidget(m_accountsModel, this);
    m_contactGridWidget->contactFilterLineEdit()->setClickMessage(i18n("Search in Contacts..."));
    m_contactGridWidget->filter()->setPresenceTypeFilterFlags(AccountsFilterModel::ShowOnlyConnected);
    m_contactGridWidget->filter()->setCapabilityFilterFlags(AccountsFilterModel::FilterByFileTransferCapability);
    ui->recipientVLayout->addWidget(m_contactGridWidget);

    connect(m_contactGridWidget,
            SIGNAL(selectionChanged(Tp::AccountPtr,Tp::ContactPtr)),
            SLOT(onContactSelectionChanged(Tp::AccountPtr,Tp::ContactPtr)));

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
    m_accountsModel->setAccountManager(m_accountManager);
}

void MainWindow::onDialogAccepted()
{
    // don't do anytghing if no contact has been selected
    if (!m_contactGridWidget->hasSelection()) {
        // show message box?
        return;
    }

    Tp::ContactPtr contact = m_contactGridWidget->selectedContact();
    Tp::AccountPtr sendingAccount = m_contactGridWidget->selectedAccount();

    if (sendingAccount.isNull()) {
        kDebug() << "sending account: NULL";
    } else {
        kDebug() << "Account is: " << sendingAccount->displayName();
        kDebug() << "sending to: " << contact->alias();
    }

    // start sending file
    kDebug() << "FILE TO SEND: " << m_url.path();
    Tp::FileTransferChannelCreationProperties fileTransferProperties(m_url.path(), KMimeType::findByFileContent(m_url.path())->name());

    Tp::PendingChannelRequest* channelRequest = sendingAccount->createFileTransfer(contact,
                                                                                   fileTransferProperties,
                                                                                   QDateTime::currentDateTime(),
                                                                                   PREFERRED_FILETRANSFER_HANDLER);

    connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), SLOT(slotFileTransferFinished(Tp::PendingOperation*)));

    //disable the buttons
    foreach(QAbstractButton* button, ui->buttonBox->buttons()) {
        button->setEnabled(false);
    }
}

void MainWindow::onContactSelectionChanged(Tp::AccountPtr account, Tp::ContactPtr contact)
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
        kDebug() << "ERROR!: " << errorMsg;
        KMessageBox::error(this, i18n("Failed to send file"), i18n("File Transfer Failed"));
        close();
    } else {
        kDebug() << "Transfer started";
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
    kWarning() << "Loading thumb failed" << item.name();
    ui->filePreview->setPixmap(KIconLoader::global()->loadIcon(item.iconName(), KIconLoader::Desktop, 128));
    m_busyOverlay->stop();
}
