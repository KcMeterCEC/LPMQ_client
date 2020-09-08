#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_canvas.h>
#include "splot.h"
#include "splotgrid.h"
Splot::Splot(const QwtText &title, QWidget *parent):
    QwtPlot(title, parent)
{
    // canvas
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth(1);
    canvas->setFrameStyle(QFrame::Box | QFrame::Plain);
    canvas->setBorderRadius(5);

    QPalette canvasPalette(Qt::white);
    canvasPalette.setColor(QPalette::Foreground, QColor( 255, 255, 255 ));
    canvas->setPalette(canvasPalette);

    setCanvas(canvas);
    //body
    setAutoFillBackground(true);
    setPalette(QPalette(QColor(255, 255, 255)));

    mousePicker = new QwtPlotPicker(this->xBottom, this->yLeft,
                                    QwtPlotPicker::NoRubberBand,
                                    QwtPicker::AlwaysOff,
                                    this->canvas());
    mousePicker->setStateMachine(new QwtPickerTrackerMachine());

    connect(mousePicker, &QwtPlotPicker::moved, this, &Splot::mouseMoved);
    connect(mousePicker, &QwtPlotPicker::removed, this, &Splot::mouseRemoved);

    grid = new SplotGrid();
    grid->attach(this);
}
