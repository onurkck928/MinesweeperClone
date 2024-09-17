#include "cell.h"

/*
 * This file provides implementations for the member functions of Cell class declared in cell.h
 */

// Static member definitions
int Cell:: s_totalCellAmount = ROW_NUMBER * COLUMN_NUMBER;      // Calculate the total number of cells
int Cell::s_mineAmount = MINE_AMOUNT;                           // Stores the total number of mines
int Cell::s_revealedCellAmount = 0;                             // Stores the number of revealed cells to display as score
QLabel* Cell::s_scoreLabel = nullptr;                           // Stores a reference to the scoreLabel to modify it inside Cell class

// Initializes a cell object in the given row, column indices
Cell::Cell(int rowPosition, int columnPosition, QWidget* widget, QObject *parent)
    : QObject{parent}
    , m_rowPosition{rowPosition}
    , m_columnPosition{columnPosition}
    , m_widget{widget}

{
}

// Assigns a number to the label depending on the number of neighbouring cells with mine
void Cell::setNumberOfNeighbouringMines()
{
    // Set the size of label depending on the number of cells
    int labelSize = getLabelSize(COLUMN_NUMBER, ROW_NUMBER);
    QSize pixmapSize(labelSize, labelSize);

    // Don't assign a number if that cell has a mine.
    if (m_isMine) {
        m_cellLabel->setPixmap(QPixmap(":/image/mine.png").scaled(pixmapSize));
        return;
    }

    // Calculate the number of cells with mine by iterating over each neighbour
    int neighbouringMines = 0;
    for (int i = 0; i < m_neighbourAmount; i++) {
        if(m_neighbourCells[i]->m_isMine) {
            neighbouringMines++;
        }
    }

    // Assign a number to the label based on the previously calculated neighbouringMines value
    switch(neighbouringMines) {
    case 0:
        m_cellLabel->setPixmap(QPixmap(":/image/0.png").scaled(pixmapSize));
        break;
    case 1:
        m_cellLabel->setPixmap(QPixmap(":/image/1.png").scaled(pixmapSize));
        break;
    case 2:
        m_cellLabel->setPixmap(QPixmap(":/image/2.png").scaled(pixmapSize));
        break;
    case 3:
        m_cellLabel->setPixmap(QPixmap(":/image/3.png").scaled(pixmapSize));
        break;
    case 4:
        m_cellLabel->setPixmap(QPixmap(":/image/4.png").scaled(pixmapSize));
        break;
    case 5:
        m_cellLabel->setPixmap(QPixmap(":/image/5.png").scaled(pixmapSize));
        break;
    case 6:
        m_cellLabel->setPixmap(QPixmap(":/image/6.png").scaled(pixmapSize));
        break;
    case 7:
        m_cellLabel->setPixmap(QPixmap(":/image/7.png").scaled(pixmapSize));
        break;
    case 8:
        m_cellLabel->setPixmap(QPixmap(":/image/8.png").scaled(pixmapSize));
        break;

    }

    // Store the number of neighbour cells with mine as a member variable of the cell
    m_numberOfNeighbouringMines = neighbouringMines;


}

// Add a neighbour for the cell
void Cell::addNeighbour(Cell *neighbourCell)
{
    this->m_neighbourCells[m_neighbourAmount++] = neighbourCell;

}

// Recursively reveals the cells and their neighbours together with revealCell()
// This function is called whenever a cell with empty value is revealed by revealCell()
void Cell::revealNeighbours()
{
    // Iterate over each neighbour
    for (int i = 0; i < m_neighbourAmount; i++) {
        Cell * neighbour = m_neighbourCells[i];
        // Skip already revealed cells
        if(neighbour->m_isRevealed)
            continue;

        // If the neighbour cell is a mine, don't reveal
        if(neighbour->m_isMine)
            continue;

        // Reveal the neighbour cell
        neighbour->revealCell();
    }
}

/*
 * Reveals the cell by making its button invisible and setting its label visible
 * If the revealed cell is an empty cell, all of its neighbours are also revealed
 */
void Cell::revealCell()
{
    // If the cell is already revealed, skip it
    if(m_isRevealed)
        return;


    // Reveal the cell by setting its member fields to necessary values
    m_isRevealed = true;
    m_cellButton->setVisible(false);
    m_cellLabel->setVisible(true);

    // Only update it if it's not a mine
    if (!m_isMine)
        s_revealedCellAmount++;

    // Update score
    s_scoreLabel->setText("Score: " + QString::number(s_revealedCellAmount));

    // If player reveals a mine, emitted lostGame signal makes the game end with loseCondition
    if(m_isMine) {
        emit lostGame();

    }

    // If the player revealed all non-mine cells, wonGame signal makes the game end with winCondition
    else if(s_revealedCellAmount + s_mineAmount == s_totalCellAmount){
        emit wonGame();
    }

    // Recursive call for the neighbours for empty cells
    if(m_numberOfNeighbouringMines == 0 && !m_isMine)
        revealNeighbours();
}


// Iterate over each neighbour, return the number of unrevealed neighbours
// Used in hint algorithm
int Cell::getUnrevealedNeighbourAmount()
{
    int unrevealedNeighbourAmount = 0;
    for (int i = 0; i < m_neighbourAmount; i++) {
        if(!m_neighbourCells[i]->m_isRevealed)
            unrevealedNeighbourAmount++;
    }

    return unrevealedNeighbourAmount;
}

// Iterate over each neighbour, return the number of safe neighbours
// Used in hint algorithm
int Cell::getSafeNeighbourAmount()
{
    int safeNeighbourAmount = 0;
    for (int i = 0; i < m_neighbourAmount; i++) {
        if(m_neighbourCells[i]->m_isMarkedSafe && !m_neighbourCells[i]->m_isRevealed)
            safeNeighbourAmount++;
    }

    return safeNeighbourAmount;
}

// Iterate over each neighbour, return the number of unsafe neighbours
// Used in hint algorithm
int Cell::getUnsafeNeighbourAmount()
{
    int unsafeNeighbourAmount = 0;
    for (int i = 0; i < m_neighbourAmount; i++) {
        if(m_neighbourCells[i]->m_isMarkedUnsafe && !m_neighbourCells[i]->m_isRevealed)
            unsafeNeighbourAmount++;
    }

    return unsafeNeighbourAmount;
}

// Iterate over each neighbour, mark the unsafe neighbours
// Used in hint algorithm.
// Returns true if any cell is marked, false otherwise
bool Cell::markNeighboursAsUnsafe()
{
    bool changeOccured = false;

    for (int i = 0; i < m_neighbourAmount; i++) {
        if(!m_neighbourCells[i]->m_isMarkedSafe && !m_neighbourCells[i]->m_isRevealed && !m_neighbourCells[i]->m_isMarkedUnsafe) {
            m_neighbourCells[i]->m_isMarkedUnsafe = true;
            changeOccured = true;
        }
    }

    return changeOccured;

}

// Iterate over each neighbour, mark the safe neighbours
// Used in hint algorithm.
// Returns true if any cell is marked, false otherwise
bool Cell::markUnmarkedNeighboursAsSafe()
{
    bool changeOccured = false;

    for (int i = 0; i < m_neighbourAmount; i++) {
        if(!m_neighbourCells[i]->m_isMarkedUnsafe && !m_neighbourCells[i]->m_isRevealed && !m_neighbourCells[i]->m_isMarkedSafe) {
            m_neighbourCells[i]->m_isMarkedSafe = true;
            changeOccured = true;
        }
    }

    return changeOccured;
}


// This function is called whenever a cell is right clicked
// Flags the cell
void Cell::flagCell() {

    // If the cell is not flagged, flag it.
    if(!m_isFlagged) {


        m_isFlagged = true;
        m_cellButton->setIcon(QIcon(":/image/flag.png"));

        // If the cell is suggested as a hint, flagging should make it unsuggested
        if(m_isHinted) {
            m_isHinted = false;
        }
    }
    // Unflag it if it's previously flagged
    else {
        m_isFlagged = false;
        m_cellButton->setIcon(QIcon(":/image/empty.png"));
    }
}

// Sets the label size depending on total number of cells

int Cell::getLabelSize(int columnNumber, int rowNumber) {

    // Use the higher value of columnNumber and rowNumber for determining size
    int longestSide = (columnNumber >= rowNumber ) ? columnNumber : rowNumber;

    // Set the size based on the interval of the longestSide
    int cellSize;
    if(longestSide <= 10) {
        cellSize = 60;
    }
    else if(longestSide <= 15) {
        cellSize =  45;
    }
    else {
        cellSize =  35;
    }

    return cellSize;

}
