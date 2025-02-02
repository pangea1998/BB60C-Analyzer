#ifndef WATERFALLPLOT_H
#define WATERFALLPLOT_H

#include <QWidget>
#include <QImage>
#include <QVector>
#include <QColor>
#include <QPoint>
#include <memory>

class WaterfallPlot : public QWidget
{
    Q_OBJECT
public:
    explicit WaterfallPlot(QWidget *parent = nullptr);
    ~WaterfallPlot() override;

    // Veri ekleme ve görüntüleme
    void addData(const QVector<double>& spectrum);
    void setFrequencyRange(double startFreq, double stopFreq);
    void setAmplitudeRange(double minAmp, double maxAmp);
    void setColorMap(const QVector<QRgb>& colorMap);

    // Görüntüleme ayarları
    void setHistorySize(int size);
    void setScrollSpeed(int linesPerUpdate);
    void setTimePerLine(int ms);
    void clear();
    void setMinLevel(double level);
    void setMaxLevel(double level);
    void setTimeSpan(int seconds);

    // Durum sorgulama
    int historySize() const { return maxHistory; }
    double startFrequency() const { return startFreq; }
    double stopFrequency() const { return stopFreq; }
    double minAmplitude() const { return minAmp; }
    double maxAmplitude() const { return maxAmp; }

public slots:
    // Yeni veri geldiğinde çağrılır
    void updatePlot(const QVector<double>& frequencies,
                   const QVector<double>& amplitudes);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    // Görüntüleme parametreleri
    QImage buffer;
    QVector<QVector<double>> history;
    QVector<QRgb> colorMap;
    int maxHistory;
    int scrollSpeed;
    int timePerLine;

    // Frekans ve genlik aralıkları
    double startFreq;
    double stopFreq;
    double minAmp;
    double maxAmp;

    // Yakınlaştırma ve kaydırma
    double zoomLevel;
    QPoint lastMousePos;
    bool isDragging;

    // Yardımcı fonksiyonlar
    void updateBuffer();
    QRgb amplitudeToColor(double amplitude) const;
    void createDefaultColorMap();
    QPoint dataToScreen(double freq, double time);
    void screenToData(const QPoint& pos, double& freq, double& time);
    void updateScrollRegion();

    // Fare etkileşimi
    void handleZoom(const QPoint& pos, int delta);
    void handlePan(const QPoint& pos);
    void emitCursorData(const QPoint& pos);

    // Yeni eklenen fonksiyonlar
    void initializeColorMap();
    void scrollImage();
    void updateRow(const QVector<double>& amplitudes);
    void createImage();

signals:
    void frequencyAtCursor(double freq);
    void timeAtCursor(double time);
    void amplitudeAtCursor(double amp);
};

#endif // WATERFALLPLOT_H
