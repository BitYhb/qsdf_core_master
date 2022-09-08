#ifndef SCREENDESIGNER_DIAGRAMSCENE_H
#define SCREENDESIGNER_DIAGRAMSCENE_H

#include <QGraphicsScene>

namespace ScreenDesigner {

class DiagramScene : public QGraphicsScene
{
public:
    DiagramScene(QObject *parent = nullptr);
    ~DiagramScene();
};

} // namespace ScreenDesigner

#endif //SCREENDESIGNER_DIAGRAMSCENE_H
