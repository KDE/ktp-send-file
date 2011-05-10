#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <KFileItem>
#include <KFileItemList>
#include <KIO/PreviewJob>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    KFileItem file(KUrl("/home/david/a.png"), "image/png", KFileItem::Unknown);

    KIO::PreviewJob* job = KIO::filePreview(KFileItemList() << file, ui->filePreviewLabel->size());

    ui->fileNameLabel->setText(file.name());
    ui->filePreviewLabel->setText(QString());

    connect(job, SIGNAL(gotPreview(KFileItem, QPixmap)),
            this, SLOT(showPreview(KFileItem, QPixmap)));
    connect(job, SIGNAL(failed(KFileItem)),
            this, SLOT(showIcon(KFileItem)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showPreview(const KFileItem &file, const QPixmap &pixmap)
{
    ui->filePreviewLabel->setMinimumSize(pixmap.size());
    ui->filePreviewLabel->setPixmap(pixmap);
}

void MainWindow::showIcon(const KFileItem &file)
{
    //icon is     file.iconName();

}
