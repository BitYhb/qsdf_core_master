#ifndef THUMBNAILSCENE_H
#define THUMBNAILSCENE_H

#include <QGraphicsScene>

namespace ScreenDesigner {

class ThumbnailScene : public QGraphicsScene
{
public:
    ThumbnailScene(QObject *parent = nullptr);
    ~ThumbnailScene();
};

} // namespace ScreenDesigner

#endif // THUMBNAILSCENE_H
