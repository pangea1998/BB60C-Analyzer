#include "datamanager.h"
#include <QFile>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>

DataManager::DataManager(QObject *parent) : QObject(parent) {
}

bool DataManager::saveTrace(const QString& filename,
                          const QVector<double>& freqs,
                          const QVector<double>& amplitudes) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_6_0);
    
    // Veri formatı versiyonu
    stream << quint32(0x01);
    
    // Veri boyutu
    stream << quint32(freqs.size());
    
    // Frekans ve genlik verilerini yaz
    for (int i = 0; i < freqs.size(); ++i) {
        stream << freqs[i] << amplitudes[i];
    }
    
    file.close();
    return true;
}

bool DataManager::loadTrace(const QString& filename,
                          QVector<double>& freqs,
                          QVector<double>& amplitudes) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_6_0);
    
    // Versiyon kontrolü
    quint32 version;
    stream >> version;
    if (version != 0x01) {
        return false;
    }
    
    // Veri boyutunu oku
    quint32 size;
    stream >> size;
    
    // Vektörleri temizle ve yeni boyut için yer ayır
    freqs.clear();
    amplitudes.clear();
    freqs.reserve(size);
    amplitudes.reserve(size);
    
    // Verileri oku
    for (quint32 i = 0; i < size; ++i) {
        double freq, amp;
        stream >> freq >> amp;
        freqs.append(freq);
        amplitudes.append(amp);
    }
    
    file.close();
    return true;
}

bool DataManager::exportCSV(const QString& filename,
                          const QVector<double>& freqs,
                          const QVector<double>& amplitudes) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream stream(&file);
    
    // Başlık satırı
    stream << "Frequency (Hz),Amplitude (dBm)\n";
    
    // Verileri yaz
    for (int i = 0; i < freqs.size(); ++i) {
        stream << QString::number(freqs[i], 'f', 1) << ","
               << QString::number(amplitudes[i], 'f', 2) << "\n";
    }
    
    file.close();
    return true;
}

bool DataManager::saveState(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonObject state;
    // Program durumunu JSON formatında kaydet
    // Bu kısım MainWindow'dan gelen verilerle doldurulacak
    
    QJsonDocument doc(state);
    file.write(doc.toJson());
    file.close();
    return true;
}

bool DataManager::loadState(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        return false;
    }
    
    QJsonObject state = doc.object();
    // Program durumunu JSON'dan yükle
    // Bu kısım MainWindow'a veri göndermek için kullanılacak
    
    file.close();
    return true;
}

bool DataManager::saveIQData(const QString& filename,
                           const QVector<std::complex<float>>& iqData,
                           double sampleRate) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_6_0);
    
    // Format versiyonu
    stream << quint32(0x01);
    
    // Örnekleme hızı
    stream << double(sampleRate);
    
    // Veri boyutu
    stream << quint32(iqData.size());
    
    // IQ verilerini yaz
    for (const auto& sample : iqData) {
        stream << sample.real() << sample.imag();
    }
    
    file.close();
    return true;
}

bool DataManager::loadIQData(const QString& filename,
                           QVector<std::complex<float>>& iqData,
                           double& sampleRate) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_6_0);
    
    // Versiyon kontrolü
    quint32 version;
    stream >> version;
    if (version != 0x01) {
        return false;
    }
    
    // Örnekleme hızını oku
    stream >> sampleRate;
    
    // Veri boyutunu oku
    quint32 size;
    stream >> size;
    
    // Vektörü temizle ve yeni boyut için yer ayır
    iqData.clear();
    iqData.reserve(size);
    
    // IQ verilerini oku
    for (quint32 i = 0; i < size; ++i) {
        float real, imag;
        stream >> real >> imag;
        iqData.append(std::complex<float>(real, imag));
    }
    
    file.close();
    return true;
} 