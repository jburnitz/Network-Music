/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QDebug>
#include <QVBoxLayout>
#include <qmath.h>
#include <qendian.h>

#include <QHash>

#include <QThread>

#include <pcap.h>

#include "audiooutput.h"
#include "tone.h"
#include "packetcapturer.h"

#define PUSH_MODE_LABEL "Enable push mode"
#define PULL_MODE_LABEL "Enable pull mode"
#define SUSPEND_LABEL   "Suspend playback"
#define RESUME_LABEL    "Resume playback"
#define VOLUME_LABEL    "Volume:"


AudioTest::AudioTest()
    :   m_pullTimer(new QTimer(this))
    ,   m_modeButton(0)
    ,   m_suspendResumeButton(0)
    ,   m_audioDeviceBox(0)

{

    PacketCaptureThread = NULL;

    //set up the graphical stuff
    qDebug() << "setting up GUI";
    initializeWindow();
    qDebug() << Q_FUNC_INFO << "Window initialized";

}
void AudioTest::Setup(){

    //Set up the audio devices
    qDebug() << "Setting up audio devices";
    for(int i=0; i<TONE_COUNT; i++){
        //tone(toneduration(seconds), frequency(hz), parent)
        qDebug() << "creating tones ["<<i<<"]";
        Tones[i] = new tone(1, 200+(i*10), NULL);
        audioThreads[i] = new QThread;

        connect (this->m_volumeSlider, SIGNAL(valueChanged(int)), Tones[i], SLOT(OnVolumeChanged(int)) );
        connect(this, SIGNAL(Start_Audio()), Tones[i], SLOT(StartPlaying()) );

        Tones[i]->moveToThread(audioThreads[i]);
        audioThreads[i]->start();
    }

    //start the tone players
    qDebug() << "Starting players";
    emit(this->Start_Audio());

    currentTone = 0;

    qDebug() << "Creating new Packetcapture thread";
    PacketCaptureThread = new QThread();

}

//set up the graphical elements
void AudioTest::initializeWindow()
{
    QScopedPointer<QWidget> window(new QWidget);
    QScopedPointer<QVBoxLayout> layout(new QVBoxLayout);

    /*
    m_audioDeviceBox = new QComboBox(this);
    const QAudioDeviceInfo &defaultDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
    m_audioDeviceBox->addItem(defaultDeviceInfo.deviceName(), qVariantFromValue(defaultDeviceInfo));
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)) {
        if (deviceInfo != defaultDeviceInfo)
            m_audioDeviceBox->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    }
    connect(m_audioDeviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(m_audioDeviceBox);
    */

    qDebug() << "creating combo box for devices";
    m_networkDeviceBox = new QComboBox(this);

    pcap_if_t *alldevs;
    pcap_if_t *d;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Retrieve the device list on the local machine
   if( pcap_findalldevs( &alldevs, errbuf)  == -1 )
    {
        m_networkDeviceBox->addItem("No Devices, are you root?");
        qWarning() << "Error in pcap_findalldevs:" <<errbuf;

   } else {
        // Print the list
        qDebug() << "getting devices";
        for(d=alldevs; d; d=d->next)
        {
            qDebug() << "device:" << d->name;
            m_networkDeviceBox->addItem( QString(d->name)/*.append(" ").append(d->description)*/  );

        }
        qDebug() << "got all devices";
         layout->addWidget(m_networkDeviceBox);
         qDebug() << "widget added";
   }

   qDebug() << "freeing devices";
   pcap_freealldevs( alldevs);
   qDebug() << "devices freed";


    qDebug() << "Creating volumeSlider";
    QHBoxLayout *volumeBox = new QHBoxLayout;
    m_volumeLabel = new QLabel;
    m_volumeLabel->setText(tr(VOLUME_LABEL));
    m_volumeSlider = new QSlider(Qt::Horizontal);
    m_volumeSlider->setMinimum(0);
    m_volumeSlider->setMaximum(100);
    m_volumeSlider->setSingleStep(2);

    qDebug() << "volumeSlider Created";
    //connect(m_volumeSlider, SIGNAL(valueChanged(int)), t, SLOT(volumeChanged(int)));

    qDebug() << "Adding frequency slider";
    m_frequencySlider = new QSlider(Qt::Horizontal);
    m_frequencySlider->setMinimum(1);
    m_frequencySlider->setMaximum(300);
    m_frequencySlider->setSingleStep(1);
    m_frequencySlider->setSliderPosition(4);

    qDebug() << "Frequency slider added: connecting...";
    connect(m_frequencySlider, SIGNAL(valueChanged(int)), this, SLOT(frequencyChanged(int)));

    qDebug() << "Frequency slider added: connecting...connected!";

    qDebug() << "adding widgets to QBox layout";
    volumeBox->addWidget(m_volumeLabel);
    volumeBox->addWidget(m_volumeSlider);
    volumeBox->addWidget(m_frequencySlider);

    qDebug() << "adding Sliders to main layout ";
    layout->addLayout(volumeBox);

    QPushButton* m_pcapButton = new QPushButton("Start Packet Music", this);
    connect( m_pcapButton, SIGNAL(clicked()), this, SLOT(PcapButtonPressed()) );
    layout->addWidget(m_pcapButton);

    window->setLayout(layout.data());

    layout.take(); // ownership transferred

    setCentralWidget(window.data());
    QWidget *const windowPtr = window.take(); // ownership transferred

    windowPtr->show();

    qDebug() << "finished setting up GUI";

}

void AudioTest::PcapButtonPressed(){
    qDebug()<< Q_FUNC_INFO;

    qDebug() << "Creating new PacketCapturer with dev=" << m_networkDeviceBox->currentText();
    pk = new PacketCapturer( m_networkDeviceBox->currentText().toStdString().c_str() );

    qDebug() << "Moving pk to packetCaptureThread";
    pk->moveToThread(PacketCaptureThread);

    connect(this, SIGNAL(SIGNAL_BEGIN_CAPTURE()), pk, SLOT(SLOT_CAPTURE()) );
    connect(pk, SIGNAL(SIG_NEW_TONE(int)), this, SLOT(SetFrequency(int)) );

    qDebug() << "Starting pkThread";
    PacketCaptureThread->start();

    emit( SIGNAL_BEGIN_CAPTURE() );
}

AudioTest::~AudioTest()
{
    delete pk;
}

void AudioTest::deviceChanged(int index)
{
    /*
    m_pullTimer->stop();
    m_generator->stop();
    m_audioOutput->stop();
    m_audioOutput->disconnect(this);
    m_device = m_deviceBox->itemData(index).value<QAudioDeviceInfo>();
    createAudioOutput();
    */
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

void AudioTest::frequencyChanged(int frequency){

    frequency *=30;
    SetFrequency(frequency);
}

void AudioTest::SetFrequency(int frequency){

    if(frequency < 100)
        frequency = 100;
    else if(frequency > 1000)
        frequency = 1000;

    if( !toneBuffers.contains(frequency) ){
        qDebug() << Q_FUNC_INFO << "Buffer Frequency miss for :" << frequency << " hz";
        toneBuffers.insert(frequency, CreateToneBuffer(Tones[currentTone]->GetFormat(), frequency));
    }

    Tones[currentTone]->m_generator->m_buffer = toneBuffers[frequency];
    Tones[currentTone]->m_generator->m_pos = 0;

    currentTone++;

    //resetting the tone counter
    if(currentTone >= TONE_COUNT)
        currentTone = 0;
}
