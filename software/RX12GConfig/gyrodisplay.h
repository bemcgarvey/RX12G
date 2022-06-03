#ifndef GYRODISPLAY_H
#define GYRODISPLAY_H

#include <QWidget>

class GyroDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit GyroDisplay(QWidget *parent = nullptr);
    typedef enum {HORIZONTAL, VERTICAL} OrientationType;
    void setOrientation(OrientationType orient);
    void setRange(int r) {range = r;}
public slots:
    void setValue(int v);
signals:

private:
    int range;
    bool isHorizontal;
    int value;

    // QWidget interface
public:
    virtual QSize minimumSizeHint() const override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif // GYRODISPLAY_H
