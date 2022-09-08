#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_sourceFolderSelectDialog = new QFileDialog(this);
    m_sourceFolderSelectDialog->setFileMode(QFileDialog::Directory);

    m_buildFolderSelectDialog = new QFileDialog(this);
    m_buildFolderSelectDialog->setFileMode(QFileDialog::Directory);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::on_sourcePathSelectBtn_clicked()
{
    connect(
        m_sourceFolderSelectDialog,
        &QFileDialog::urlSelected,
        this,
        [&](const QUrl &url) { m_ui->sourcePathLineEdit->setText(url.toString().replace("file:///", "")); },
        Qt::SingleShotConnection);
    m_sourceFolderSelectDialog->setWindowTitle("Enter Path to Source");
    m_sourceFolderSelectDialog->open();
}

void MainWindow::on_buildPathSelectBtn_clicked()
{
    connect(
        m_buildFolderSelectDialog,
        &QFileDialog::urlSelected,
        this,
        [&](const QUrl &url) { m_ui->buildPathLineEdit->setText(url.toString().replace("file:///", ""));
        }, Qt::SingleShotConnection);
    m_buildFolderSelectDialog->setWindowTitle("Enter Path to Build");
    m_buildFolderSelectDialog->open();
}

void MainWindow::on_configureBtn_clicked()
{
    
}
