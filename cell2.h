#ifndef CELL2_H
#define CELL2_H

#include <QObject>
#include <QPushButton>
#include <QLabel>

class Cell2 : public QObject
{
    Q_OBJECT
public:
    explicit Cell2(QObject *parent = nullptr);

public:
    bool m_isMine = false;
    int m_numberOfNeighbouringMines = 0;
private:

    int m_rowPosition;
    int m_columnPosition;
    bool m_isRevealed = false;
    int neighbourAmount = 0;
    Cell2 *m_neighbourCells[8];

public:
    QPushButton* m_cellButton;
    QLabel* m_cellLabel;


public:
    Cell2(int rowPosition, int columnPosition);

    void setNumberOfNeighbouringMines();
    void addNeighbour(Cell2* neighbourCell);

    void revealNeighbours();

public slots:
    void revealCell();
    void flagCell();
};

#endif // CELL2_H
