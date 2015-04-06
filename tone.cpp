#include "tone.h"

#include "audiooutput.h"
#include <QDebug>
#include <QThread>

const int DurationSeconds = 1;


tone::tone(int volumePercent, int frequency, QObject* parent)
{
    m_device = QAudioDeviceInfo::defaultOutputDevice();

    m_format.setSampleRate(44100);
    m_format.setChannelCount(2);
    m_format.setSampleSize(16);
    m_format.setCodec("audio/pcm");
    m_format.setByteOrder(QAudioFormat::LittleEndian);
    m_format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }

    m_generator = new Generator(m_format, DurationSeconds*1000000, frequency, this);

    /*
    m_audioOutput = 0;
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    m_audioOutput->setVolume(qreal(volumePercent/100.0f));
    m_generator->start();

   //m_volumeSlider->setValue(int(m_audioOutput->volume()*100.0f));

   // thread = new QThread;
    //m_audioOutput->moveToThread(thread);
    m_audioOutput->start(m_generator);
    */
}
void tone::StartPlaying(){
    m_audioOutput = 0;
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    m_audioOutput->setVolume(qreal(15/100.0f));
    m_generator->start();

   //m_volumeSlider->setValue(int(m_audioOutput->volume()*100.0f));

   // thread = new QThread;
    //m_audioOutput->moveToThread(thread);
    m_audioOutput->start(m_generator);
}

void tone::OnVolumeChanged(int value){
    qDebug() << Q_FUNC_INFO << "sound value: " << value;
    m_audioOutput->setVolume(qreal(value/100.0f));
}

tone::~tone()
{
    delete m_audioOutput;

}
void tone::Process(){

}

void tone::SetGenerator(Generator *gen){
    gen->SetPos(m_generator->GetPos());
    //m_audioOutput->suspend();
    //m_audioOutput->
    m_generator = gen;
  //  m_audioOutput->resume();
    m_audioOutput->start(m_generator);
}

void tone::SetFrequency(qreal value){
    Generator* tmp = new Generator(m_format, DurationSeconds*1000000, value, this);
    tmp->start();
//    m_audioOutput->stop();
    m_audioOutput->suspend();
    m_audioOutput->start(tmp);
    delete m_generator;
    m_generator = tmp;
}
QAudioFormat tone::GetFormat(){
    return m_format;
}

void tone::Play(){
    m_audioOutput->start();
}

void tone::Pause(){
    m_audioOutput->stop();
}
