#include "gyrodisplay.h"
#include <QPainter>

GyroDisplay::GyroDisplay(QWidget *parent)
    : QWidget{parent}, range(32768), isHorizontal(true), value(0)
{

}

void GyroDisplay::setOrientation(OrientationType orient)
{
    isHorizontal = (orient == HORIZONTAL);
}

void GyroDisplay::setValue(int v)
{
    value = v;
    repaint();
}


QSize GyroDisplay::minimumSizeHint() const
{
    return QSize(24, 24);
}

void GyroDisplay::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    QPen pen;
    QBrush brush;
    if (!isEnabled()) {
        pen.setColor(Qt::lightGray);
        brush.setColor(Qt::white);
        painter.setBrush(brush);
        painter.setPen(pen);
        painter.drawRect(1, 1, width() - 2, height() - 2);
        return;
    }
    pen.setColor(Qt::black);
    painter.drawRect(1, 1, width() - 2, height() - 2);
    int mid;
    int len;
    if (isHorizontal) {
        mid = width() / 2;
        len = (mid * value) / range;
        if (len >= 0) {
            painter.fillRect(mid, 1, len, height() - 3, Qt::darkGreen);
        } else {
            painter.fillRect(mid, 1, len, height() - 3, Qt::red);
        }
    } else {
        mid = height() / 2;
        len = (mid * value) / range;
        if (len >= 0) {
            painter.fillRect(1, mid, width() - 3, -len, Qt::darkGreen);
        } else {
            painter.fillRect(1, mid, width() - 3, -len, Qt::red);
        }
    }
}
