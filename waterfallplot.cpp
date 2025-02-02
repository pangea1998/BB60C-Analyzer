#include "waterfallplot.h"
#include <QPainter>

WaterfallPlot::WaterfallPlot(QWidget *parent) : QWidget(parent) {
    startFreq = 0;
    stopFreq = 1e9;
    minAmp = -120;
    maxAmp = 0;
    
    // Arkaplan rengini siyah yap
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    
    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::black);
    setPalette(pal);
}

void WaterfallPlot::addData(const QVector<double>& spectrum) {
    // Yeni veriyi geçmişe ekle
    history.prepend(spectrum);
    
    // Maksimum geçmiş boyutunu kontrol et
    while (history.size() > height())
        history.removeLast();
        
    // Görüntüyü güncelle
    updateBuffer();
    update();
}

void WaterfallPlot::setFrequencyRange(double start, double stop) {
    startFreq = start;
    stopFreq = stop;
    update();
}

void WaterfallPlot::setAmplitudeRange(double min, double max) {
    minAmp = min;
    maxAmp = max;
    updateBuffer();
    update();
}

void WaterfallPlot::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawImage(rect(), buffer);
}

void WaterfallPlot::resizeEvent(QResizeEvent *) {
    buffer = QImage(size(), QImage::Format_RGB32);
    updateBuffer();
}

void WaterfallPlot::updateBuffer() {
    buffer.fill(Qt::black);
    
    if (history.isEmpty())
        return;
        
    QPainter painter(&buffer);
    int w = width();
    int h = height();
    
    // Her spektrum satırını çiz
    for (int y = 0; y < history.size() && y < h; ++y) {
        const QVector<double>& spectrum = history[y];
        for (int x = 0; x < spectrum.size() && x < w; ++x) {
            double amp = spectrum[x];
            painter.setPen(amplitudeToColor(amp));
            painter.drawPoint(x, y);
        }
    }
}

QColor WaterfallPlot::amplitudeToColor(double amplitude) {
    // Genliği renk skalasına dönüştür
    double normalized = (amplitude - minAmp) / (maxAmp - minAmp);
    normalized = qBound(0.0, normalized, 1.0);
    
    // Sıcak renk haritası (mavi->kırmızı)
    if (normalized < 0.25)
        return QColor(0, 0, int(255 * normalized * 4));
    else if (normalized < 0.5)
        return QColor(0, int(255 * (normalized - 0.25) * 4), 255);
    else if (normalized < 0.75)
        return QColor(int(255 * (normalized - 0.5) * 4), 255, int(255 * (1 - (normalized - 0.5) * 4)));
    else
        return QColor(255, int(255 * (1 - (normalized - 0.75) * 4)), 0);
} 