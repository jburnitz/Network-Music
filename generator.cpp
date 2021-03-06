#include "generator.h"
#include <qmath.h>
#include <qendian.h>
#include <QDebug>

/** \abstract Acts as a file type device to get audio from */
Generator::Generator(const QAudioFormat &format, qint64 durationUs, int sampleRate, QObject *parent) : QIODevice(parent), m_pos(0)
{
    frequency = sampleRate;
    if (format.isValid())
        generateData(format, durationUs);
}

Generator::~Generator()
{

}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    m_pos = 0;
    close();
}

void Generator::setFrequency(int value){
    frequency=value;
}

/** Generates a buffer of pure tones */
void Generator::generateData(const QAudioFormat &format, qint64 durationUs)
{

    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channelCount() * channelBytes;

    qint64 length = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8))
                        * durationUs / 100000;

    Q_ASSERT(length % sampleBytes == 0);

    m_buffer = new QByteArray(length, '0');


    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer->data());
    int sampleIndex = 0;

    while (length) {
        const qreal x = qSin(2 * M_PI * frequency * qreal(sampleIndex % format.sampleRate()) / format.sampleRate());

        for (int i=0; i<format.channelCount(); ++i) {
            if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::UnSignedInt) {
                const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::SignedInt) {
                const qint8 value = static_cast<qint8>(x * 127);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::UnSignedInt) {
                quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<quint16>(value, ptr);
                else
                    qToBigEndian<quint16>(value, ptr);
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt) {
                qint16 value = static_cast<qint16>(x * 32767);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<qint16>(value, ptr);
                else
                    qToBigEndian<qint16>(value, ptr);
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
        ++sampleIndex;
    }
}

QByteArray* Generator::GenerateData(const QAudioFormat &format, qint64 frequency)
{

    //qDebug() << Q_FUNC_INFO << "    begin";

    //qDebug() << Q_FUNC_INFO << "    format="<< format.codec();
    qint64 durationUs = 1000000;
    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channelCount() * channelBytes;

    qint64 length = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8)) * durationUs / 100000;

    //qDebug() << Q_FUNC_INFO << "    length="<< length << "samplebytes= "<< sampleBytes;
    Q_ASSERT(length % sampleBytes == 0);

    QByteArray* m_buffer = new QByteArray(length, '0');
    unsigned char *ptr = reinterpret_cast<unsigned char *>( m_buffer->data() );

    int sampleIndex = 0;

    while (length) {

        const qreal x = qSin(2 * M_PI * frequency * qreal(sampleIndex % format.sampleRate()) / format.sampleRate());

        for (int i=0; i<format.channelCount(); ++i) {
            if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::UnSignedInt) {
                const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::SignedInt) {
                const qint8 value = static_cast<qint8>(x * 127);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::UnSignedInt) {
                quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<quint16>(value, ptr);
                else
                    qToBigEndian<quint16>(value, ptr);
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt) {
                qint16 value = static_cast<qint16>(x * 32767);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<qint16>(value, ptr);
                else
                    qToBigEndian<qint16>(value, ptr);
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
        ++sampleIndex;
    }

    //qDebug() << Q_FUNC_INFO << "    end";
    return m_buffer;
}

qint64 Generator::readData(char *data, qint64 len)
{

    qint64 total = 0;

    if (!m_buffer->isEmpty()) {
        while (len - total > 0) {

            const qint64 chunk = qMin((m_buffer->size() - m_pos), len - total);

            memcpy(data + total, m_buffer->constData() + m_pos, chunk);

            m_pos = (m_pos + chunk) % m_buffer->size();
            total += chunk;
        }
    }
    return total;
}
qint64 Generator::GetPos(){
    return m_pos;
}
void Generator::SetPos(qint64 val){
    m_pos = val;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Generator::bytesAvailable() const
{
    return m_buffer->size() + QIODevice::bytesAvailable();
}
