#include "audiograph.h"

using namespace QtDataVisualization;

#define TIMER_TICK (1000)
#define TIME_RANGE (30) //30 seconds

const int numberOfItems = 3600;
const float curveDivider = 3.0f;
const int lowerNumberOfItems = 900;
const float lowerCurveDivider = 0.75f;

AudioGraph::AudioGraph(Q3DScatter *scatter)
    : time(0.0f)
{


    timer = new QTimer();

    //using a coarse timer to save some cycles
    timer->setTimerType(Qt::CoarseTimer);

    connect( timer, SIGNAL(timeout()), this, SLOT(SLOT_TimerTick()), Qt::QueuedConnection );

    m_graph = new Q3DScatter();
    container = QWidget::createWindowContainer(m_graph);

    //setting up some boilerplate windowing
    QSize screenSize = m_graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.5));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    QWidget *widget = new QWidget;
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);


    m_graph->activeTheme()->setType(Q3DTheme::ThemeEbony);
    QFont font = m_graph->activeTheme()->font();
    m_graph->activeTheme()->setFont(font);
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualitySoftLow);
    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);

    QScatterDataProxy *proxy = new QScatterDataProxy;
    QScatter3DSeries *series = new QScatter3DSeries(proxy);
    series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    //series->setMeshSmooth(true);
    series->setMesh(QAbstract3DSeries::MeshSphere);
    //series->setItemSize();
    m_graph->addSeries(series);

    m_graph->axisX()->setAutoAdjustRange(true);
    m_graph->axisX()->setTitle("Time");
    m_graph->axisX()->setMin(0);


    //m_graph->axisY()->setAutoAdjustRange(true);
    m_graph->axisY()->setMin(0);
    m_graph->axisY()->setMax(500);
    m_graph->axisZ()->setAutoAdjustRange(true);
    //m_graph->axisZ()->setLabels(QStringList("Frequency"));
    //m_graph->axisZ()->setMin(0);

    timer->start(TIMER_TICK);
}

//taking some fresh data and adding it to the graph
void AudioGraph::AddData(int value){
    m_graph->seriesList().at(0)->dataProxy()->addItem(QScatterDataItem(QVector3D(time,value,0)));

    //adjusting the graph to show all
    if(value > m_graph->axisY()->max() )
        m_graph->axisY()->setMax(value);
}

void AudioGraph::SLOT_TimerTick(){
    time++;
    m_graph->axisX()->setMax(time);

    if(m_graph->axisX()->max() > TIME_RANGE )
        m_graph->axisX()->setMin(time-TIME_RANGE);
}

