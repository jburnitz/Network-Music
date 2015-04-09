#include "tonemanager.h"

#include <QDebug>


ToneManager::ToneManager()
{

}

ToneManager::~ToneManager()
{

}

void ToneManager::SLOT_NumberOfTonesChanged(int value){

    qDebug() << Q_FUNC_INFO << value << numberOfTones << Tones.size();
    if(Tones.size() < value ){ //Tones.size() SHOULD be same as audioThreads.size()
        for( int i=Tones.size(); i< value; i++){
            //Tones[i] doesn't exists and needs to be added
            AddTone(i);
            //emit( this->Start_Audio() );
            qDebug() << "adding new tone";
            connect(this, SIGNAL(Start_Audio()), Tones[i], SLOT(DoStartPlaying()) );
            emit( this->Start_Audio() );
            disconnect(this, SIGNAL(Start_Audio()), Tones[i], SLOT(DoStartPlaying()) );
        }
    }else if( value > numberOfTones && value < Tones.size() ){ //tone already exists and needs resuming
        for( int i=numberOfTones; i< value; i++){

            qDebug() << "resuming previously made toneplayer";
            //connect, send signal, disconnect
            connect(this, SIGNAL(Resume_Audio()), Tones[i], SLOT(DoResumeAudio()) );
            emit( this->Resume_Audio() );
            disconnect(this, SIGNAL(Resume_Audio()), Tones[i], SLOT(DoResumeAudio()) );

        }
    }else if(Tones.size() > value ){
        for(int i= value; i<Tones.size(); i++){

            qDebug() << "pausing the tone["<<i<<"]";
            //turn these end tones off
            connect(this, SIGNAL(Pause_Audio()), Tones[i], SLOT(DoPauseAudio()) );
            emit( this->Pause_Audio() );
            disconnect(this, SIGNAL(Pause_Audio()), Tones[i], SLOT(DoPauseAudio()) );
        }
    }else{
        //do nothing if its the same size
    }
    //adjust the iterator for other stuff
    numberOfTones = value;

}

//Creates a bytearray of pure (sin) tones as specified by the frequency
QByteArray* AudioTest::CreateToneBuffer(const QAudioFormat &format, int frequency){

    qint64 durationUs = 1000000;
    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channelCount() * channelBytes;

    qint64 length = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8)) * durationUs / 100000;

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

    return m_buffer;
}
