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

//used only for enumerating devices for GUI elements
#ifdef WIN32
#include <win/WpdPack/Include/pcap.h>
#else
#include <pcap.h>
#endif

#include "audiooutput.h"
#include "tone.h"
#include "packetcapturer.h"
#include "tonemanager.h"

#define SUSPEND_LABEL   "Suspend playback"
#define RESUME_LABEL    "Resume playback"
#define VOLUME_LABEL    "Volume:"

#define TONE_MAX (15) //Maximum number of threads
#define TONE_MIN (3)
#define TONE_COUNT (5) //default tone count

/** \abstract Controller class that manages the tone players and receives input from pcap */
AudioTest::AudioTest()
{
    PacketCaptureThread = NULL;

    srand(time(NULL));


    //set up the graphical stuff
    qDebug() << "setting up GUI";
    initializeWindow();
    qDebug() << Q_FUNC_INFO << "Window initialized";

}

//setup the non graphical elements
void AudioTest::Setup(){
    qDebug() << Q_FUNC_INFO << " begin";

    qDebug() << Q_FUNC_INFO << "   creating ToneManager";
    toneManager = new ToneManager(TONE_COUNT);
    ToneManagerThread = new QThread;

    qDebug() << Q_FUNC_INFO << "   connecting slots";
    connect( this, SIGNAL(SIGNAL_BEGIN_TONES()), this->toneManager, SLOT(SLOT_InitializeTones()) );
    connect( this->m_volumeSlider, SIGNAL(valueChanged(int)), this->toneManager, SLOT(SLOT_VolumeChanged(int)) );
    connect( this->m_numberOfTones, SIGNAL(valueChanged(int)), this->toneManager, SLOT(SLOT_NumberOfTonesChanged(int)) , Qt::QueuedConnection);
    connect( m_baseFrequency, SIGNAL(valueChanged(int)), this->toneManager, SLOT(SLOT_SetBaseFrequency(int)), Qt::QueuedConnection );

    //connect(m_frequencySlider, SIGNAL(valueChanged(int)), this, SLOT(frequencyChanged(int)));

    qDebug() << Q_FUNC_INFO << "   moving ToneManager to own thread";
    toneManager->moveToThread(ToneManagerThread);

    qDebug() << Q_FUNC_INFO << "   Starting ToneManager thread";
    ToneManagerThread->start();

    qDebug() << Q_FUNC_INFO << "   Sending begin signal";
    emit( SIGNAL_BEGIN_TONES() );

    qDebug() << Q_FUNC_INFO << " done";

}

//set up the graphical elements
void AudioTest::initializeWindow()
{
    qDebug()<< Q_FUNC_INFO << " begin";

    QScopedPointer<QWidget> window(new QWidget);
    QScopedPointer<QVBoxLayout> layout(new QVBoxLayout);

    qDebug() << "   creating combo box for devices";
    m_networkDeviceBox = new QComboBox(this);

    /** \attention Consider moving this device finding to a non-graphical context */
    pcap_if_t *alldevs;
    pcap_if_t *d;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Retrieve the device list on the local machine
    qDebug() << "   finding all pcap devices";
    if( pcap_findalldevs( &alldevs, errbuf)  == -1 )
    {
        m_networkDeviceBox->addItem("No Devices, are you root?");
        qWarning() << "Error in pcap_findalldevs:" <<errbuf;

    } else {
        // Print the list
        for(d=alldevs; d; d=d->next)
        {
            qDebug() << "       device:" << d->name;
            m_networkDeviceBox->addItem( QString(d->name)/*.append(" ").append(d->description)*/  );

        }
        qDebug() << "   got all devices";
        layout->addWidget(m_networkDeviceBox);
        qDebug() << "  widget added";
    }

    qDebug() << "    freeing devices";
    pcap_freealldevs( alldevs);
    qDebug() << "    devices freed";

    qDebug()<< " creating tonecount SpinBox";
    m_numberOfTonesLabel = new QLabel("Number of Tones");

    m_numberOfTones = new QSpinBox();
    m_numberOfTones->setWhatsThis("Set the number of simultaneous tone players");
    m_numberOfTones->setMaximum(TONE_MAX);
    m_numberOfTones->setMinimum(TONE_MIN);
    m_numberOfTones->setValue(TONE_COUNT);

    m_baseFrequencyLabel = new QLabel("Base Tone Frequency");
    m_baseFrequency = new QSpinBox();
    m_baseFrequency->setRange(0,1000);
    m_baseFrequency->setSingleStep(5);

    QHBoxLayout *toneChooserLayoutBox = new QHBoxLayout;
    toneChooserLayoutBox->addWidget(m_numberOfTonesLabel);
    toneChooserLayoutBox->addWidget(m_numberOfTones);

    toneChooserLayoutBox->addWidget(m_baseFrequencyLabel);
    toneChooserLayoutBox->addWidget(m_baseFrequency);

    layout->addLayout(toneChooserLayoutBox);
    qDebug() << "    tone number chooser created";

    qDebug() << "   Creating volumeSlider";
    QHBoxLayout *volumeBox = new QHBoxLayout;
    m_volumeLabel = new QLabel;
    m_volumeLabel->setText(tr(VOLUME_LABEL));
    m_volumeSlider = new QSlider(Qt::Horizontal);
    m_volumeSlider->setMinimum(0);
    m_volumeSlider->setMaximum(100);
    m_volumeSlider->setSingleStep(2);
    qDebug() << "   volumeSlider Created";

    qDebug() << "   Adding frequency slider";
    m_frequencySlider = new QSlider(Qt::Horizontal);
    m_frequencySlider->setMinimum(1);
    m_frequencySlider->setMaximum(300);
    m_frequencySlider->setSingleStep(1);
    m_frequencySlider->setSliderPosition(4);
    qDebug() << "   Frequency slider added";

    qDebug() << "   adding sliders to slider layout";
    volumeBox->addWidget(m_volumeLabel);
    volumeBox->addWidget(m_volumeSlider);
    volumeBox->addWidget(m_frequencySlider);

    qDebug() << "   adding Slider layout to main layout ";
    layout->addLayout(volumeBox);

    QPushButton* m_pcapButton = new QPushButton("Start Packet Music", this);
    connect( m_pcapButton, SIGNAL(clicked()), this, SLOT(PcapButtonPressed()) );
    layout->addWidget(m_pcapButton);

    m_statusBar = new QStatusBar();
    m_statusBarLabelString = new QString("ready");
    m_statusBarLabel = new QLabel( *m_statusBarLabelString);
    m_statusBar->addPermanentWidget(m_statusBarLabel, 1);

    //setting up the graph pointers
    //SetupGraph();

    layout->addWidget(m_statusBar);

    window->setLayout(layout.data());

    layout.take(); // ownership transferred

    setCentralWidget(window.data());
    QWidget *const windowPtr = window.take(); // ownership transferred

    windowPtr->show();

    qDebug() << "   finished setting up GUI";
    qDebug()<< Q_FUNC_INFO << " end";
}


/** \brief Creating a new packetcapturer instance and moves it to it's own thread */
void AudioTest::PcapButtonPressed(){
    qDebug()<< Q_FUNC_INFO << " begin";

    if( PacketCaptureThread == NULL ){
        qDebug() << "   Creating new Packetcapture thread";
        PacketCaptureThread = new QThread();
    }

    qDebug() << "   Creating new PacketCapturer with dev=" << m_networkDeviceBox->currentText();
    pk = new PacketCapturer( m_networkDeviceBox->currentText().toStdString().c_str() );

    qDebug() << "   Moving pk to packetCaptureThread";
    pk->moveToThread(PacketCaptureThread);

    qDebug() << "   Connecting slots";
    connect(this, SIGNAL(SIGNAL_BEGIN_CAPTURE()), pk, SLOT(SLOT_CAPTURE()) );
    connect(pk, SIGNAL(SIG_NEW_TONE(int, int)), toneManager, SLOT(SLOT_SetFrequency(int, int)) );
    connect(pk, SIGNAL(SIG_NEW_TONE(int, int)), this, SLOT(UpdateChart(int,int)), Qt::QueuedConnection );

    qDebug() << "   Starting pkThread";
    PacketCaptureThread->start();

    m_statusBarLabel->setText("playing");

    emit( SIGNAL_BEGIN_CAPTURE() );

    qDebug()<< Q_FUNC_INFO << " end";
}

//since this a graphical object, I'm processing on the UI thread
void AudioTest::UpdateChart(int freq, int priority){


}
