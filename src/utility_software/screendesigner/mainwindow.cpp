#include "mainwindow.h"
#include "diagramscene.h"
#include "diagramview.h"
#include "thumbnailscene.h"
#include "thumbnailview.h"

#include <extensionsystem/iplugin.h>
#include <utils/minisplitter.h>

#include <QHBoxLayout>
#include <QMenuBar>
#include <QFile>

using namespace Utils;

namespace ScreenDesigner {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}
{
    createActions();
    createMenus();

    m_diagramScene = new DiagramScene;
    m_thumbnailScene = new ThumbnailScene;

    m_diagramView = new DiagramView;
    m_thumbnailView = new ThumbnailView(m_thumbnailScene, nullptr);
    m_thumbnailView->setFixedWidth(300);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_thumbnailView);
    mainLayout->addWidget(m_diagramView);

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);
    setWindowTitle(tr("ScreenDesigner"));
}

MainWindow::~MainWindow() {}

void MainWindow::loadTrueSightLibs()
{
    QFile xmlFile("");
}

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_createProjectAction);
    m_fileMenu->addAction(m_quitAction);
}

void MainWindow::createActions()
{
    m_quitAction = new QAction(tr("E&xit"), this);
    m_quitAction->setStatusTip(tr("Quit ScreenDesigner"));
    m_quitAction->setShortcuts(QKeySequence::Quit);
    connect(m_quitAction, &QAction::triggered, this, &QMainWindow::close);

    m_createProjectAction = new QAction(tr("N&ew Project"), this);
    m_createProjectAction->setStatusTip(tr("New Project"));
    m_createProjectAction->setShortcut(tr("Ctrl+N"));
}

} // namespace ScreenDesigner
