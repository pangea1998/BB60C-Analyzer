#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt Core
#include <QMainWindow>
#include <QTimer>
#include <QVector>

// Qt Widgets
#include <QDockWidget>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QToolBar>
#include <QStatusBar>
#include <QSlider>

// Qt Custom Widgets
#include <QCustomPlot>

// STL
#include <complex>
#include <vector>
#include <memory>

// Project Headers
#include "bb_api.h"
#include "waterfallplot.h"
#include "demodulator.h"
#include "analyzer.h"
#include "datamanager.h"

// Forward declarations
class BbDeviceInterface;
class QCPItemTracer;
class QCPItemText;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

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

    // Veri güncelleme - slot olarak tanımlandı
    void updateData();

private:
    // GUI bileşenleri
    std::unique_ptr<QCustomPlot> plotWidget;
    std::unique_ptr<WaterfallPlot> waterfallWidget;
    
    // Dock widget'lar
    std::unique_ptr<QDockWidget> freqDock;
    std::unique_ptr<QDockWidget> markerDock;
    std::unique_ptr<QDockWidget> traceDock;
    std::unique_ptr<QDockWidget> triggerDock;
    std::unique_ptr<QDockWidget> demodDock;
    std::unique_ptr<QDockWidget> measureDock;
    
    // Kontrol bileşenleri
    std::unique_ptr<QDoubleSpinBox> centerFreq;
    std::unique_ptr<QDoubleSpinBox> spanFreq;
    std::unique_ptr<QComboBox> rbwSelect;
    std::unique_ptr<QComboBox> vbwSelect;
    std::unique_ptr<QDoubleSpinBox> refLevel;
    
    // Trigger kontrolleri
    std::unique_ptr<QComboBox> triggerSource;
    std::unique_ptr<QDoubleSpinBox> triggerLevel;
    std::unique_ptr<QComboBox> triggerMode;
    
    // Demodülasyon kontrolleri
    std::unique_ptr<QComboBox> demodType;
    std::unique_ptr<QDoubleSpinBox> demodFreq;
    std::unique_ptr<QDoubleSpinBox> demodBW;
    std::unique_ptr<QSlider> volumeSlider;
    
    // Marker sistemi
    struct Marker {
        bool active{false};
        double frequency{0.0};
        double amplitude{-120.0};
        QCPItemTracer* tracer{nullptr};
        QCPItemText* label{nullptr};
    };
    std::vector<Marker> markers;
    int activeMarker{0};
    bool deltaMode{false};
    
    // BB60C cihaz kontrolü
    std::unique_ptr<BbDeviceInterface> device;
    bool isConnected{false};
    bool isRunning{false};
    
    // Yardımcı sınıflar
    std::unique_ptr<Demodulator> demodulator;
    std::unique_ptr<Analyzer> analyzer;
    std::unique_ptr<DataManager> dataManager;
    
    // Veri toplama ve işleme
    std::unique_ptr<QTimer> updateTimer;
    QVector<double> frequencies;
    QVector<double> amplitudes;
    QVector<std::complex<float>> iqData;
    
    // Yardımcı fonksiyonlar
    void setupUI();
    void createMenuBar();
    void createToolBar();
    void createDockWindows();
    void createStatusBar();
    void setupPlot();
    void updatePlot();
    void updateWaterfall();
    void updateMeasurements();
    void updateDemodulation();
    void processIQData();
    void startAcquisition();
    void stopAcquisition();
    void setupMarkers();
    void updateMarker(int index);
};

#endif // MAINWINDOW_H 