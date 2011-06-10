#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include <TelepathyQt4/AccountManager>


namespace Ui {
    class MainWindow;
}

class AccountsModel;
class KFileItem;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onAccountManagerReady();
    void onDialogAccepted();

    void slotFileTransferFinished(Tp::PendingOperation *op);

private:
    Ui::MainWindow *ui;
    AccountsModel *m_accountsModel;
    Tp::AccountManagerPtr m_accountManager;
};

#endif // MAINWINDOW_H
