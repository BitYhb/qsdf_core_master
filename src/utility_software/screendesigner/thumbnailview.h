#ifndef THUMBNAILVIEW_H
#define THUMBNAILVIEW_H

#include <QGraphicsView>

namespace ScreenDesigner {

class ThumbnailView : public QGraphicsView
{
public:
    ThumbnailView(QWidget *parent = nullptr);
    ThumbnailView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~ThumbnailView();
};

} // namespace ScreenDesigner

#endif // THUMBNAILVIEW_H
