#include "mainwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QFormLayout>
#include <QLabel>
#include <QApplication>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , device(std::make_unique<BbDeviceInterface>())
    , demodulator(std::make_unique<Demodulator>(this))
    , analyzer(std::make_unique<Analyzer>(this))
    , dataManager(std::make_unique<DataManager>(this))
    , updateTimer(std::make_unique<QTimer>(this))
    , isConnected(false)
    , isRunning(false)
{
    setupUI();
    createMenuBar();
    createToolBar();
    createDockWindows();
    createStatusBar();
    
    // Spektrum görüntüleme alanını oluştur
    plotWidget = new QCustomPlot(this);
    setupPlot();
    mainLayout->addWidget(plotWidget);
    
    // Waterfall görüntüleme
    waterfallWidget = new WaterfallPlot(this);
    mainLayout->addWidget(waterfallWidget);
    
    // Timer bağlantısı
    connect(updateTimer.get(), &QTimer::timeout, this, &MainWindow::updateData);
    
    // Varsayılan ayarlar
    centerFreq->setValue(1e9);  // 1 GHz
    spanFreq->setValue(100e6);  // 100 MHz
    refLevel->setValue(0);      // 0 dBm
    
    // Durum çubuğunu güncelle
    statusBar()->showMessage("Program hazır...");
}

MainWindow::~MainWindow()
{
    if (isRunning) {
        stopAcquisition();
    }
}

void MainWindow::setupUI()
{
    // Pencere başlığı
    setWindowTitle(tr("BB60C Spectrum Analyzer"));
    
    // Pencere boyutu
    resize(1024, 768);
    
    // Ana widget'ı oluştur
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Arayüz bileşenlerini oluştur
    setupUI();
    createMenuBar();
    createToolBar();
    createDockWindows();
    createStatusBar();
    
    // Spektrum görüntüleme alanını oluştur
    plotWidget = new QCustomPlot(this);
    setupPlot();
    mainLayout->addWidget(plotWidget);
    
    // Waterfall görüntüleme
    waterfallWidget = new WaterfallPlot(this);
    mainLayout->addWidget(waterfallWidget);
    
    // Timer bağlantısı
    connect(updateTimer.get(), &QTimer::timeout, this, &MainWindow::updateData);
    
    // Varsayılan ayarlar
    centerFreq->setValue(1e9);  // 1 GHz
    spanFreq->setValue(100e6);  // 100 MHz
    refLevel->setValue(0);      // 0 dBm
    
    // Durum çubuğunu güncelle
    statusBar()->showMessage("Program hazır...");
}

void MainWindow::createMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // Dosya menüsü
    QMenu* fileMenu = menuBar->addMenu(tr("Dosya"));
    fileMenu->addAction(tr("Trace Kaydet"), this, &MainWindow::onSaveTrace);
    fileMenu->addAction(tr("Trace Yükle"), this, &MainWindow::onLoadTrace);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Durum Kaydet"), this, &MainWindow::onSaveState);
    fileMenu->addAction(tr("Durum Yükle"), this, &MainWindow::onLoadState);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("CSV Olarak Dışa Aktar"), this, &MainWindow::onExportData);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("Çıkış"), this, &QWidget::close);
    
    // Görünüm menüsü
    QMenu* viewMenu = menuBar->addMenu(tr("Görünüm"));
    viewMenu->addAction(freqDock->toggleViewAction());
    viewMenu->addAction(markerDock->toggleViewAction());
    viewMenu->addAction(traceDock->toggleViewAction());
    viewMenu->addAction(triggerDock->toggleViewAction());
    viewMenu->addAction(demodDock->toggleViewAction());
    viewMenu->addAction(measureDock->toggleViewAction());
    
    // Ölçüm menüsü
    QMenu* measureMenu = menuBar->addMenu(tr("Ölçüm"));
    measureMenu->addAction(tr("Kanal Gücü"), this, &MainWindow::onChannelPowerMeasure);
    measureMenu->addAction(tr("OBW"), this, &MainWindow::onOBWMeasure);
    measureMenu->addAction(tr("ACPR"), this, &MainWindow::onACPRMeasure);
    measureMenu->addAction(tr("Spur Arama"), this, &MainWindow::onSpurSearch);
    measureMenu->addAction(tr("Faz Gürültüsü"), this, &MainWindow::onPhaseNoiseMeasure);
}

void MainWindow::createToolBar()
{
    QToolBar* toolbar = addToolBar(tr("Ana Araç Çubuğu"));
    toolbar->setMovable(false);
    
    // Cihaz kontrolü
    toolbar->addAction(QIcon(":/icons/connect.png"), tr("Bağlan"), 
                      this, &MainWindow::onConnect);
    toolbar->addAction(QIcon(":/icons/start.png"), tr("Başlat/Durdur"), 
                      this, &MainWindow::onStartStop);
    toolbar->addAction(QIcon(":/icons/single.png"), tr("Tek Çekim"), 
                      this, &MainWindow::onSingleSweep);
    
    toolbar->addSeparator();
    
    // Marker kontrolü
    toolbar->addAction(QIcon(":/icons/peak.png"), tr("Tepe Noktası"), 
                      this, &MainWindow::onPeakSearch);
    toolbar->addAction(QIcon(":/icons/next_peak.png"), tr("Sonraki Tepe"), 
                      this, &MainWindow::onNextPeak);
    toolbar->addAction(QIcon(":/icons/delta.png"), tr("Delta Marker"), 
                      this, &MainWindow::onDeltaMarkerToggled);
}

void MainWindow::createDockWindows()
{
    // Frekans paneli
    freqDock = new QDockWidget(tr("Frekans Ayarları"), this);
    QWidget* freqWidget = new QWidget(freqDock);
    QFormLayout* freqLayout = new QFormLayout(freqWidget);
    
    centerFreq = new QDoubleSpinBox(freqWidget);
    centerFreq->setRange(9e3, 6.4e9);
    centerFreq->setSuffix(" Hz");
    centerFreq->setDecimals(0);
    connect(centerFreq, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onCenterFreqChanged);
            
    spanFreq = new QDoubleSpinBox(freqWidget);
    spanFreq->setRange(20, 6.4e9);
    spanFreq->setSuffix(" Hz");
    spanFreq->setDecimals(0);
    connect(spanFreq, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onSpanChanged);
            
    freqLayout->addRow(tr("Merkez:"), centerFreq);
    freqLayout->addRow(tr("Span:"), spanFreq);
    freqDock->setWidget(freqWidget);
    addDockWidget(Qt::RightDockWidgetArea, freqDock);
    
    // Diğer dock widget'lar benzer şekilde...
}

void MainWindow::setupPlot()
{
    plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    
    // Eksen ayarları
    plotWidget->xAxis->setLabel("Frekans (Hz)");
    plotWidget->yAxis->setLabel("Güç (dBm)");
    
    // Izgara çizgileri
    plotWidget->xAxis->grid()->setVisible(true);
    plotWidget->yAxis->grid()->setVisible(true);
    
    // Grafik ekleme
    plotWidget->addGraph();
    plotWidget->graph(0)->setPen(QPen(Qt::yellow));
    
    // Eksen aralıkları
    plotWidget->xAxis->setRange(1e9, 1.1e9);
    plotWidget->yAxis->setRange(-120, 0);
}

void MainWindow::updateData()
{
    if (!isRunning || !device) return;
    
    try {
        // BB60C'den veri al
        auto trace = device->bb_fetch_trace();
        
        // Frekans vektörünü güncelle
        double start = centerFreq->value() - spanFreq->value()/2;
        double stop = centerFreq->value() + spanFreq->value()/2;
        frequencies = QVector<double>::fromStdVector(
            std::vector<double>(trace.size(), 0.0));
        for (int i = 0; i < trace.size(); ++i) {
            frequencies[i] = start + i * (stop - start) / (trace.size() - 1);
        }
        
        // Genlik verilerini güncelle
        amplitudes = QVector<double>::fromStdVector(trace);
        
        // Grafikleri güncelle
        updatePlot();
        updateWaterfall();
        updateMeasurements();
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Hata", 
            QString("Veri okuma hatası: %1").arg(e.what()));
        stopAcquisition();
    }
}

void MainWindow::updatePlot()
{
    plotWidget->graph(0)->setData(frequencies, amplitudes);
    plotWidget->replot();
}

void MainWindow::updateWaterfall()
{
    waterfallWidget->addData(amplitudes);
    waterfallWidget->setFrequencyRange(
        frequencies.first(), frequencies.last());
}

// Slot implementasyonları...
void MainWindow::onConnect()
{
    try {
        if (!isConnected) {
            if (device->connect()) {
                isConnected = true;
                statusBar()->showMessage(tr("Cihaz bağlandı"));
            } else {
                QMessageBox::warning(this, tr("Hata"), 
                    tr("Cihaz bağlantısı başarısız: %1")
                    .arg(device->getLastError()));
            }
        } else {
            device->disconnect();
            isConnected = false;
            statusBar()->showMessage(tr("Cihaz bağlantısı kesildi"));
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Hata"), 
            tr("Bağlantı hatası: %1").arg(e.what()));
    }
}

void MainWindow::onStartStop()
{
    if (!isConnected) {
        QMessageBox::warning(this, tr("Uyarı"), 
            tr("Önce cihaza bağlanın"));
        return;
    }
    
    if (!isRunning) {
        startAcquisition();
    } else {
        stopAcquisition();
    }
}

void MainWindow::onSingleSweep()
{
    if (!isConnected) {
        QMessageBox::warning(this, tr("Uyarı"), 
            tr("Önce cihaza bağlanın"));
        return;
    }
    
    // Tek çekim için timer'ı bir kez çalıştır
    updateData();
}

void MainWindow::onCenterFreqChanged(double freq)
{
    if (device) {
        device->setCenterFrequency(freq);
        updateData();
    }
}

void MainWindow::onSpanChanged(double span)
{
    if (device) {
        device->setSpan(span);
        updateData();
    }
}

void MainWindow::onRBWChanged(int index)
{
    if (device) {
        device->setRBW(index);
        updateData();
    }
}

void MainWindow::onVBWChanged(int index)
{
    if (device) {
        device->setVBW(index);
        updateData();
    }
}

void MainWindow::onRefLevelChanged(double level)
{
    if (device) {
        device->setRefLevel(level);
        updateData();
    }
}

void MainWindow::onSaveTrace()
{
    QString filename = QFileDialog::getSaveFileName(this,
        tr("Trace Kaydet"), QString(),
        tr("Trace Dosyaları (*.trace);;Tüm Dosyalar (*)"));
        
    if (filename.isEmpty())
        return;
        
    if (!dataManager->saveTrace(filename, frequencies, amplitudes)) {
        QMessageBox::critical(this, tr("Hata"),
            tr("Trace kaydedilemedi"));
    }
}

void MainWindow::onLoadTrace()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Trace Yükle"), QString(),
        tr("Trace Dosyaları (*.trace);;Tüm Dosyalar (*)"));
        
    if (filename.isEmpty())
        return;
        
    QVector<double> loadedFreqs, loadedAmps;
    if (!dataManager->loadTrace(filename, loadedFreqs, loadedAmps)) {
        QMessageBox::critical(this, tr("Hata"),
            tr("Trace yüklenemedi"));
        return;
    }
    
    frequencies = loadedFreqs;
    amplitudes = loadedAmps;
    updatePlot();
    updateWaterfall();
}

void MainWindow::startAcquisition()
{
    isRunning = true;
    updateTimer->start(50);  // 50ms = 20 FPS
    statusBar()->showMessage(tr("Veri toplama başladı"));
}

void MainWindow::stopAcquisition()
{
    isRunning = false;
    updateTimer->stop();
    statusBar()->showMessage(tr("Veri toplama durdu"));
}

void MainWindow::updateMeasurements()
{
    if (frequencies.isEmpty() || amplitudes.isEmpty())
        return;
        
    // Aktif marker'ı güncelle
    if (!markers.empty() && activeMarker < markers.size()) {
        updateMarker(activeMarker);
    }
}

void MainWindow::setupMarkers()
{
    // İlk marker'ı oluştur
    Marker marker;
    marker.active = true;
    marker.tracer = new QCPItemTracer(plotWidget);
    marker.tracer->setStyle(QCPItemTracer::tsPlus);
    marker.tracer->setPen(QPen(Qt::yellow));
    marker.tracer->setSize(10);
    
    marker.label = new QCPItemText(plotWidget);
    marker.label->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    marker.label->position->setParentAnchor(marker.tracer->position);
    marker.label->setText("M1");
    marker.label->setColor(Qt::yellow);
    
    markers.push_back(marker);
}

void MainWindow::updateMarker(int index)
{
    if (index >= markers.size() || frequencies.isEmpty())
        return;
        
    auto& marker = markers[index];
    if (!marker.active)
        return;
        
    // Marker pozisyonunu güncelle
    int dataIndex = frequencies.size() / 2;  // Varsayılan olarak ortada
    marker.frequency = frequencies[dataIndex];
    marker.amplitude = amplitudes[dataIndex];
    
    marker.tracer->setGraphKey(marker.frequency);
    marker.tracer->setGraphValue(marker.amplitude);
    
    // Label'ı güncelle
    QString labelText = QString("M%1\n%2 Hz\n%3 dBm")
        .arg(index + 1)
        .arg(marker.frequency, 0, 'f', 0)
        .arg(marker.amplitude, 0, 'f', 1);
    marker.label->setText(labelText);
    
    plotWidget->replot();
}

void MainWindow::onPeakSearch()
{
    if (frequencies.isEmpty() || amplitudes.isEmpty())
        return;
        
    // En yüksek genlikli noktayı bul
    int peakIndex = 0;
    double peakValue = amplitudes[0];
    
    for (int i = 1; i < amplitudes.size(); ++i) {
        if (amplitudes[i] > peakValue) {
            peakValue = amplitudes[i];
            peakIndex = i;
        }
    }
    
    // Aktif marker'ı tepe noktasına taşı
    if (!markers.empty() && activeMarker < markers.size()) {
        auto& marker = markers[activeMarker];
        marker.frequency = frequencies[peakIndex];
        marker.amplitude = amplitudes[peakIndex];
        updateMarker(activeMarker);
    }
}

void MainWindow::onNextPeak()
{
    if (frequencies.isEmpty() || amplitudes.isEmpty() || markers.empty())
        return;
        
    const auto& currentMarker = markers[activeMarker];
    int currentIndex = 0;
    
    // Mevcut marker'ın indeksini bul
    for (int i = 0; i < frequencies.size(); ++i) {
        if (std::abs(frequencies[i] - currentMarker.frequency) < 1e-6) {
            currentIndex = i;
            break;
        }
    }
    
    // Sonraki tepe noktasını bul
    int nextPeakIndex = -1;
    double nextPeakValue = -INFINITY;
    
    for (int i = currentIndex + 1; i < amplitudes.size(); ++i) {
        if (amplitudes[i] > nextPeakValue && amplitudes[i] < currentMarker.amplitude) {
            nextPeakValue = amplitudes[i];
            nextPeakIndex = i;
        }
    }
    
    if (nextPeakIndex != -1) {
        auto& marker = markers[activeMarker];
        marker.frequency = frequencies[nextPeakIndex];
        marker.amplitude = amplitudes[nextPeakIndex];
        updateMarker(activeMarker);
    }
}

void MainWindow::onDeltaMarkerToggled(bool enabled)
{
    deltaMode = enabled;
    
    if (enabled && markers.size() == 1) {
        // İkinci marker'ı oluştur
        Marker marker;
        marker.active = true;
        marker.tracer = new QCPItemTracer(plotWidget);
        marker.tracer->setStyle(QCPItemTracer::tsCross);
        marker.tracer->setPen(QPen(Qt::red));
        marker.tracer->setSize(10);
        
        marker.label = new QCPItemText(plotWidget);
        marker.label->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        marker.label->position->setParentAnchor(marker.tracer->position);
        marker.label->setText("Δ");
        marker.label->setColor(Qt::red);
        
        markers.push_back(marker);
        activeMarker = 1;
        updateMarker(activeMarker);
    }
}

void MainWindow::onChannelPowerMeasure()
{
    if (frequencies.isEmpty() || amplitudes.isEmpty())
        return;
        
    double startFreq = centerFreq->value() - spanFreq->value() / 4;  // 25% span
    double stopFreq = centerFreq->value() + spanFreq->value() / 4;   // 25% span
    
    double power = analyzer->measureChannelPower(frequencies, amplitudes, startFreq, stopFreq);
    
    QMessageBox::information(this, tr("Kanal Gücü"),
        tr("Kanal Gücü: %1 dBm").arg(power, 0, 'f', 2));
}

void MainWindow::onOBWMeasure()
{
    if (frequencies.isEmpty() || amplitudes.isEmpty())
        return;
        
    double obw = analyzer->measureOBW(frequencies, amplitudes, 99.0);  // 99% güç
    
    QMessageBox::information(this, tr("OBW"),
        tr("Occupied Bandwidth: %1 Hz").arg(obw, 0, 'f', 0));
}

void MainWindow::onACPRMeasure()
{
    if (frequencies.isEmpty() || amplitudes.isEmpty())
        return;
        
    double channelBW = 1e6;     // 1 MHz
    double channelSpacing = 2e6; // 2 MHz
    
    auto result = analyzer->measureACPR(frequencies, amplitudes, channelBW, channelSpacing);
    
    QString message = tr("ACPR Ölçümü:\n\n"
                        "Ana Kanal Gücü: %1 dBm\n"
                        "Alt Kanal Gücü: %2 dBm\n"
                        "Üst Kanal Gücü: %3 dBm\n"
                        "Alt Kanal Oranı: %4 dB\n"
                        "Üst Kanal Oranı: %5 dB")
                        .arg(result.mainChannelPower, 0, 'f', 2)
                        .arg(result.lowerChannelPower, 0, 'f', 2)
                        .arg(result.upperChannelPower, 0, 'f', 2)
                        .arg(result.lowerRatio, 0, 'f', 2)
                        .arg(result.upperRatio, 0, 'f', 2);
                        
    QMessageBox::information(this, tr("ACPR"), message);
}

void MainWindow::onSpurSearch()
{
    if (frequencies.isEmpty() || amplitudes.isEmpty())
        return;
        
    double threshold = -50.0;  // -50 dBm eşik değeri
    auto spurs = analyzer->findSpurs(frequencies, amplitudes, threshold);
    
    if (spurs.isEmpty()) {
        QMessageBox::information(this, tr("Spur Arama"),
            tr("Eşik değerinin üzerinde spur bulunamadı."));
        return;
    }
    
    QString message = tr("Bulunan Spurlar:\n\n");
    for (int i = 0; i < spurs.size(); ++i) {
        message += tr("Spur %1:\n"
                     "  Frekans: %2 Hz\n"
                     "  Genlik: %3 dBm\n"
                     "  Bağıl Güç: %4 dB\n\n")
                     .arg(i + 1)
                     .arg(spurs[i].frequency, 0, 'f', 0)
                     .arg(spurs[i].amplitude, 0, 'f', 1)
                     .arg(spurs[i].relativePower, 0, 'f', 1);
    }
    
    QMessageBox::information(this, tr("Spur Arama"), message);
}

void MainWindow::onPhaseNoiseMeasure()
{
    if (frequencies.isEmpty() || amplitudes.isEmpty())
        return;
        
    // Faz gürültüsü ölçüm noktaları (offset frekansları)
    QVector<double> offsets = {1e3, 10e3, 100e3, 1e6};  // 1k, 10k, 100k, 1M Hz
    
    // IQ verisi al
    auto iqData = device->bb_fetch_iq_data();
    double sampleRate = device->getSampleRate();
    
    auto phaseNoise = analyzer->measurePhaseNoise(iqData, sampleRate, offsets);
    
    QString message = tr("Faz Gürültüsü Ölçümü:\n\n");
    for (int i = 0; i < offsets.size(); ++i) {
        message += tr("Offset %1 Hz: %2 dBc/Hz\n")
                     .arg(offsets[i], 0, 'f', 0)
                     .arg(phaseNoise[i], 0, 'f', 1);
    }
    
    QMessageBox::information(this, tr("Faz Gürültüsü"), message);
}

void MainWindow::onDemodTypeChanged(int index)
{
    if (!demodulator)
        return;
        
    demodulator->setMode(static_cast<Demodulator::Mode>(index));
    updateDemodulation();
}

void MainWindow::onDemodFreqChanged(double freq)
{
    if (!demodulator)
        return;
        
    demodulator->setFrequency(freq);
    updateDemodulation();
}

void MainWindow::onDemodBWChanged(double bw)
{
    if (!demodulator)
        return;
        
    demodulator->setBandwidth(bw);
    updateDemodulation();
}

void MainWindow::updateDemodulation()
{
    if (!device || !demodulator)
        return;
        
    // IQ verisi al
    auto iqData = device->bb_fetch_iq_data();
    
    // Demodüle et
    auto audioData = demodulator->demodulate(iqData);
    
    // Audio verisi işleme/oynatma kodu buraya gelecek
    // ...
}

void MainWindow::processIQData()
{
    if (!device)
        return;
        
    try {
        // IQ verisi al
        auto iqData = device->bb_fetch_iq_data();
        
        // Demodülasyon aktif ise
        if (demodType->currentIndex() != 0) {  // 0 = Demodülasyon yok
            updateDemodulation();
        }
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, tr("Hata"),
            tr("IQ veri okuma hatası: %1").arg(e.what()));
    }
}

// Diğer slot implementasyonları... 