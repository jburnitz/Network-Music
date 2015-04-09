#include "tonemanager.h"

#include "generator.h"
#include "tone.h"

#include <QDebug>

#define TONE_COUNT (5) //base number of threads/tones


ToneManager::ToneManager()
{
    numberOfTones = TONE_COUNT;

}

ToneManager::~ToneManager()
{
    //delete this->Tones;

}

void ToneManager::SLOT_InitializeTones(){

    //Set up the audio devices
    qDebug() << "Setting up audio devices";

    for(int i=0; i<numberOfTones; i++){
        AddTone(i);

        //start the tone players
        qDebug() << "Starting players";
        emit(this->Start_Audio());
        disconnect(this, SIGNAL(Start_Audio()), Tones[i], SLOT(DoStartPlaying()) );
    }

    currentTone = 0;

    qDebug() << "Creating new Packetcapture thread";
    PacketCaptureThread = new QThread();
    //setup happens later

}

void ToneManager::SLOT_SetFrequency(int frequency){
    //this->Tones[0].theThread
    //this->Tones[0].theTone

    if(frequency < 50)
        frequency = frequency*frequency;
   // else if(frequency > 1000)
        //frequency = 1000;

    //time consuming operation
    if( !toneBuffers.contains(frequency) ){
        qDebug() << Q_FUNC_INFO << "Buffer Frequency miss for :" << frequency << " hz";
        toneBuffers.insert(frequency, Generator::GenerateData(Tones[currentTone].theTone->format(), frequency ) );
    }

    Tones[currentTone]->m_generator->m_buffer = toneBuffers[frequency];
    Tones[currentTone]->m_generator->m_pos = 0;

    currentTone++;

    //resetting the tone iterator
    if(currentTone >= numberOfTones)
        currentTone = 0;

}

/** \badcode Very clunky*/
void ToneManager::SLOT_NumberOfTonesChanged(int newNumberOfTones){

    qDebug() << Q_FUNC_INFO << value << numberOfTones << Tones.size();
    if(Tones.size() < newNumberOfTones ){ //Tones.size() SHOULD be same as audioThreads.size()
        for( int i=Tones.size(); i< value; i++){
            //Tones[i] doesn't exists and needs to be added
            AddTone(i);
            //emit( this->Start_Audio() );
            qDebug() << "adding new tone";
            connect(this, SIGNAL(Start_Audio()), Tones[i], SLOT(DoStartPlaying()) );
            emit( this->Start_Audio() );
            disconnect(this, SIGNAL(Start_Audio()), Tones[i], SLOT(DoStartPlaying()) );
        }
    }else if( newNumberOfTones > numberOfTones && value < Tones.size() ){ //tone already exists and needs resuming
        for( int i=numberOfTones; i< value; i++){

            qDebug() << "resuming previously made toneplayer";
            //connect, send signal, disconnect
            connect(this, SIGNAL(Resume_Audio()), Tones[i], SLOT(DoResumeAudio()) );
            emit( this->Resume_Audio() );
            disconnect(this, SIGNAL(Resume_Audio()), Tones[i], SLOT(DoResumeAudio()) );

        }
    }else if(Tones.size() > newNumberOfTones ){
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
    numberOfTones = newNumberOfTones;

}

//adds a new tone and thread and starts it
void AudioTest::AddTone(int i){

    qDebug() << "Adding new tone["<<i<<"]";
    //tone(toneduration(seconds), frequency(hz), parent)
    Tones.append(  new tone(1, 200+(i*i), NULL)  );
    audioThreads.append( new QThread );

    //signals needed to talk to tone after moved to another thread
    connect (this->m_volumeSlider, SIGNAL(valueChanged(int)), Tones[i], SLOT(OnVolumeChanged(int)) );
    connect(this, SIGNAL(Start_Audio()), Tones[i], SLOT(DoStartPlaying()) );

    Tones[i]->moveToThread(audioThreads[i]);
    audioThreads[i]->start();
}

