#ifndef DEMODULATOR_H
#define DEMODULATOR_H

#include <QObject>
#include <complex>
#include <vector>

class Demodulator : public QObject {
    Q_OBJECT
public:
    enum Mode {
        AM,
        FM,
        USB,
        LSB,
        CW
    };

    explicit Demodulator(QObject *parent = nullptr);
    void setMode(Mode mode);
    void setFrequency(double freq);
    void setBandwidth(double bw);
    std::vector<float> demodulate(const std::vector<std::complex<float>>& iqData);

private:
    Mode currentMode;
    double centerFreq;
    double bandwidth;
    
    // Demodülasyon fonksiyonları
    std::vector<float> demodulateAM(const std::vector<std::complex<float>>& iqData);
    std::vector<float> demodulateFM(const std::vector<std::complex<float>>& iqData);
    std::vector<float> demodulateSSB(const std::vector<std::complex<float>>& iqData, bool upperSideband);
    std::vector<float> demodulateCW(const std::vector<std::complex<float>>& iqData);
};

#endif // DEMODULATOR_H 