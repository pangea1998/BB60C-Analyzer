#ifndef WATERFALLPLOT_H
#define WATERFALLPLOT_H

#include <QWidget>
#include <QImage>
#include <QVector>

class WaterfallPlot : public QWidget {
    Q_OBJECT
public:
    explicit WaterfallPlot(QWidget *parent = nullptr);
    
    void addData(const QVector<double>& spectrum);
    void setFrequencyRange(double start, double stop);
    void setAmplitudeRange(double min, double max);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    
private:
    QImage buffer;
    QVector<QVector<double>> history;
    double startFreq, stopFreq;
    double minAmp, maxAmp;
    
    void updateBuffer();
    QColor amplitudeToColor(double amplitude);
};

#endif 