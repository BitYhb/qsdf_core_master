#ifndef SCREENDESIGNER_MAINWINDOW_H
#define SCREENDESIGNER_MAINWINDOW_H

#include <QMainWindow>

#include <extensionsystem/pluginspec.h>

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
QT_END_NAMESPACE

using namespace ExtensionSystem;

namespace ScreenDesigner {

class ThumbnailView;
class DiagramView;
class ThumbnailScene;
class DiagramScene;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadTrueSightLibs();

signals:

private:
    void createMenus();
    void createActions();

    ThumbnailView *m_thumbnailView;
    ThumbnailScene *m_thumbnailScene;

    DiagramView *m_diagramView;
    DiagramScene *m_diagramScene;

    QMenu *m_fileMenu;
    QAction *m_quitAction;
    QAction *m_createProjectAction;
};

} // namespace ScreenDesigner

#endif // SCREENDESIGNER_MAINWINDOW_H
