#include "minmaxbar.h"
#include <QPainter>
#include <QBrush>
#include <QPen>

MinMaxBar::MinMaxBar(QWidget *parent)
    : QWidget{parent}, minValue(-1), maxValue(-1), limit(2047),
      oldMin(0), oldMax(limit)

{

}

void MinMaxBar::reset()
{
    minValue = maxValue = -1;
    update();
}

void MinMaxBar::setValue(int value)
{
    if (value < 0 || value > limit) {
        return;
    }
    if (minValue == -1 || value < minValue) {
        minValue = value;
        update();
    }
    if (value > maxValue) {
        maxValue = value;
        update();
    }
}

void MinMaxBar::setLimit(int newLimit)
{
    if (newLimit < 0) {
        return;
    }
    limit = newLimit;
    update();
}

void MinMaxBar::setInitialMinMax(int min, int max)
{
    if (min > max) {
        return;
    }
    if (min >= 0 && min < limit) {
        oldMin = min;
    }
    if (max > 0 && max <= limit) {
        oldMax = max;
    }
    update();
}


void MinMaxBar::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    QBrush brush(Qt::white);
    QPen pen(Qt::black);
    if (!isEnabled()) {
        pen.setColor(Qt::lightGray);
        painter.setBrush(brush);
        painter.setPen(pen);
        painter.drawRect(0, 0, width() - 1, height() - 1);
        return;
    }
    QFontMetrics fm = painter.fontMetrics();
    QRect bound = fm.boundingRect(QString().number(limit));
    int textWidth = bound.width();
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawRect(0, 0, width() - 1, height() - 1);
    int left;
    int right;
    if (minValue != -1) {
        left = ((width() - 2) * minValue) / limit;
        right = ((width() - 2) * maxValue) / limit;
        int len = right - left;
        if (right - left == 0) {
            len = 1;
        }
        QRect fill(left + 1, 1, len, height() - 2);
        painter.fillRect(fill, Qt::red);
    }
    painter.drawText(5, 0, textWidth, height(), Qt::AlignLeft | Qt::AlignVCenter, QString::number(oldMin));
    painter.drawText(width() - textWidth - 6, 0, textWidth, height(), Qt::AlignLeft | Qt::AlignVCenter, QString::number(oldMax));
    pen.setColor(Qt::blue);
    painter.setPen(pen);
    left = ((width() - 2) * oldMin) / limit;
    right = ((width() - 2) * oldMax) / limit;
    painter.drawLine(1 + left, 1, 1 + left, height() - 1);
    painter.drawLine(right, 1, right, height() - 1);
}


QSize MinMaxBar::minimumSizeHint() const
{
    QSize minSize;
    minSize.setHeight(24);
    minSize.setWidth(200);
    return minSize;
}


QSize MinMaxBar::sizeHint() const
{
    QSize size;
    size.setHeight(24);
    size.setWidth(400);
    return size;
}
