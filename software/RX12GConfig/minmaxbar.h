#ifndef MINMAXBAR_H
#define MINMAXBAR_H

#include <QWidget>

class MinMaxBar : public QWidget
{
    Q_OBJECT
public:
    explicit MinMaxBar(QWidget *parent = nullptr);
    int getMinValue() {return minValue;}
    int getMaxValue() {return maxValue;}
    void reset();
    void setValue(int value);
    void setLimit(int newLimit);
    int getLimit() {return limit;}
    void setInitialMinMax(int min, int max);
private:
    int minValue;
    int maxValue;
    int limit;
    int oldMin;
    int oldMax;
signals:


// QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
public:
    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;
};

#endif // MINMAXBAR_H
