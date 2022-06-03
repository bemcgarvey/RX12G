#ifndef QHORIZONWIDGET_H
#define QHORIZONWIDGET_H

#include <QObject>
#include <QWidget>

class QHorizonWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int pitch READ pitch WRITE setPitch)
    Q_PROPERTY(int roll READ roll WRITE setRoll)
public:
    explicit QHorizonWidget(QWidget *parent = 0);
    ~QHorizonWidget();
    int pitch() const {return currentPitch;}
    int roll() const {return currentRoll;}
    void setPitch(int newPitch);
    void setRoll(int newRoll);
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void updateRoll(int newRoll);
    void updatePitch(int newPitch);
private:
    int currentPitch;
    int currentRoll;
    QPixmap gaugeOutline;
};

#endif // QHORIZONWIDGET_H
