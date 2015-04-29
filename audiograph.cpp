#include "audiograph.h"

using namespace QtDataVisualization;

#define RANDOM_SCATTER // Uncomment this to switch to random scatter

const int numberOfItems = 3600;
const float curveDivider = 3.0f;
const int lowerNumberOfItems = 900;
const float lowerCurveDivider = 0.75f;

AudioGraph::AudioGraph(Q3DScatter *scatter)
    : time(0.0f)
{


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
    series->setMeshSmooth(true);
    m_graph->addSeries(series);

    m_graph->axisX()->setAutoAdjustRange(true);
    m_graph->axisY()->setAutoAdjustRange(true);
    m_graph->axisZ()->setAutoAdjustRange(true);

}

//taking some fresh data and adding it to the graph
void AudioGraph::AddData(int value){
    m_graph->seriesList().at(0)->dataProxy()->addItem(QScatterDataItem(QVector3D(0,time,time++)));
}

