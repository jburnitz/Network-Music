#ifndef TONEMANAGER_H
#define TONEMANAGER_H

class tone;
class Generator;
class QByteArray;
class QAudioFormat;

class QThread;

#include <QHash>;
#include <QList>

struct ToneObject{
    tone* theTone;
    QThread* theThread;
};

/** \abstract Class that acts as the controller to the tones */
class ToneManager
{
public:
    ToneManager();
    ~ToneManager();

protected:

    QHash<int, Generator*> toneGenerators;
    QHash<int, QByteArray*> toneBuffers;

    int numberOfTones;

    QByteArray* CreateToneBuffer(const QAudioFormat &format, int frequency);

    /*isomorphic datastructures. Consider:
    class ToneObject{

        public:
            tone* theTone;
            QThread* theThread;
    };

    //or
    struct ToneObject{
        tone* theTone;
        QThread* theThread;
    };

    */
    QList<ToneObject> Tones;
    //QList<QThread*> audioThreads;

public slots:
    void SLOT_NumberOfTonesChanged(int value);

    void SLOT_SetFrequency(int frequency);
};

#endif // TONEMANAGER_H
