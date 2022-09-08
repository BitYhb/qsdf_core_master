#include "flowgraphicslayout.h"

namespace Utils {

Utils::FlowGraphicsLayout::FlowGraphicsLayout(QGraphicsLayoutItem *parent /*= nullptr*/)
    : QGraphicsLayout(parent)
{
    QSizePolicy sp = sizePolicy();
    sp.setHeightForWidth(true);
    setSizePolicy(sp);
}

void FlowGraphicsLayout::insertItem(int index, QGraphicsLayoutItem *item)
{
    item->setParentLayoutItem(this);
    if (index > m_items.count() || index < 0)
        index = m_items.count();
    m_items.insert(index, item);
    invalidate();
}

qreal FlowGraphicsLayout::spacing(Qt::Orientation o) const
{
    return m_spacing[int(o) - 1];
}

void FlowGraphicsLayout::setSapcing(Qt::Orientation o, qreal spacing)
{
    if (o & Qt::Horizontal)
        m_spacing[0] = spacing;
    if (o & Qt::Vertical)
        m_spacing[1] = spacing;
}

void FlowGraphicsLayout::setGeometry(const QRectF &rect)
{
    QGraphicsLayout::setGeometry(rect);
    doLayout(rect, true);
}

int FlowGraphicsLayout::count() const
{
    return m_items.count();
}

QGraphicsLayoutItem *FlowGraphicsLayout::itemAt(int index) const
{
    return m_items.at(index);
}

void FlowGraphicsLayout::removeAt(int index)
{
    m_items.removeAt(index);
    invalidate();
}

QSizeF FlowGraphicsLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint /* = QSizeF() */) const
{
    QSizeF sh = constraint;
    switch (which) {
    case Qt::PreferredSize:
        sh = prefSize();
        break;
    case Qt::MinimumSize:
        sh = minSize(constraint);
        break;
    case Qt::MaximumSize:
        sh = maxSize();
        break;
    default:
        break;
    }
    return sh;
}

qreal FlowGraphicsLayout::doLayout(const QRectF &rect, bool applyNewGeometry) const
{
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    const qreal maxw = rect.width() - left - right;

    qreal x = 0;
    qreal y = 0;
    qreal maxRowHeight = 0;
    QSizeF pref;
    for (QGraphicsLayoutItem *item : m_items) {
        pref = item->effectiveSizeHint(Qt::PreferredSize);
        maxRowHeight = qMax(maxRowHeight, pref.height());

        qreal next_x;
        next_x = x + pref.width();
        if (next_x > maxw) {
            if (qFuzzyIsNull(x)) {
                pref.setWidth(maxw);
            } else {
                x = 0;
                next_x = pref.width();
            }
            y += maxRowHeight + spacing(Qt::Vertical);
            maxRowHeight = 0;
        }

        if (applyNewGeometry)
            item->setGeometry(QRectF(QPointF(left + x, top + y), pref));
        x = next_x + spacing(Qt::Horizontal);
    }

    maxRowHeight = qMax(maxRowHeight, pref.height());
    return top + y + maxRowHeight + bottom;
}

QSizeF FlowGraphicsLayout::minSize(const QSizeF &constraint) const
{
    QSizeF size(0, 0);
    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    if (constraint.width() >= 0) { // height for width
        const qreal height = doLayout(QRectF(QPointF(0, 0), constraint), false);
        size = QSizeF(constraint.width(), height);
    } else if (constraint.height() >= 0) { // width for height?
        // not supported
    } else {
        for (const QGraphicsLayoutItem *item : qAsConst(m_items))
            size = size.expandedTo(item->effectiveSizeHint(Qt::MinimumSize));
        size + QSizeF(left + right, top + bottom);
    }
    return size;
}

QSizeF FlowGraphicsLayout::prefSize() const
{
    qreal left, right;
    getContentsMargins(&left, nullptr, &right, nullptr);

    qreal maxh = 0;
    qreal totalWidth = 0;
    for (const QGraphicsLayoutItem *item : qAsConst(m_items)) {
        if (totalWidth > 0)
            totalWidth += spacing(Qt::Horizontal);
        QSizeF pref = item->effectiveSizeHint(Qt::PreferredSize);
        totalWidth += pref.width();
        maxh = qMax(maxh, pref.height());
    }
    maxh += spacing(Qt::Vertical);

    const qreal goldenAspectRatio = 1.61803399;
    qreal w = qSqrt(totalWidth * maxh * goldenAspectRatio) + left + right;
    return minSize(QSizeF(w, -1));
}

QSizeF FlowGraphicsLayout::maxSize() const
{
    qreal totalWidth = 0;
    qreal totalHeight = 0;
    for (const QGraphicsLayoutItem *item : qAsConst(m_items)) {
        if (totalWidth > 0)
            totalWidth += spacing(Qt::Horizontal);
        if (totalHeight > 0)
            totalHeight += spacing(Qt::Vertical);
        QSizeF pref = item->effectiveSizeHint(Qt::PreferredSize);
        totalWidth += pref.width();
        totalHeight += pref.height();
    }

    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    return QSizeF(left + totalWidth + right, top + totalHeight + bottom);
}

} // namespace Utils
