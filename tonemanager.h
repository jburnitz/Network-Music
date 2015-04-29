#ifndef TONEMANAGER_H
#define TONEMANAGER_H

#include <QObject>

#include <QHash>
#include <QList>
#include <QAudioFormat>

class tone;

class QByteArray;
class QAudioFormat;
class QThread;

struct ToneObject{
    tone* theTone;
    QThread* theThread;
};

/** \abstract Class that acts as the controller to the tones */
class ToneManager : public QObject
{
    Q_OBJECT

public:
    ToneManager();
    ToneManager(int BaseNumberOfTones);
    ~ToneManager();

protected:
    ToneObject *AddTone(int i);

    int numberOfTones;
    int currentTone;
    int currentUncommonTone;

    int toneBaseFrequency;
    QAudioFormat m_format;

    QHash<int, QByteArray*> toneBuffers;
    QList<ToneObject*> Tones;

public slots:
    void SLOT_NumberOfTonesChanged(int newNumberOfTones);
    void SLOT_VolumeChanged(int newVolume);
    void SLOT_SetFrequency(int frequency, int often);
    void SLOT_SetBaseFrequency(int baseFrequency);
    void SLOT_InitializeTones();

signals:
    void Set_Volume(int);
    void Resume_Audio();
    void Pause_Audio();
    void Start_Audio();
};

#endif // TONEMANAGER_H
