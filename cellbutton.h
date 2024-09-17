#ifndef CELLBUTTON_H
#define CELLBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QMouseEvent>

/*
 * This class implements a subclass of QPushButton
 * in order to override mousePresEvent for producing different actions based on left or right click
 */

class CellButton : public QPushButton
{
    Q_OBJECT
public:
    CellButton(QWidget *parent = nullptr);

protected:
    // Overrides the mousePressEvent() function to generate different actions depending on left or right click
    void mousePressEvent(QMouseEvent *event) override;

public:
signals:
    void onLeftClick();     // Emits when the button is left clicked
    void onRightClick();    // Emits when the button is right clicked
};

#endif // CELLBUTTON_H
