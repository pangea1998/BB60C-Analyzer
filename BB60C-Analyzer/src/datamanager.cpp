#include "datamanager.h"
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// PIMPL implementation
struct DataManager::Impl {
    // Dosya formatı versiyonu
    static constexpr int FILE_VERSION = 1;
};

DataManager::DataManager(QObject *parent)
    : QObject(parent)
    , pimpl(std::make_unique<Impl>())
{
}

DataManager::~DataManager() = default;

bool DataManager::saveTrace(const QString& filename,
                          const QVector<double>& frequencies,
                          const QVector<double>& amplitudes)
{
    if (frequencies.size() != amplitudes.size()) {
        setError(tr("Frekans ve genlik vektörlerinin boyutları eşleşmiyor"));
        return false;
    }

    QJsonObject root;
    root["version"] = Impl::FILE_VERSION;
    root["size"] = frequencies.size();

    QJsonArray freqArray;
    QJsonArray ampArray;

    for (int i = 0; i < frequencies.size(); ++i) {
        freqArray.append(frequencies[i]);
        ampArray.append(amplitudes[i]);
    }

    root["frequencies"] = freqArray;
    root["amplitudes"] = ampArray;

    QJsonDocument doc(root);
    return writeFile(filename, doc.toJson());
}

bool DataManager::loadTrace(const QString& filename,
                          QVector<double>& frequencies,
                          QVector<double>& amplitudes)
{
    QByteArray data;
    if (!readFile(filename, data)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        setError(tr("Geçersiz JSON formatı"));
        return false;
    }

    QJsonObject root = doc.object();
    int version = root["version"].toInt();
    if (version != Impl::FILE_VERSION) {
        setError(tr("Desteklenmeyen dosya versiyonu"));
        return false;
    }

    int size = root["size"].toInt();
    QJsonArray freqArray = root["frequencies"].toArray();
    QJsonArray ampArray = root["amplitudes"].toArray();

    if (freqArray.size() != size || ampArray.size() != size) {
        setError(tr("Bozuk dosya formatı"));
        return false;
    }

    frequencies.resize(size);
    amplitudes.resize(size);

    for (int i = 0; i < size; ++i) {
        frequencies[i] = freqArray[i].toDouble();
        amplitudes[i] = ampArray[i].toDouble();
    }

    return true;
}

bool DataManager::exportToCSV(const QString& filename,
                            const QVector<double>& frequencies,
                            const QVector<double>& amplitudes)
{
    if (frequencies.size() != amplitudes.size()) {
        setError(tr("Frekans ve genlik vektörlerinin boyutları eşleşmiyor"));
        return false;
    }

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(file.errorString());
        return false;
    }

    QTextStream out(&file);
    out << "Frequency (Hz),Amplitude (dBm)\n";

    for (int i = 0; i < frequencies.size(); ++i) {
        out << QString::number(frequencies[i], 'f', 1) << ","
            << QString::number(amplitudes[i], 'f', 2) << "\n";
    }

    return true;
}

bool DataManager::saveState(const QString& filename, const QByteArray& state)
{
    return writeFile(filename, state);
}

bool DataManager::loadState(const QString& filename, QByteArray& state)
{
    return readFile(filename, state);
}

QString DataManager::getLastError() const
{
    return lastError;
}

void DataManager::setError(const QString& error)
{
    lastError = error;
}

bool DataManager::writeFile(const QString& filename, const QByteArray& data)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        setError(file.errorString());
        return false;
    }

    if (file.write(data) != data.size()) {
        setError(file.errorString());
        return false;
    }

    return true;
}

bool DataManager::readFile(const QString& filename, QByteArray& data)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        setError(file.errorString());
        return false;
    }

    data = file.readAll();
    if (data.isEmpty() && file.error() != QFile::NoError) {
        setError(file.errorString());
        return false;
    }

    return true;
}
