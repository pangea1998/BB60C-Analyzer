#ifndef BB_API_H
#define BB_API_H

#include <QObject>
#include <QVector>
#include <complex>
#include <memory>
#include <string>
#include <QString>

// BB60C cihazı için temel ayarlar
struct BBSettings {
    double centerFreq{1e9};     // 1 GHz
    double span{100e6};         // 100 MHz
    double refLevel{0.0};       // 0 dBm
    int rbw{100000};           // 100 kHz
    int vbw{100000};           // 100 kHz
    double sampleRate{40e6};    // 40 MHz
};

class BbDeviceInterface : public QObject {
    Q_OBJECT
public:
    explicit BbDeviceInterface(QObject *parent = nullptr);
    ~BbDeviceInterface() override;

    // Cihaz kontrolü
    bool connect();
    void disconnect();
    bool isConnected() const;
    QString getLastError() const;

    // Ayarlar
    void setCenterFrequency(double freq);
    void setSpan(double span);
    void setRBW(int rbw);
    void setVBW(int vbw);
    void setRefLevel(double level);
    double getSampleRate() const;

    // Veri toplama
    QVector<double> bb_fetch_trace();
    QVector<std::complex<float>> bb_fetch_iq_data();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;  // PIMPL idiom
    BBSettings settings;
    bool connected{false};
    QString lastError;

    // Yardımcı fonksiyonlar
    void initializeDevice();
    void configureDevice();
    void checkError(int status);
};

#endif // BB_API_H 