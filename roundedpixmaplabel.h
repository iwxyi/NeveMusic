#ifndef ROUNDEDLABEL_H
#define ROUNDEDLABEL_H

#include <QLabel>
#include <QPainter>
#include <QPainterPath>

class RoundedPixmapLabel : public QLabel
{
public:
    RoundedPixmapLabel(QWidget* parent = nullptr) : QLabel(parent)
    {
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        const QPixmap* pixmap = this->pixmap();
        if (!pixmap)
            return ;
        QPainter painter(this);
        QPainterPath path;
        path.addRoundedRect(rect(), 4, 4);
        painter.setClipPath(path);
        painter.drawPixmap(rect(), *pixmap);
    }
};

#endif // ROUNDEDLABEL_H
