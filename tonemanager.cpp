#include "tonemanager.h"

#include "generator.h"
#include "tone.h"

#include <QDebug>
#include <QThread>
#include <QTime>

#define TONE_COUNT (5) //base number of threads/tones

//constructor
ToneManager::ToneManager(int BaseNumberOfTones=TONE_COUNT){

    toneBaseFrequency=0;
    currentUncommonTone=0;
    numberOfTones=BaseNumberOfTones;

    qsrand(QTime::currentTime().msec());

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
}

ToneManager::~ToneManager()
{
    //delete this->Tones;

}

void ToneManager::SLOT_InitializeTones(){

    qDebug() << Q_FUNC_INFO << "    begin";

    //Set up the audio devices
    qDebug() << "    Setting up audio devices";

    for(int i=0; i<numberOfTones; i++){
        ToneObject* to = AddTone(i);

        //signals needed to talk to tone after moved to another thread
        connect( this, SIGNAL( Set_Volume(int)), to->theTone, SLOT(OnVolumeChanged(int)) );
        connect( this, SIGNAL( Start_Audio()), to->theTone, SLOT(DoStartPlaying()) );

        //start the tone players
        qDebug() << Q_FUNC_INFO << "   Starting players";
        emit( Start_Audio() );
        disconnect(this, SIGNAL(Start_Audio()), to->theTone, SLOT(DoStartPlaying()) );
    }

    currentTone = 0;
}

void ToneManager::SLOT_SetBaseFrequency(int baseFrequency){
    toneBaseFrequency = baseFrequency;
}

/** \attention modify the frequency for "niceness" */
void ToneManager::SLOT_SetFrequency(int frequency, int often){

    frequency+=toneBaseFrequency;

    int toneToModify;

    if(frequency < 50)
        frequency = frequency+(qrand()%80);

    else if(frequency > 2000)
        frequency = 2000-(qrand()%1500);

    //time consuming operation
    if( !toneBuffers.contains(frequency) ){
        qDebug() << Q_FUNC_INFO << "    BufferCache Frequency miss for :" << frequency;
        //toneBuffers.insert(frequency, Generator::GenerateData( Tones[0]->theTone->format(), frequency ) );
        toneBuffers.insert(frequency, Generator::GenerateData( m_format, frequency ) );
    }

    //this is normal changing tone
    if(often){
        toneToModify = currentTone;
        currentTone++;

        //resetting the tone iterator
        if(currentTone >= numberOfTones){ currentTone = 2; }

    }else{//the not often case, we'll modify just the lowest tones
        toneToModify = currentUncommonTone;
        currentUncommonTone++;

        //resetting the tone iterator
        if(currentUncommonTone >= 2){ currentUncommonTone = 0; }
    }

    qDebug() << Q_FUNC_INFO << "    Setting frequency:"<< frequency << "on Tones["<<toneToModify<<"]";
    Tones[toneToModify]->theTone->m_generator->m_buffer = toneBuffers[frequency];
    Tones[toneToModify]->theTone->m_generator->m_pos = 0;
}

//forwards the signal to children
void ToneManager::SLOT_VolumeChanged(int newVolume){
    //qDebug() << Q_FUNC_INFO << "    begin";

    emit( Set_Volume(newVolume) );

    //qDebug() << Q_FUNC_INFO << "    end";
}

/** \badcode Very clunky*/
void ToneManager::SLOT_NumberOfTonesChanged(int newNumberOfTones){

    qDebug() << Q_FUNC_INFO << newNumberOfTones << numberOfTones << Tones.size();
    if(Tones.size() < newNumberOfTones ){ //if the counter has increased
        for( int i=Tones.size(); i< newNumberOfTones; i++){
            //Tones[i] doesn't exists and needs to be added
            ToneObject* to = AddTone(i);
            //emit( this->Start_Audio() );
            qDebug() << "adding new tone";
            connect(this, SIGNAL(Start_Audio()), to->theTone, SLOT(DoStartPlaying()) );
            connect(this, SIGNAL(Set_Volume(int)), to->theTone, SLOT(OnVolumeChanged(int)), Qt::QueuedConnection);
            emit( Start_Audio() );
            disconnect(this, SIGNAL(Start_Audio()), to->theTone, SLOT(DoStartPlaying()) );
        }
    }else if( newNumberOfTones > numberOfTones && newNumberOfTones < Tones.size() ){ //tone already exists and needs resuming
        for( int i=numberOfTones; i< newNumberOfTones; i++){

            qDebug() << "resuming previously made toneplayer";
            //connect, send signal, disconnect
            connect(this, SIGNAL(Resume_Audio()), Tones[i]->theTone, SLOT(DoResumeAudio()) );
            emit( this->Resume_Audio() );
            disconnect(this, SIGNAL(Resume_Audio()), Tones[i]->theTone, SLOT(DoResumeAudio()) );

        }
    }else if(Tones.size() > newNumberOfTones ){//turn these end tones off
        for(int i=newNumberOfTones; i<Tones.size(); i++){

            qDebug() << "pausing the tone["<<i<<"]";

            connect(this, SIGNAL(Pause_Audio()), Tones[i]->theTone, SLOT(DoPauseAudio()) );
            emit( this->Pause_Audio() );
            disconnect(this, SIGNAL(Pause_Audio()), Tones[i]->theTone, SLOT(DoPauseAudio()) );
        }
    }else{
        //do nothing if its the same size
    }

    //adjust the iterator for other stuff
    numberOfTones = newNumberOfTones;

}

//adds a new tone and thread and starts it
ToneObject* ToneManager::AddTone(int i){

    qDebug() << Q_FUNC_INFO << "    begin";

    qDebug() << Q_FUNC_INFO << "    Adding new tone["<<i<<"]";

    ToneObject *newTone = new ToneObject;
    if( i%2)
        newTone->theTone = new tone(1, toneBaseFrequency+100*i+400, NULL );
    else
        newTone->theTone = new tone(1, toneBaseFrequency+(100*i)+i + 300, NULL );

    newTone->theThread = new QThread;

    newTone->theTone->moveToThread( newTone->theThread );
    newTone->theThread->start();

    Tones.prepend( newTone );

    qDebug() << Q_FUNC_INFO << "    end";
    return newTone;
}

