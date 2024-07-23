#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QObject>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QCategoryAxis>
#include <cmath>

#define  MAX_PRECISION  350

QT_CHARTS_USE_NAMESPACE


enum AppColors
{
    COLOR_BACKGROUND_1 = 0x00303030,
    COLOR_BACKGROUND_2 = 0x00000000,
};


enum RunMode
{
    DOT_ITERATIONS,
    SEGMENT_ITERATIONS,
};


class MainWindow:
    public QMainWindow
{
private:
    Q_OBJECT
    double dataX[MAX_PRECISION];
    double dataY[MAX_PRECISION];
    double initDotX;
    double initDotY;
    double segmentBeginX;
    double segmentBeginY;
    double segmentEndX;
    double segmentEndY;
    double lowerX;
    double higherX;
    double lowerY;
    double higherY;
    long int iteration_number;
    QLabel *labelInitDot;
    QLabel *labelInitSegmentBegin;
    QLabel *labelInitSegmentEnd;
    QLabel *labelX;
    QLabel *labelY;
    QLabel *labelIter;
    QLineEdit *editInitDot;
    QLineEdit *editInitSegmentBegin;
    QLineEdit *editInitSegmentEnd;
    QLineEdit *editX;
    QLineEdit *editY;
    QLineEdit *editIter;
    QPushButton *button_go;
    QRadioButton *radio_dots;
    QRadioButton *radio_segments;
    QCheckBox *optionIntersection;
    QVBoxLayout *layoutSettings;
    QHBoxLayout *lineInitDot;
    QHBoxLayout *lineInitSegmentBegin;
    QHBoxLayout *lineInitSegmentEnd;
    QHBoxLayout *lineIterations;
    QHBoxLayout *lineDomainX;
    QHBoxLayout *lineDomainY;
    QWidget *widgetCentral;

    double distance(QPointF dot1, QPointF dot2);
    double distance(double dot1X, double dot1Y,
        double dot2X, double dot2Y);
    double givenSegmentOrdinate(double X);
    bool segmentIntersection(QPointF A1, QPointF A2,
        QPointF B1, QPointF B2);

    bool retrieveIterationNumber(QString &src);
    bool retrieveDomains(QString &src, char target);
    QPointF formulaIter(QPointF dot, bool direction = true);
    void fillAxis(double lo, double hi, QCategoryAxis* axis);
    void fillSeries(QScatterSeries *series);
    void fillSeries(QLineSeries *series, bool direction = true);

public:
    RunMode mode;
    bool check_intersection;

    MainWindow();

private slots:
    void setRunMode(bool value);
    void setCheckIntersections(bool value);
    void refreshData();
    void runGraphics();
};

#endif // MAINWINDOW_H
