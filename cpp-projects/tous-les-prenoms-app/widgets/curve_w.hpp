

#pragma once


// Qt
#include <QMouseEvent>

// QWt
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_curve_fitter.h>
#include <qwt_scale_engine.h>

class CurveW : public QwtPlot{

public:

    std::vector<double> xCoords = {0,1};
    std::vector<double> yCoords = {0,1};

    CurveW();

private:

    double minScaleX = 0.;
    double maxScaleX = 1.;

    double minScaleY = 0.;
    double maxScaleY = 1.;

public slots:

    void reset();

    void set_points(std::vector<double> x, std::vector<double> y);

    void set_first_y(double value);
    void set_last_y(double value);

    void set_pen(QPen pen);

    void set_fitted_state(bool state);
    void set_title(QString title);
    void set_x_title(QString title);
    void set_y_title(QString title);
    void set_x_range(qreal min, qreal max);
    void set_y_range(qreal min, qreal max);

    void remove_symbol();

private:

    QwtPlotCurve curve;
    bool sort = true;
    bool fitted = true;
};



