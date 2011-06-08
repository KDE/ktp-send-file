#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include <TelepathyQt4/AccountManager>


namespace Ui {
    class MainWindow;
}

class KFileItem;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onAccountManagerReady();


private:
    Ui::MainWindow *ui;
    Tp::AccountManagerPtr m_accountManager;
};

#endif // MAINWINDOW_H
