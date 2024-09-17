#include "cellbutton.h"

CellButton::CellButton(QWidget *parent)
    : QPushButton(parent)
{};

// Overridden function for mousePressEvent, Emits different signals that shows,
// whether the button is left-clicked or right-clicked
void CellButton::mousePressEvent(QMouseEvent *event)  {
    if (event->button() == Qt::LeftButton) {
        emit onLeftClick();
    }
    else if (event->button() == Qt::RightButton) {
        emit onRightClick();
    }
}


