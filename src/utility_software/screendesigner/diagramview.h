#ifndef DIAGRAMVIEW_H
#define DIAGRAMVIEW_H

#include <QGraphicsView>

namespace ScreenDesigner {

class DiagramView : public QGraphicsView
{
public:
    DiagramView(QWidget *parent = nullptr);
    ~DiagramView();
};

} // namespace ScreenDesigner

#endif // DIAGRAMVIEW_H
