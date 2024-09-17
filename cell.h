#ifndef CELL_H
#define CELL_H

#define COLUMN_NUMBER 15
#define ROW_NUMBER 15
#define MINE_AMOUNT 20

#include <QObject>
#include <QPushButton>
#include <QMessageBox>
#include <cellbutton.h>
#include <QLabel>


/*
 * This class is used to represent the cells in the game
 * Cell instances acts as a contaier for the UI elements (button and label)
 * Cell class also provides some functions to define actions for the neighbour interactions
 */

class Cell : public QObject
{
    Q_OBJECT



public:
    explicit Cell(int rowPosition, int columnPosition, QWidget* widget, QObject *parent = nullptr);


public:

    // These boolean values act like a type and status qualifier for the cells
    bool m_isMine = false;                 // Shows that the cell contains a mine
    bool m_isRevealed = false;             // Shows that the button of the cell is clicked
    bool m_isFlagged = false;              // Shows that the cell is flagged by the user
    bool m_isHinted = false;               // Shows that the cell is suggested as a hint to the player

    int m_numberOfNeighbouringMines = 0;   // Number of neighbour cells with mine that is adjacent to the mine)
    static int s_revealedCellAmount;       // Counts the number of cells revealed so far to display as the score
    static int s_mineAmount;               // The total number of cells with mine
    static int s_totalCellAmount;          // The total number of cells which is equal to COLUMN_NUMBER * ROW_NUMBER
    static QLabel* s_scoreLabel;           // The reference to the scoreLabel that is used to display player score
private:
    int m_rowPosition;                     // The row that cell belongs to
    int m_columnPosition;                  // The column that cell belongs to
    int m_neighbourAmount = 0;             // Each cell can have at least 3 and at most 8 neighbours based on its location
    Cell* m_neighbourCells[8];             // An array of neighbour cells which is used to traverse neighbours
    QWidget* m_widget;

public:
    bool m_isMarkedSafe = false;           // Used in Widget::giveHint() method, denotes the cell is certain to not contain a mine
    bool m_isMarkedUnsafe = false;         // Used in Widget::giveHint() method, denotes the cell is certain to contain a mine
public:
    CellButton* m_cellButton;              // Active when cell is unrevealed, when left clicked reveals the cell
    QLabel* m_cellLabel;                   // Active when cell is revealed, displays the the cell content either a number or a mine


public:

    void setNumberOfNeighbouringMines();    // Calculates the number of neighbour cells with mines adjacent to the cell
    void addNeighbour(Cell* neighbourCell); // Adds a neighbour for the cell
    void revealNeighbours();                // Used to recursively reveal the neighbours of the cell, this is called when an empty cell is revealed

public:
    // Used for hint algorithm
    int getUnrevealedNeighbourAmount();     // Calculates and returns the number of neighbours that are unrevealed
    int getSafeNeighbourAmount();           // Calculates and returns the number of neighbours that are safe
    int getUnsafeNeighbourAmount();         // Calculates and returns the number of neighbours that are unsafe
    bool markNeighboursAsUnsafe();          // Used in Widget::giveHint(), returns true if it marks any neighbour
    bool markUnmarkedNeighboursAsSafe();    // Used in Widget::giveHint(), returns true if it marks any neighbour

    int getLabelSize(int columnNumber, int rowNumber);     // Determines the size of the label depending on the number of cells
public slots:
    void revealCell();                      // Triggered when a cell is left clicked, reveals the cell
    void flagCell();                        // Triggered when a cell is right clicked, flags the cell

signals:
    void wonGame();                         // Emitted when all non-mine cells are revealed
    void lostGame();                        // Emitted when a mine is revealed by a player
};

#endif // CELL_H
