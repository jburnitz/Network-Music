#include "tone.h"

#include "audiooutput.h"
#include <QDebug>
#include <QThread>

const int DurationSeconds = 1;

/** \abstract Container that manages the audio playing of individual tones */
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
    m_audioOutput = 0;

    //creating this own generator may be going away with a generator reference from parent
    m_generator = new Generator(m_format, DurationSeconds*1000000, frequency, this);

}
void tone::DoStartPlaying(){

    //delete m_audioOutput;

    qDebug() << Q_FUNC_INFO << "    begin";
    //m_audioOutput = 0;
    m_audioOutput = new QAudioOutput(m_device, m_format, this);
    m_audioOutput->setVolume(qreal(15/100.0f));
    m_generator->start();

    m_audioOutput->start(m_generator);

    qDebug() << Q_FUNC_INFO << "    end";
}

void tone::DoResumeAudio(){

    //bad assumption
    if( !m_generator->isOpen() ){
        m_generator->start();
    }
    if(m_audioOutput == 0){ // 0 is null, right?
        m_audioOutput = new QAudioOutput(m_device, m_format, this);
        m_audioOutput->setVolume(qreal(15/100.0f));
        m_audioOutput->start(m_generator);
    }

    m_audioOutput->resume();
}

//pauses the player if needed
void tone::DoPauseAudio(){
    qDebug() << "Pausing on thread" << QThread::currentThreadId();
    m_audioOutput->suspend();
}

void tone::OnVolumeChanged(int value){
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
