#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow()
{
    labelInitDot = new QLabel("Initial dot: ");
    labelInitSegmentBegin = new QLabel("Segment begin: ");
    labelInitSegmentEnd = new QLabel("Segment end: ");
    labelIter = new QLabel("Number of iterations: ");
    labelX = new QLabel("Domains of X axis: ");
    labelY = new QLabel("Domains of Y axis: ");
    editInitDot = new QLineEdit("1 1");
    editInitSegmentBegin = new QLineEdit("-1 -1");
    editInitSegmentEnd = new QLineEdit("1 1");
    editIter = new QLineEdit("10");
    editX = new QLineEdit("-2 2");
    editY = new QLineEdit("-2 2");
    button_go = new QPushButton("Generate");
    radio_dots = new QRadioButton("Dots iterations");
    radio_segments = new QRadioButton("Segments iterations");
    optionIntersection = new QCheckBox("Check curves intersections");
    layoutSettings = new QVBoxLayout();
    lineInitDot = new QHBoxLayout();
    lineInitSegmentBegin = new QHBoxLayout();
    lineInitSegmentEnd = new QHBoxLayout();
    lineIterations = new QHBoxLayout();
    lineDomainX = new QHBoxLayout();
    lineDomainY = new QHBoxLayout();
    widgetCentral = new QWidget();

    editInitDot->setPalette(Qt::gray);
    editInitSegmentBegin->setPalette(Qt::gray);
    editInitSegmentEnd->setPalette(Qt::gray);
    editIter->setPalette(Qt::gray);
    editX->setPalette(Qt::gray);
    editY->setPalette(Qt::gray);
    button_go->setPalette(Qt::gray);

    lineInitDot->addWidget(labelInitDot);
    lineInitDot->addWidget(editInitDot);
    lineInitSegmentBegin->addWidget(labelInitSegmentBegin);
    lineInitSegmentBegin->addWidget(editInitSegmentBegin);
    lineInitSegmentEnd->addWidget(labelInitSegmentEnd);
    lineInitSegmentEnd->addWidget(editInitSegmentEnd);
    lineIterations->addWidget(labelIter);
    lineIterations->addWidget(editIter);
    lineIterations->addWidget(optionIntersection);
    lineDomainX->addWidget(labelX);
    lineDomainX->addWidget(editX);
    lineDomainY->addWidget(labelY);
    lineDomainY->addWidget(editY);

    layoutSettings->insertSpacing(0, 25);
    layoutSettings->addWidget(radio_dots);
    layoutSettings->addWidget(radio_segments);
    layoutSettings->addLayout(lineInitDot);
    layoutSettings->addLayout(lineInitSegmentBegin);
    layoutSettings->addLayout(lineInitSegmentEnd);
    layoutSettings->addLayout(lineIterations);
    layoutSettings->addLayout(lineDomainX);
    layoutSettings->addLayout(lineDomainY);
    layoutSettings->addWidget(button_go);

    widgetCentral->setLayout(layoutSettings);
    mode = DOT_ITERATIONS;

    setCentralWidget(widgetCentral);
    setWindowTitle("2D Iterations 0.4");
    setPalette(Qt::black);
    setAutoFillBackground(true);

    connect(button_go, SIGNAL(clicked()), this,
        SLOT(refreshData()));
    connect(radio_dots, SIGNAL(toggled(bool)), this,
        SLOT(setRunMode(bool)));
    connect(optionIntersection, SIGNAL(toggled(bool)),
        this, SLOT(setCheckIntersections(bool)));

    optionIntersection->setChecked(false);
    radio_dots->setChecked(true);

    check_intersection = 0;
}


bool MainWindow::retrieveIterationNumber(QString &src)
{
    bool status;
    long int result;

    result = src.toLong(&status);
    iteration_number = (status)? result : iteration_number;

    return status;
}


bool MainWindow::retrieveDomains(QString &src, char target)
{
    QStringList list;
    bool status;
    double result1, result2;

    list = src.split(" ", QString::SkipEmptyParts);

    if (list.length() != 2)
        return false;

    result1 = list[0].toDouble(&status);

    if (!status)
        return false;

    result2 = list[1].toDouble(&status);

    if (!status)
        return false;


    switch (target)
    {
        case 'X':
            lowerX = result1;
            higherX = result2;
            break;

        case 'Y':
            lowerY = result1;
            higherY = result2;
            break;

        case '0':
            initDotX = result1;
            initDotY = result2;
            break;

        case 'B':
            segmentBeginX = result1;
            segmentBeginY = result2;

        case 'E':
            segmentEndX = result1;
            segmentEndY = result2;

        default:
            // Incorrect usage!
            break;
    }

    return true;
}


void MainWindow::fillAxis(double lo, double hi, QCategoryAxis *axis)
{
    double rounded = floor(lo);
    double supremum = ceil(hi) + 0.1;

    axis->setRange(lo, hi);

    while (rounded < supremum)
    {
        axis->append(QString::number((int) rounded),
                     (int) rounded);
        rounded++;
    }
}


void MainWindow::fillSeries(QScatterSeries *series)
{
    QPointF point(initDotX, initDotY);
    int i = 0;

    *series << point;

    while (i++ < iteration_number)
    {
        point = formulaIter(point);
        *series << point;
    }
}

/**
 *  The algorhythm: it checks length of X's of the
 *  segment, then divide it by length of OX.
 *  For all X's is given MAX_PRECISION dots. From that
 *  equations the following precision is calculated.
 */
void MainWindow::fillSeries(QLineSeries *series, bool direction)
{
    QPointF point;
    int dotsPerSegment;
    long int round = 0;
    double step;

    dotsPerSegment = (int) ceil(MAX_PRECISION *
        (segmentEndX-segmentBeginX) / (higherX-lowerX));
    step = (segmentEndX-segmentBeginX) / dotsPerSegment;

    for (int i = 0; i < dotsPerSegment; i++)
    {
        dataX[i] = segmentBeginX + step*i;
        dataY[i] = givenSegmentOrdinate(segmentBeginX + step*i);
    }

    while (round++ < iteration_number)
    {
        for (int i = 0; i < dotsPerSegment; i++)
        {
            point = formulaIter(QPointF(dataX[i], dataY[i]), direction);
            dataX[i] = point.x();
            dataY[i] = point.y();
        }
    }

    for (int i = 0; i < dotsPerSegment; i++)
        *series << QPointF(dataX[i], dataY[i]);
}


double MainWindow::distance(QPointF dot1, QPointF dot2)
{
    return sqrt((dot1.x()-dot2.x())*(dot1.x()-dot2.x()) +
                (dot1.y()-dot2.y())*(dot1.y()-dot2.y()));
}


double MainWindow::distance(double dot1X, double dot1Y,
        double dot2X, double dot2Y)
{
    return sqrt((dot1X-dot2X)*(dot1X-dot2X) +
                (dot1Y-dot2Y)*(dot1Y-dot2Y));
}

/**
 *  Returns Y of the line represented by two
 *  different dots given in class data.
 *
 *  @param X specifies the argument
 *  @return Y expression due to given X
 *
 */
double MainWindow::givenSegmentOrdinate(double X)
{
    return X*((segmentEndY-segmentBeginY)/(segmentEndX-segmentBeginX)) +
            ((segmentBeginY*segmentEndX - segmentBeginX*segmentEndY) /
             (segmentEndX-segmentBeginX));
}


bool MainWindow::segmentIntersection(QPointF A1, QPointF A2,
        QPointF B1, QPointF B2)
{
    if (((B1.x()<B2.x())?B1.x():B2.x()) <= ((A1.x()>A2.x())?A1.x():A2.x())
        && ((A1.x()<A2.x())?A1.x():A2.x()) <= ((B1.x()>B2.x())?B1.x():B2.x()))

        if (((B1.y()<B2.y())?B1.y():B2.y()) <= ((A1.y()>A2.y())?A1.y():A2.y())
                && ((A1.y()<A2.y())?A1.y():A2.y()) <=
                ((B1.y()>B2.y())?B1.y():B2.y()))

            return true;

    return false;
}


QPointF MainWindow::formulaIter(QPointF dot, bool direction)
{
    QPointF output;

    if (direction)
    {
        output.setX(dot.x() + dot.y() +
            0.4*dot.x()*(1 - dot.x()*dot.x()));
        output.setY(dot.y() +
            0.4*dot.x()*(1 - dot.x()*dot.x()));
    }

    else // Inverse mapping
    {
        output.setX(dot.x() - dot.y());
        output.setY(dot.y() -
            0.4*(dot.x() - dot.y())*
            (1 - (dot.x() - dot.y())*
             (dot.x() - dot.y())));
    }

    return output;
}


void MainWindow::setRunMode(bool value)
{
    if (value)
    {
        mode = DOT_ITERATIONS;
        labelInitDot->setHidden(false);
        editInitDot->setHidden(false);
        optionIntersection->setHidden(true);
        labelInitSegmentBegin->setHidden(true);
        editInitSegmentBegin->setHidden(true);
        labelInitSegmentEnd->setHidden(true);
        editInitSegmentEnd->setHidden(true);
    }

    else
    {
        mode = SEGMENT_ITERATIONS;
        labelInitDot->setHidden(true);
        editInitDot->setHidden(true);
        optionIntersection->setHidden(false);
        labelInitSegmentBegin->setHidden(false);
        editInitSegmentBegin->setHidden(false);
        labelInitSegmentEnd->setHidden(false);
        editInitSegmentEnd->setHidden(false);
    }
}


void MainWindow::setCheckIntersections(bool value)
{
    if (value)
        check_intersection = 1;

    else
        check_intersection = 0;
}


void MainWindow::refreshData()
{
    if (retrieveIterationNumber(editIter->text())
        && retrieveDomains(editX->text(), 'X')
        && retrieveDomains(editY->text(), 'Y')
        && retrieveDomains(editInitDot->text(), '0')
        && retrieveDomains(editInitSegmentBegin->text(), 'B')
        && retrieveDomains(editInitSegmentEnd->text(), 'E'))

        runGraphics();

    else
    {
        QDialog *message = new QDialog();
        message->setFixedSize(200, 120);
        message->setPalette(Qt::gray);
        message->setWindowTitle("Error");
        message->show();
    }
}


void MainWindow::runGraphics()
{
    QChart *chart = new QChart();

    QLinearGradient background;
    background.setStart(QPointF(0, 0));
    background.setFinalStop(QPointF(0, 1));
    background.setColorAt(0.0,
        QRgb(COLOR_BACKGROUND_1));
    background.setColorAt(1.0,
        QRgb(COLOR_BACKGROUND_2));
    background.setCoordinateMode(QGradient::ObjectBoundingMode);
    chart->setBackgroundBrush(background);

    QCategoryAxis *axisX = new QCategoryAxis();
    QCategoryAxis *axisY = new QCategoryAxis();
    QFont axis_font;
    axis_font.setPixelSize(12);
    axis_font.setBold(true);
    axisX->setLabelsFont(axis_font);
    axisX->setLabelsColor(QColor(250, 240, 230));
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setTitleText("X axis");
    axisX->setLabelsPosition
            (QCategoryAxis::AxisLabelsPositionOnValue);
    axisX->setGridLineVisible(true);
    axisY->setLabelsFont(axis_font);
    axisY->setLabelsColor(QColor(250, 240, 230));
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setTitleText("Y axis");
    axisY->setLabelsPosition
            (QCategoryAxis::AxisLabelsPositionOnValue);
    axisY->setGridLineVisible(true);

    fillAxis(lowerX, higherX, axisX);
    fillAxis(lowerY, higherY, axisY);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->legend()->hide();
    chart->setMinimumSize(400.0, 400.0);

    switch (mode)
    {
        case DOT_ITERATIONS:
        {
            QScatterSeries *seriesScatter = new QScatterSeries();
            fillSeries(seriesScatter);
            chart->addSeries(seriesScatter);
            seriesScatter->setMarkerSize(4.0);
            seriesScatter->setColor(QRgb(0x60838338));

            seriesScatter->attachAxis(axisX);
            seriesScatter->attachAxis(axisY);
            break;
        }

        case SEGMENT_ITERATIONS:
        {
            QLineSeries *seriesStable = new QLineSeries();
            QLineSeries *seriesUnstable = new QLineSeries();
            QLineSeries *seriesSegment = new QLineSeries();

            *seriesSegment << QPointF(segmentBeginX, segmentBeginY)
                    << QPointF(segmentEndX, segmentEndY);
            seriesSegment->setPen(QPen(QBrush(QColor(200, 200, 200)), 2.5));

            seriesStable->setPen(QPen(QBrush(QColor(10, 10, 250)), 2.5));
            seriesUnstable->setPen(QPen(QBrush(QColor(250, 10, 10)), 2.5));
            fillSeries(seriesStable);
            fillSeries(seriesUnstable, false);

            chart->addSeries(seriesSegment);
            chart->addSeries(seriesStable);
            chart->addSeries(seriesUnstable);

            if (check_intersection)
            {
                QList <QPointF> listStable(seriesStable->points());
                QList <QPointF> listUnstable(seriesUnstable->points());
                int result = 0;

                for (int i = 0; i < listStable.length()-1; i++)
                    for (int j = 0; j < listStable.length()-1; j++)
                        if (segmentIntersection(listStable[i], listStable[i+1],
                            listUnstable[j], listUnstable[j+1]))

                            result++;

                QDialog *message = new QDialog();
                QHBoxLayout *lay = new QHBoxLayout();
                QLabel *text = new QLabel("Number of intersections: ");
                QLabel *res = new QLabel(QString::number(result));

                lay->addWidget(text);
                lay->addWidget(res);
                message->setLayout(lay);
                message->setFixedSize(200, 120);
                message->setPalette(Qt::gray);
                message->setWindowTitle("Result");
                message->show();
            }

            seriesStable->attachAxis(axisX);
            seriesStable->attachAxis(axisY);
            seriesUnstable->attachAxis(axisX);
            seriesUnstable->attachAxis(axisY);
            seriesSegment->attachAxis(axisX);
            seriesSegment->attachAxis(axisY);
            break;
        }

        default:
            break;
            // Incorrect usage!

    }

    QChartView *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->show();
}
