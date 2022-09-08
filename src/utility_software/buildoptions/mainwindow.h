#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

QT_BEGIN_NAMESPACE
class QFileDialog;
QT_END_NAMESPACE

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void on_sourcePathSelectBtn_clicked();
    void on_buildPathSelectBtn_clicked();
    void on_configureBtn_clicked();

private:
    Ui::MainWindow *m_ui;
    QFileDialog *m_sourceFolderSelectDialog = nullptr;
    QFileDialog *m_buildFolderSelectDialog = nullptr;
};

#endif // MAINWINDOW_H
