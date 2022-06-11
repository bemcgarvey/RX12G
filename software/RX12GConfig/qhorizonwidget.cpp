#include "qhorizonwidget.h"
#include <QPainter>

QHorizonWidget::QHorizonWidget(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    currentRoll = currentPitch = 0;
    gaugeOutline.load(":/images/Gauge.png");
}

QHorizonWidget::~QHorizonWidget() {

}

void QHorizonWidget::setPitch(int newPitch) {
    currentPitch = newPitch;
    update();
}

void QHorizonWidget::setRoll(int newRoll) {
    currentRoll = newRoll;
    update();
}


QSize QHorizonWidget::sizeHint() const {
    QSize size(400, 400);
    return size;
}

void QHorizonWidget::paintEvent(QPaintEvent *) {
    const int radius = 148;
    const int diameter = 296;
    const QPoint center(200, 198);
    QRegion clip(-radius, -radius, diameter, diameter, QRegion::Ellipse);

    QPainter painter(this);
    if (this->isEnabled()) {
        painter.setOpacity(1.0);
    } else {
        painter.setOpacity(0.1);
    }
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(0, 0, gaugeOutline);
    QTransform transform;
    transform.translate(center.x(), center.y());
    painter.setTransform(transform);
    painter.setClipRegion(clip);
    painter.save();
    transform.rotate(currentRoll);
    painter.setTransform(transform);
    painter.save();
    transform.translate(0, (currentPitch * radius) / 90);
    painter.setTransform(transform);
    QBrush brush(QColor(145, 185, 255, 255));
    painter.setBrush(brush);
    painter.drawRect(QRect(QPoint(-diameter, -diameter), QPoint(diameter, 0)));
    brush.setColor(QColor(175, 135, 50, 255));
    painter.setBrush(brush);
    painter.drawRect(QRect(QPoint(-diameter, 0), QPoint(diameter, diameter)));
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::white);
    painter.setPen(pen);
    painter.drawLine(-diameter, 0, diameter, 0);
    pen.setColor(Qt::yellow);
    pen.setWidth(1);
    painter.setPen(pen);
    QString str;
    for (int i = 10; i <= 90; i += 10) {
        double p = (i * radius) / 90.0;
        if (i % 20 == 0) {
            painter.drawLine(-50, p, 50, p);
            painter.drawLine(-50, -p, 50, -p);
            painter.drawText(-75, p, str.setNum(-i));
            painter.drawText(-70, -p, str.setNum(i));
            painter.drawText(50, p, str.setNum(-i));
            painter.drawText(55, -p, str.setNum(i));
        } else {
            painter.drawLine(-25, p, 25, p);
            painter.drawLine(-25, -p, 25, -p);
        }
    }

    painter.restore();
    pen.setColor(Qt::yellow);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(0, -radius, -5, -radius + 10);
    painter.drawLine(0, -radius, 5, -radius + 10);
    painter.restore();
    pen.setColor(Qt::yellow);
    pen.setWidth(6);
    painter.setPen(pen);
    painter.drawLine(-100, 0, -30, 0);
    painter.drawLine(100, 0, 30, 0);
    painter.drawArc(-30, -30, 60, 60, 0, -16 * 180);
    painter.drawEllipse(QPoint(0, 0), 3, 3);
}

void QHorizonWidget::updateRoll(int newRoll) {
    setRoll(newRoll);
}

void QHorizonWidget::updatePitch(int newPitch) {
    setPitch(newPitch);
}
