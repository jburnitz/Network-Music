#ifndef AUDIOGRAPH_H
#define AUDIOGRAPH_H

#include <QtDataVisualization/q3dscatter.h>

#include <QtDataVisualization/qabstract3dseries.h>
#include <QtGui/QFont>

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QLabel>
#include <QtGui/QScreen>
#include <QtGui/QFontDatabase>
#include <QTimer>

using namespace QtDataVisualization;


class AudioGraph : public QObject
{
    Q_OBJECT
public:
    explicit AudioGraph(Q3DScatter *scatter=0);
    QWidget *container;
    Q3DScatter *m_graph;

    void AddData(int value);

protected:
    float time;
    QTimer *timer;

private slots:
    void SLOT_TimerTick();

};

#endif // AUDIOGRAPH_H
