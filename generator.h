#ifndef GENERATOR_H
#define GENERATOR_H

#include <QByteArray>
#include <QIODevice>
#include <QAudioFormat>

class Generator : public QIODevice
{
    Q_OBJECT

public:
    Generator(const QAudioFormat &format, qint64 durationUs, int sampleRate, QObject *parent);
    ~Generator();

    void start();
    void stop();

    void setFrequency(int value);
    qint64 GetPos();
    void SetPos(qint64 val);

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;

    QByteArray* m_buffer;
    qint64 m_pos;

    static QByteArray* GenerateData(const QAudioFormat &format, qint64 frequency);

private:
    void generateData(const QAudioFormat &format, qint64 durationUs);

    //qint64 m_pos;
    //QByteArray m_buffer;
    int frequency;
};

#endif // GENERATOR_H
