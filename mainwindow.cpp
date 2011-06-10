#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <KFileItem>
#include <KFileItemList>
#include <KIO/PreviewJob>
#include <KApplication>
#include <KCmdLineArgs>
#include <KMimeType>


#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/PendingChannelRequest>
#include <TelepathyQt4/PendingReady>


#include "accounts-model.h"
#include "flat-model-proxy.h"
#include "account-filter-model.h"
#include "contact-model-item.h"

#define PREFERRED_FILETRANSFER_HANDLER "org.freedesktop.Telepathy.Client.KDE.FileTransfer"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_accountsModel(0)
{
    Tp::registerTypes();

    ui->setupUi(this);

    qDebug() << KApplication::arguments();


    KUrl filePath (KCmdLineArgs::parsedArgs()->arg(0));
    ui->filePreview->showPreview(filePath);
    ui->fileNameLabel->setText(filePath.fileName());


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


    connect(m_accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)), SLOT(onAccountManagerReady()));

    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(onDialogAccepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAccountManagerReady()
{
    m_accountsModel = new AccountsModel(m_accountManager, this);
    AccountFilterModel *filterModel = new AccountFilterModel(this);
    filterModel->setSourceModel(m_accountsModel);
    filterModel->filterOfflineUsers(true);
    FlatModelProxy *flatProxyModel = new FlatModelProxy(filterModel);

    ui->listView->setModel(flatProxyModel);
}

void MainWindow::onDialogAccepted()
{
    // don't do anytghing if no contact has been selected
    if (!ui->listView->currentIndex().isValid()) {
        // show message box?
        return;
    }

    ContactModelItem *contactModelItem = ui->listView->currentIndex().data(AccountsModel::ItemRole).value<ContactModelItem*>();
    Tp::ContactPtr contact = contactModelItem->contact();
    Tp::AccountPtr sendingAccount = m_accountsModel->accountForContactItem(contactModelItem);

    if (sendingAccount.isNull()) {
        qDebug("sending account: NULL");
    } else {
        qDebug() << "Account is: " << sendingAccount->displayName();
        qDebug() << "sending to: " << contact->alias();
    }

    // start sending file
    QString filePath (KCmdLineArgs::parsedArgs()->arg(0));
    qDebug() << "FILE TO SEND: " << filePath;

    Tp::FileTransferChannelCreationProperties fileTransferProperties(filePath, KMimeType::findByFileContent(filePath)->name());

    Tp::PendingChannelRequest* channelRequest = sendingAccount->createFileTransfer(contact,
                                                                                   fileTransferProperties,
                                                                                   QDateTime::currentDateTime(),
                                                                                   PREFERRED_FILETRANSFER_HANDLER);

    connect(channelRequest, SIGNAL(finished(Tp::PendingOperation*)), SLOT(slotFileTransferFinished(Tp::PendingOperation*)));
}

void MainWindow::slotFileTransferFinished(Tp::PendingOperation* op)
{
    if (op->isError()) {
        QString errorMsg(op->errorName() + ": " + op->errorMessage());
        qDebug() << "ERROR!: " << errorMsg;
    } else {
        qDebug("FUCK YEAH TRANSFER STARTED");
        // now I can close the dialog
        close();
    }
}


