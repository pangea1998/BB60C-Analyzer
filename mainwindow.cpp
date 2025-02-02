#include "mainwindow.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtCore/QSettings>
#include <QApplication>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("BB60C Spectrum Analyzer");
    setMinimumSize(1200, 800);
    
    // Ana widget'ı oluştur
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Menü çubuğunu oluştur
    createMenuBar();
    
    // Arayüz bileşenlerini oluştur
    setupUI();
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
    
    // Timer'ı ayarla
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateData);
    
    // BB60C cihazını başlat
    device = new BbDeviceInterface();
    
    // Yardımcı sınıfları oluştur
    demodulator = new Demodulator(this);
    analyzer = new Analyzer(this);
    dataManager = new DataManager(this);
    
    // Varsayılan ayarlar
    centerFreq->setValue(1e9);  // 1 GHz
    spanFreq->setValue(100e6);  // 100 MHz
    refLevel->setValue(0);      // 0 dBm
    
    // Durum çubuğunu güncelle
    statusBar()->showMessage("Program hazır...");
}

void MainWindow::setupUI() {
    // Stil ayarları
    setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QDockWidget {
            color: #ffffff;
            titlebar-close-icon: url(close.png);
        }
        QGroupBox {
            border: 1px solid #404040;
            border-radius: 5px;
            margin-top: 1ex;
            color: #ffffff;
        }
        QLabel {
            color: #ffffff;
        }
        QSpinBox, QDoubleSpinBox, QComboBox {
            background-color: #404040;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 3px;
            padding: 2px;
        }
        QPushButton {
            background-color: #404040;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 3px;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: #505050;
        }
        QPushButton:pressed {
            background-color: #353535;
        }
    )");
}

void MainWindow::createToolBar() {
    QToolBar* toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    
    // Bağlantı butonu
    QAction* connectAction = toolbar->addAction("Bağlan");
    connectAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    connect(connectAction, &QAction::triggered, this, &MainWindow::onConnect);
    
    toolbar->addSeparator();
    
    // Başlat/Durdur butonu
    QAction* startAction = toolbar->addAction("Başlat");
    startAction->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    connect(startAction, &QAction::triggered, this, &MainWindow::onStartStop);
    
    // Tek çekim butonu
    QAction* singleAction = toolbar->addAction("Tek Çekim");
    connect(singleAction, &QAction::triggered, this, &MainWindow::onSingleSweep);
}

void MainWindow::setupMarkers() {
    markers.resize(4); // 4 marker destekle
    
    for (size_t i = 0; i < markers.size(); ++i) {
        markers[i].tracer = new QCPItemTracer(plotWidget);
        markers[i].tracer->setVisible(false);
        markers[i].tracer->setStyle(QCPItemTracer::tsPlus);
        markers[i].tracer->setPen(QPen(Qt::yellow));
        markers[i].tracer->setBrush(QBrush(Qt::yellow));
        
        markers[i].label = new QCPItemText(plotWidget);
        markers[i].label->setVisible(false);
        markers[i].label->setPen(QPen(Qt::yellow));
        markers[i].label->setBrush(QBrush(Qt::black));
        markers[i].label->setColor(Qt::yellow);
    }
    
    // İlk marker'ı aktif et
    markers[0].active = true;
    markers[0].tracer->setVisible(true);
    markers[0].label->setVisible(true);
}

void MainWindow::onMarkerFreqChanged(double freq) {
    if (activeMarker >= 0 && activeMarker < markers.size()) {
        markers[activeMarker].frequency = freq;
        updateMarker(activeMarker);
    }
}

void MainWindow::updateMarker(int index) {
    if (index < 0 || index >= markers.size()) return;
    
    auto& marker = markers[index];
    if (!marker.active) return;
    
    // Frekansa karşılık gelen genliği bul
    double amp = -120.0; // Varsayılan değer
    if (plotWidget->graph(0)->data()->size() > 0) {
        // En yakın frekans noktasını bul
        auto it = std::lower_bound(
            plotWidget->graph(0)->data()->begin(),
            plotWidget->graph(0)->data()->end(),
            marker.frequency,
            [](const QCPGraphData& data, double freq) {
                return data.key < freq;
            }
        );
        
        if (it != plotWidget->graph(0)->data()->end()) {
            amp = it->value;
        }
    }
    
    marker.amplitude = amp;
    
    // Marker pozisyonunu güncelle
    marker.tracer->position->setCoords(marker.frequency, marker.amplitude);
    
    // Label'ı güncelle
    QString labelText = QString("M%1\n%2 Hz\n%3 dBm")
        .arg(index + 1)
        .arg(marker.frequency, 0, 'f', 0)
        .arg(marker.amplitude, 0, 'f', 1);
    
    if (deltaMode && index == activeMarker && index > 0) {
        double deltaF = marker.frequency - markers[0].frequency;
        double deltaA = marker.amplitude - markers[0].amplitude;
        labelText += QString("\nΔF: %1 Hz\nΔA: %2 dB")
            .arg(deltaF, 0, 'f', 0)
            .arg(deltaA, 0, 'f', 1);
    }
    
    marker.label->setText(labelText);
    marker.label->position->setCoords(marker.frequency, marker.amplitude + 5);
    
    // GUI'yi güncelle
    if (index == activeMarker) {
        markerFreq->setValue(marker.frequency);
        markerAmp->setText(QString("%1 dBm").arg(marker.amplitude, 0, 'f', 1));
    }
    
    plotWidget->replot();
}

std::vector<MainWindow::Peak> MainWindow::findPeaks(
    const QVector<double>& freqs,
    const QVector<double>& amps,
    double threshold,
    double minDistance) {
    
    std::vector<Peak> peaks;
    
    for (int i = 1; i < amps.size() - 1; ++i) {
        if (amps[i] > threshold && 
            amps[i] > amps[i-1] && 
            amps[i] > amps[i+1]) {
            
            // Önceki peak ile mesafeyi kontrol et
            bool tooClose = false;
            for (const auto& peak : peaks) {
                if (std::abs(freqs[i] - peak.frequency) < minDistance) {
                    tooClose = true;
                    break;
                }
            }
            
            if (!tooClose) {
                peaks.push_back({freqs[i], amps[i]});
            }
        }
    }
    
    // Genliğe göre sırala
    std::sort(peaks.begin(), peaks.end());
    
    return peaks;
}

void MainWindow::onPeakSearch() {
    if (plotWidget->graph(0)->data()->size() == 0) return;
    
    QVector<double> freqs, amps;
    plotWidget->graph(0)->data()->toVector(freqs, amps);
    
    auto peaks = findPeaks(freqs, amps);
    if (!peaks.empty()) {
        markers[activeMarker].frequency = peaks[0].frequency;
        updateMarker(activeMarker);
    }
}

void MainWindow::createMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // Dosya menüsü
    QMenu* fileMenu = menuBar->addMenu("Dosya");
    fileMenu->addAction("Trace Kaydet", this, &MainWindow::onSaveTrace);
    fileMenu->addAction("Trace Yükle", this, &MainWindow::onLoadTrace);
    fileMenu->addSeparator();
    fileMenu->addAction("Durum Kaydet", this, &MainWindow::onSaveState);
    fileMenu->addAction("Durum Yükle", this, &MainWindow::onLoadState);
    fileMenu->addSeparator();
    fileMenu->addAction("CSV Olarak Dışa Aktar", this, &MainWindow::onExportData);
    fileMenu->addSeparator();
    fileMenu->addAction("Çıkış", this, &QWidget::close);
    
    // Görünüm menüsü
    QMenu* viewMenu = menuBar->addMenu("Görünüm");
    viewMenu->addAction(freqDock->toggleViewAction());
    viewMenu->addAction(markerDock->toggleViewAction());
    viewMenu->addAction(traceDock->toggleViewAction());
    viewMenu->addAction(triggerDock->toggleViewAction());
    viewMenu->addAction(demodDock->toggleViewAction());
    viewMenu->addAction(measureDock->toggleViewAction());
    
    // Ölçüm menüsü
    QMenu* measureMenu = menuBar->addMenu("Ölçüm");
    measureMenu->addAction("Kanal Gücü", this, &MainWindow::onChannelPowerMeasure);
    measureMenu->addAction("OBW", this, &MainWindow::onOBWMeasure);
    measureMenu->addAction("ACPR", this, &MainWindow::onACPRMeasure);
    measureMenu->addAction("Spur Arama", this, &MainWindow::onSpurSearch);
    measureMenu->addAction("Faz Gürültüsü", this, &MainWindow::onPhaseNoiseMeasure);
}

void MainWindow::createDockWindows() {
    // Frekans paneli
    freqDock = new QDockWidget("Frekans Ayarları", this);
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
            
    freqLayout->addRow("Merkez:", centerFreq);
    freqLayout->addRow("Span:", spanFreq);
    freqDock->setWidget(freqWidget);
    addDockWidget(Qt::RightDockWidgetArea, freqDock);
    
    // Diğer paneller benzer şekilde...
}

void MainWindow::setupPlot() {
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

void MainWindow::updateData() {
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

void MainWindow::updatePlot() {
    plotWidget->graph(0)->setData(frequencies, amplitudes);
    plotWidget->replot();
}

void MainWindow::updateWaterfall() {
    waterfallWidget->addData(amplitudes);
    waterfallWidget->setFrequencyRange(
        frequencies.first(), frequencies.last());
}

void MainWindow::onConnect() {
    if (!isConnected) {
        try {
            device->bb_open_device();
            
            // Seri numarasını al ve göster
            QString serialNo = QString::fromStdString(device->get_serial_number());
            statusBar()->showMessage(QString("BB60C Bağlandı - Seri No: %1").arg(serialNo));
            
            // GUI durumunu güncelle
            isConnected = true;
            baslat_action->setEnabled(true);
            tek_cekim_action->setEnabled(true);
            
            // Varsayılan ayarları uygula
            device->bb_configure_center_span(centerFreq->value(), spanFreq->value());
            
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Bağlantı Hatası",
                QString("BB60C bağlantısı başarısız: %1").arg(e.what()));
        }
    } else {
        stopAcquisition();
        device->bb_close_device();
        isConnected = false;
        baslat_action->setEnabled(false);
        tek_cekim_action->setEnabled(false);
        statusBar()->showMessage("BB60C Bağlantısı Kesildi");
    }
}

void MainWindow::onStartStop() {
    if (!isRunning) {
        startAcquisition();
    } else {
        stopAcquisition();
    }
}

void MainWindow::startAcquisition() {
    if (!isConnected) return;
    
    try {
        // Cihaz ayarlarını güncelle
        device->bb_configure_center_span(centerFreq->value(), spanFreq->value());
        
        // Timer'ı başlat
        updateTimer->start(100);  // 100ms aralıklarla güncelle
        
        isRunning = true;
        baslat_action->setText("Durdur");
        tek_cekim_action->setEnabled(false);
        statusBar()->showMessage("Veri toplama başladı...");
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Hata",
            QString("Veri toplama başlatılamadı: %1").arg(e.what()));
    }
}

void MainWindow::stopAcquisition() {
    updateTimer->stop();
    isRunning = false;
    baslat_action->setText("Başlat");
    tek_cekim_action->setEnabled(true);
    statusBar()->showMessage("Veri toplama durduruldu");
}

void MainWindow::onSingleSweep() {
    if (!isConnected) return;
    
    try {
        // Tek bir ölçüm al
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
            QString("Tek çekim hatası: %1").arg(e.what()));
    }
}

void MainWindow::onCenterFreqChanged(double freq) {
    if (!isConnected) return;
    
    try {
        device->bb_configure_center_span(freq, spanFreq->value());
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Uyarı",
            QString("Frekans ayarlama hatası: %1").arg(e.what()));
    }
}

void MainWindow::onSpanChanged(double span) {
    if (!isConnected) return;
    
    try {
        device->bb_configure_center_span(centerFreq->value(), span);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Uyarı",
            QString("Span ayarlama hatası: %1").arg(e.what()));
    }
}

void MainWindow::onChannelPowerMeasure() {
    if (frequencies.isEmpty() || amplitudes.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Ölçüm için veri yok!");
        return;
    }
    
    // Kanal gücünü ölç
    double power = analyzer->measureChannelPower(frequencies, amplitudes,
        centerFreq->value() - spanFreq->value()/4,  // Merkez frekansın ±1/4 span'i
        centerFreq->value() + spanFreq->value()/4);
        
    QMessageBox::information(this, "Kanal Gücü",
        QString("Kanal Gücü: %1 dBm").arg(power, 0, 'f', 2));
}

void MainWindow::onOBWMeasure() {
    if (frequencies.isEmpty() || amplitudes.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Ölçüm için veri yok!");
        return;
    }
    
    // OBW ölç (99% güç bandgenişliği)
    double obw = analyzer->measureOBW(frequencies, amplitudes, 99.0);
    
    QMessageBox::information(this, "OBW",
        QString("Occupied Bandwidth: %1 Hz").arg(obw, 0, 'f', 0));
}

void MainWindow::onACPRMeasure() {
    if (frequencies.isEmpty() || amplitudes.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Ölçüm için veri yok!");
        return;
    }
    
    // ACPR ölç
    auto result = analyzer->measureACPR(frequencies, amplitudes,
        spanFreq->value()/10,  // Kanal genişliği
        spanFreq->value()/5);  // Kanal aralığı
    
    QString message = QString(
        "Ana Kanal Gücü: %1 dBm\n"
        "Alt Kanal Gücü: %2 dBm\n"
        "Üst Kanal Gücü: %3 dBm\n"
        "Alt ACPR: %4 dB\n"
        "Üst ACPR: %5 dB")
        .arg(result.mainChannelPower, 0, 'f', 2)
        .arg(result.lowerChannelPower, 0, 'f', 2)
        .arg(result.upperChannelPower, 0, 'f', 2)
        .arg(result.lowerRatio, 0, 'f', 2)
        .arg(result.upperRatio, 0, 'f', 2);
        
    QMessageBox::information(this, "ACPR", message);
}

void MainWindow::onSpurSearch() {
    if (frequencies.isEmpty() || amplitudes.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Ölçüm için veri yok!");
        return;
    }
    
    // Spurları bul
    auto spurs = analyzer->findSpurs(frequencies, amplitudes, -60.0);  // -60 dBm eşik
    
    QString message = "Bulunan Spurlar:\n\n";
    for (const auto& spur : spurs) {
        message += QString("Frekans: %1 Hz, Genlik: %2 dBm, Bağıl Güç: %3 dB\n")
            .arg(spur.frequency, 0, 'f', 0)
            .arg(spur.amplitude, 0, 'f', 2)
            .arg(spur.relativePower, 0, 'f', 2);
    }
    
    QMessageBox::information(this, "Spur Arama", message);
}

void MainWindow::onSaveTrace() {
    if (frequencies.isEmpty() || amplitudes.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Kaydedilecek veri yok!");
        return;
    }
    
    QString filename = QFileDialog::getSaveFileName(this,
        "Trace Kaydet", "", "Trace Dosyası (*.trc)");
        
    if (!filename.isEmpty()) {
        if (dataManager->saveTrace(filename, frequencies, amplitudes)) {
            statusBar()->showMessage("Trace kaydedildi: " + filename);
        } else {
            QMessageBox::critical(this, "Hata", "Trace kaydedilemedi!");
        }
    }
}

void MainWindow::onLoadTrace() {
    QString filename = QFileDialog::getOpenFileName(this,
        "Trace Yükle", "", "Trace Dosyası (*.trc)");
        
    if (!filename.isEmpty()) {
        if (dataManager->loadTrace(filename, frequencies, amplitudes)) {
            updatePlot();
            updateWaterfall();
            updateMeasurements();
            statusBar()->showMessage("Trace yüklendi: " + filename);
        } else {
            QMessageBox::critical(this, "Hata", "Trace yüklenemedi!");
        }
    }
}

void MainWindow::onExportData() {
    if (frequencies.isEmpty() || amplitudes.isEmpty()) {
        QMessageBox::warning(this, "Uyarı", "Dışa aktarılacak veri yok!");
        return;
    }
    
    QString filename = QFileDialog::getSaveFileName(this,
        "CSV Olarak Dışa Aktar", "", "CSV Dosyası (*.csv)");
        
    if (!filename.isEmpty()) {
        if (dataManager->exportCSV(filename, frequencies, amplitudes)) {
            statusBar()->showMessage("Veriler CSV olarak kaydedildi: " + filename);
        } else {
            QMessageBox::critical(this, "Hata", "CSV dosyası oluşturulamadı!");
        }
    }
}

void MainWindow::updateMeasurements() {
    // Aktif marker varsa güncelle
    if (activeMarker >= 0 && activeMarker < markers.size() && markers[activeMarker].active) {
        updateMarker(activeMarker);
    }
    
    // Diğer ölçümleri güncelle
    // ...
}

// Diğer marker fonksiyonları benzer şekilde implement edilebilir... 