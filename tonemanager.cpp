#include "tonemanager.h"

#include "generator.h"
#include "tone.h"

#include <QDebug>
#include <QThread>

#define TONE_COUNT (5) //base number of threads/tones

//default constructor
ToneManager::ToneManager()
{
    numberOfTones = TONE_COUNT;

}

//specify the number of tones
ToneManager::ToneManager(int BaseNumberOfTones){

    numberOfTones = BaseNumberOfTones;
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
        disconnect(this, SIGNAL(Start_Audio()), Tones[i].theTone, SLOT(DoStartPlaying()) );
    }

    currentTone = 0;
}

/** \attention modify the frequency for "niceness" */
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

    Tones[currentTone].theTone->m_generator->m_buffer = toneBuffers[frequency];
    Tones[currentTone].theTone->m_generator->m_pos = 0;

    currentTone++;

    //resetting the tone iterator
    if(currentTone >= numberOfTones)
        currentTone = 0;

}

//forwards the signal to children
void ToneManager::SLOT_VolumeChanged(int newVolume){
    emit(Set_Volume(newVolume));
}

/** \badcode Very clunky*/
void ToneManager::SLOT_NumberOfTonesChanged(int newNumberOfTones){

    qDebug() << Q_FUNC_INFO << newNumberOfTones << numberOfTones << Tones.size();
    if(Tones.size() < newNumberOfTones ){ //Tones.size() SHOULD be same as audioThreads.size()
        for( int i=Tones.size(); i< newNumberOfTones; i++){
            //Tones[i] doesn't exists and needs to be added
            AddTone(i);
            //emit( this->Start_Audio() );
            qDebug() << "adding new tone";
            connect(this, SIGNAL(Start_Audio()), Tones[i].theTone, SLOT(DoStartPlaying()) );
            emit( this->Start_Audio() );
            disconnect(this, SIGNAL(Start_Audio()), Tones[i].theTone, SLOT(DoStartPlaying()) );
        }
    }else if( newNumberOfTones > numberOfTones && newNumberOfTones < Tones.size() ){ //tone already exists and needs resuming
        for( int i=numberOfTones; i< newNumberOfTones; i++){

            qDebug() << "resuming previously made toneplayer";
            //connect, send signal, disconnect
            connect(this, SIGNAL(Resume_Audio()), Tones[i].theTone, SLOT(DoResumeAudio()) );
            emit( this->Resume_Audio() );
            disconnect(this, SIGNAL(Resume_Audio()), Tones[i].theTone, SLOT(DoResumeAudio()) );

        }
    }else if(Tones.size() > newNumberOfTones ){
        for(int i=newNumberOfTones; i<Tones.size(); i++){

            qDebug() << "pausing the tone["<<i<<"]";
            //turn these end tones off
            connect(this, SIGNAL(Pause_Audio()), Tones[i].theTone, SLOT(DoPauseAudio()) );
            emit( this->Pause_Audio() );
            disconnect(this, SIGNAL(Pause_Audio()), Tones[i].theTone, SLOT(DoPauseAudio()) );
        }
    }else{
        //do nothing if its the same size
    }

    //adjust the iterator for other stuff
    numberOfTones = newNumberOfTones;

}

//adds a new tone and thread and starts it
void ToneManager::AddTone(int i){

    qDebug() << "Adding new tone["<<i<<"]";

    ToneObject newTone;
    newTone.theTone = new tone(1, 200+(i*i), NULL );
    newTone.theThread = new QThread;
    Tones.append( newTone );

    //signals needed to talk to tone after moved to another thread
    connect ( this, SIGNAL(Set_Volume(int)), Tones[i].theTone, SLOT(OnVolumeChanged(int)) );
    connect(this, SIGNAL(Start_Audio()), Tones[i].theTone, SLOT(DoStartPlaying()) );

    Tones[i].theTone->moveToThread(Tones[i].theThread);
    //Tones[i].theThread

}

