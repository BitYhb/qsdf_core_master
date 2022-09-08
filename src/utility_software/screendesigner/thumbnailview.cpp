#include "thumbnailview.h"
#include "thumbnailscene.h"

#include <QPaintEvent>

namespace ScreenDesigner {

ThumbnailView::ThumbnailView(QWidget *parent /*= nullptr*/)
    : QGraphicsView(parent)
{}

ThumbnailView::ThumbnailView(QGraphicsScene *scene, QWidget *parent /*= nullptr*/)
    : QGraphicsView(scene, parent)
{}

ThumbnailView::~ThumbnailView() {}

} // namespace ScreenDesigner
