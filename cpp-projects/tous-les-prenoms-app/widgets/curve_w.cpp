
#include "curve_w.hpp"

// Qwt
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_curve_fitter.h>

CurveW::CurveW(){

    setCanvasBackground( Qt::gray );

    QFont axisFont("Calibri", 8);
    setAxisFont(QwtPlot::xBottom,   axisFont);
    setAxisFont(QwtPlot::yLeft,     axisFont);

    curve.setTitle("");
    curve.setPen( Qt::black, 4 );
    curve.setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve.setCurveAttribute(QwtPlotCurve::Fitted, fitted);
    curve.setSymbol( new QwtSymbol(
        QwtSymbol::Ellipse,
        QBrush(Qt::yellow),
        QPen(Qt::red, 2),
        QSize(4, 4))
    );

    setAxisScale( QwtPlot::xBottom, minScaleX, maxScaleX);
    setAxisScale( QwtPlot::yLeft,   minScaleY, maxScaleY);

    QwtSplineCurveFitter *fitter = new QwtSplineCurveFitter();
    fitter->setFitMode(QwtSplineCurveFitter::FitMode::Spline);
    curve.setCurveFitter(fitter);

    curve.setSamples(xCoords.data(), yCoords.data(), static_cast<int>(xCoords.size()));
    curve.attach(this);

    QwtPlot::replot();
    QwtPlot::repaint();
}


void CurveW::set_title(QString title){

    QwtText t(title);
    QFont font("Calibri", 10);
    font.setBold(true);
    t.setFont(font);
    setTitle(t);
}

void CurveW::set_x_title(QString title){

    QwtText t(title);
    QFont font("Calibri", 9);
    font.setBold(false);
    t.setFont(font);

    setAxisTitle(QwtPlot::xBottom, t);
}

void CurveW::set_y_title(QString title){

    QwtText t(title);
    QFont font("Calibri", 9);
    font.setBold(false);
    t.setFont(font);

    setAxisTitle(QwtPlot::yLeft, t);
}

void CurveW::set_x_range(qreal min, qreal max){

    minScaleX = min;
    maxScaleX = max;

    // erase values < minScaleX
    int count = 0;
    for(const auto &x : xCoords){
        if(x < minScaleX){
            ++count;
        }
    }

    if(count > 0){
        xCoords.erase(std::begin(xCoords), std::begin(xCoords) + count);
        yCoords.erase(std::begin(yCoords), std::begin(yCoords) + count);
    }

    // erase values > maxScaleX
    int start = -1;
    for(size_t ii = 0; ii < xCoords.size(); ++ii){
        if(xCoords[ii] > maxScaleX){
            start = static_cast<int>(ii);
            break;
        }
    }

    if(start > -1){
        xCoords.erase(std::begin(xCoords) + start, std::end(xCoords));
        yCoords.erase(std::begin(yCoords) + start, std::end(yCoords));
    }

    // set first and last x values
    if(xCoords.size() >= 2){
        xCoords[0] = minScaleX;
        xCoords[xCoords.size()-1] = maxScaleX;
    }else if(xCoords.size() == 1){

        if(std::abs(minScaleX-xCoords[0]) < std::abs(maxScaleX-xCoords[0])){
            yCoords = {yCoords[0], maxScaleY};
        }else{
            yCoords = {minScaleY, yCoords[0]};
        }
        xCoords = {minScaleX, maxScaleX};

    }else{
        xCoords = {minScaleX, maxScaleX};
        yCoords = {minScaleY, maxScaleY};
    }

    setAxisScale( QwtPlot::xBottom, minScaleX, maxScaleX);
    if(xCoords.size() > 1){
        curve.setSamples(xCoords.data(), yCoords.data(), static_cast<int>(xCoords.size()));
    }

    QwtPlot::replot();
    QwtPlot::repaint();
}

void CurveW::set_y_range(qreal min, qreal max){

    if(min > max){
        min = max - 0.01;
    }

    minScaleY = min;
    maxScaleY = max;

    for(auto &y: yCoords){
        if(y < minScaleY){
            y = minScaleY;
        }
        if(y > maxScaleY){
            y = maxScaleY;
        }
    }

    setAxisScale( QwtPlot::yLeft, minScaleY, maxScaleY);
    if(xCoords.size() > 1){
        curve.setSamples(xCoords.data(), yCoords.data(), static_cast<int>(xCoords.size()));
    }

    QwtPlot::replot();
    QwtPlot::repaint();
}

void CurveW::reset(){

    xCoords = {minScaleX, maxScaleX};
    yCoords = {minScaleY, maxScaleY};
    curve.setSamples(xCoords.data(), yCoords.data(), static_cast<int>(xCoords.size()));

    QwtPlot::replot();
    QwtPlot::repaint();
}

void CurveW::set_points(std::vector<double> x, std::vector<double> y){

    xCoords = std::move(x);
    yCoords = std::move(y);
    curve.setSamples(xCoords.data(), yCoords.data(), static_cast<int>(xCoords.size()));

    QwtPlot::replot();
    QwtPlot::repaint();
}

void CurveW::set_first_y(double value){

    yCoords[0] = value;
    curve.setSamples(xCoords.data(), yCoords.data(), static_cast<int>(xCoords.size()));

    QwtPlot::replot();
    QwtPlot::repaint();
}

void CurveW::set_last_y(double value){

    yCoords[yCoords.size()-1] = value;
    curve.setSamples(xCoords.data(), yCoords.data(), static_cast<int>(xCoords.size()));

    QwtPlot::replot();
    QwtPlot::repaint();
}

void CurveW::set_pen(QPen pen){
    curve.setPen(pen);
}

void CurveW::set_fitted_state(bool state){

    fitted = state;
    curve.setCurveAttribute(QwtPlotCurve::Fitted, state);

    QwtPlot::replot();
    QwtPlot::repaint();
}

void CurveW::remove_symbol(){

    curve.setSymbol(nullptr);

    QwtPlot::replot();
    QwtPlot::repaint();
}
