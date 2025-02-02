#ifndef DEMODULATOR_H
#define DEMODULATOR_H

#include <QObject>
#include <QVector>
#include <complex>
#include <memory>

class Demodulator : public QObject
{
    Q_OBJECT
public:
    enum class Mode {
        None,
        AM,
        FM,
        USB,
        LSB
    };
    Q_ENUM(Mode)

    explicit Demodulator(QObject *parent = nullptr);
    ~Demodulator() override;

    // Ayarlar
    void setMode(Mode mode);
    void setFrequency(double freq);
    void setBandwidth(double bw);
    void setVolume(int volume);

    // Demodülasyon
    QVector<float> demodulate(const QVector<std::complex<float>>& iqData);

    // Durum sorgulama
    Mode currentMode() const { return mode; }
    double frequency() const { return frequency; }
    double bandwidth() const { return bandwidth; }
    int volume() const { return volume; }

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    // Demodülasyon parametreleri
    Mode mode{Mode::None};
    double frequency{1e6};  // 1 MHz
    double bandwidth{10e3}; // 10 kHz
    int volume{50};        // 0-100 arası

    // Demodülasyon fonksiyonları
    QVector<float> demodulateAM(const QVector<std::complex<float>>& iqData);
    QVector<float> demodulateFM(const QVector<std::complex<float>>& iqData);
    QVector<float> demodulateSSB(const QVector<std::complex<float>>& iqData, bool upperSideband);
};

#endif // DEMODULATOR_H
