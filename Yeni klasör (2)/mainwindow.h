#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QStatusBar>
#include <QtCore/QTimer>
#include <QtCore/QVector>
#include <QCustomPlot>
#include <complex>
#include "bb_api.h"
#include "waterfallplot.h"
#include "demodulator.h"
#include "analyzer.h"
#include "datamanager.h"

class WaterfallPlot;  // İleri bildirim

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Cihaz kontrol
    void onConnect();
    void onDisconnect();
    void onStartStop();
    void onSingleSweep();
    
    // Ayarlar
    void onCenterFreqChanged(double freq);
    void onSpanChanged(double span);
    void onRBWChanged(int index);
    void onVBWChanged(int index);
    void onRefLevelChanged(double level);
    
    // Marker sistemi
    void onMarkerFreqChanged(double freq);
    void onPeakSearch();
    void onNextPeak();
    void onMarkerToPeak();
    void onDeltaMarkerToggled(bool enabled);
    
    // Trigger
    void onTriggerSourceChanged(int index);
    void onTriggerLevelChanged(double level);
    void onTriggerModeChanged(int index);
    
    // Demodülasyon
    void onDemodTypeChanged(int index);
    void onDemodFreqChanged(double freq);
    void onDemodBWChanged(double bw);
    
    // Veri kaydetme/yükleme
    void onSaveTrace();
    void onLoadTrace();
    void onSaveState();
    void onLoadState();
    void onExportData();
    
    // Analiz araçları
    void onChannelPowerMeasure();
    void onOBWMeasure();
    void onACPRMeasure();
    void onSpurSearch();
    void onPhaseNoiseMeasure();

private:
    // GUI kurulumu
    void setupUI();
    void createToolBar();
    void createDockWindows();
    void createStatusBar();
    void createWaterfallDisplay();
    
    // Spektrum görüntüleme
    QCustomPlot* plotWidget;
    WaterfallPlot* waterfallWidget;
    void setupPlot();
    void updatePlot();
    void updateWaterfall();
    
    // Yan paneller
    QDockWidget* freqDock;
    QDockWidget* markerDock;
    QDockWidget* traceDock;
    QDockWidget* triggerDock;
    QDockWidget* demodDock;
    QDockWidget* measureDock;
    
    // Kontrol bileşenleri
    QDoubleSpinBox* centerFreq;
    QDoubleSpinBox* spanFreq;
    QComboBox* rbwSelect;
    QComboBox* vbwSelect;
    QDoubleSpinBox* refLevel;
    
    // Trigger kontrolleri
    QComboBox* triggerSource;
    QDoubleSpinBox* triggerLevel;
    QComboBox* triggerMode;
    
    // Demodülasyon kontrolleri
    QComboBox* demodType;
    QDoubleSpinBox* demodFreq;
    QDoubleSpinBox* demodBW;
    QSlider* volumeSlider;
    
    // Marker sistemi
    struct Marker {
        bool active = false;
        double frequency = 0.0;
        double amplitude = -120.0;
        QCPItemTracer* tracer = nullptr;
        QCPItemText* label = nullptr;
    };
    std::vector<Marker> markers;
    int activeMarker = 0;
    bool deltaMode = false;
    
    // BB60C cihaz kontrolü
    BbDeviceInterface* device;
    bool isConnected = false;
    bool isRunning = false;
    
    // Veri toplama ve işleme
    QTimer* updateTimer;
    QVector<double> frequencies;
    QVector<double> amplitudes;
    QVector<std::complex<float>> iqData;
    void startAcquisition();
    void stopAcquisition();
    void updateData();
    
    // Analiz fonksiyonları
    double measureChannelPower(double startFreq, double stopFreq);
    double measureOBW(double percentage);
    void measureACPR(double channelBW, double channelSpacing);
    void findSpurs(double threshold);
    void measurePhaseNoise(double carrierFreq, QVector<double>& offsets);
    
    // Veri kaydetme/yükleme
    void saveTraceToFile(const QString& filename);
    void loadTraceFromFile(const QString& filename);
    void exportToCSV(const QString& filename);
    
    // Yardımcı fonksiyonlar
    void updateMeasurements();
    void updateDemodulation();
    void processIQData();
};

#endif // MAINWINDOW_H 