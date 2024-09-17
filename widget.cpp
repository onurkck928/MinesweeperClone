#include "widget.h"
#include <random>

// This file provides implementations for the member functions of Widget class declared in widget.h


// When constructed, widget objects creates the window and layouts for the UI element
Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Minesweeper");                          // Set game title

    mainLayout = new QGridLayout(this);                     // mainLayout is container of all other UI elements


    Widget::setInitialState();                              // Starts the game by setting initial state of UI elements and game logic


}

Widget::~Widget() {}

int Widget::setCellSize(int columnNumber, int rowNumber) {

    int longestSide = (columnNumber >= rowNumber ) ? columnNumber : rowNumber;

    if(longestSide <= 10) {
        return 60;
    }
    else if(longestSide <= 15) {
        return 45;
    }
    else {
        return 35;
    }
}


/*  This function is responsible for setting game-logic in three steps
 *  First, creates the cells and stores them in the m_cellArray
 *  Second, sets neighbourhood relationship for each adjacent cell
 *  Finally assigns each cell either a mine or a number indicating amount of adjacent mines.
 */
void Widget::initializeCells()
{

    for(int i = 0; i < ROW_NUMBER; i++)
    {
        for (int j = 0; j < COLUMN_NUMBER; j++)
        {
            // Each entry in m_cellArray is a pointer to a Cell that holds.
            m_cellArray[i][j] = new Cell(i, j, this);


            // Create a label for each each cell to be used after the cell is revealed
            QLabel* cellLabel = new QLabel(this);
            cellLabel->setAlignment(Qt::AlignCenter);
            cellLabel->setVisible(false);                       // Labels are invisible until the button of the cell is clicked
            m_cellArray[i][j]->m_cellLabel = cellLabel;         // Each label is stored as a member variable of the corresponding cell
            m_cellGrid->addWidget(cellLabel,i + 1, j + 2);      // Place the invisible label on the layout

            // Size of each cell is determined based on number of rows and columns
            int cellSize = setCellSize(COLUMN_NUMBER, ROW_NUMBER);

            // Create a button for each cell. When clicked these buttons reveal the label of cells that were previously invisible
            CellButton* cellButton = new CellButton(this);
            cellButton->setIcon(QIcon(":/image/empty.png"));
            cellButton->setFixedSize(cellSize, cellSize);
            cellButton->setIconSize(QSize(cellSize, cellSize));
            m_cellArray[i][j]->m_cellButton = cellButton;         // Each button is stored as a member variable of the corresponding cell
            m_cellGrid->addWidget(cellButton,i + 1, j + 2);       // Display the button on the layout


            // Connect the functionalities of left and right button clicks with related slots
            // When a button is left clicked, it reveals its label
            QObject::connect(cellButton, &CellButton::onLeftClick, m_cellArray[i][j], &Cell::revealCell);
            // When a button is right clicked, it flags the cell without revealing the label
            QObject::connect(cellButton, &CellButton::onRightClick, m_cellArray[i][j], &Cell::flagCell);

            // Connect the win and lose conditions signals with functions that sets win and lose screens
            QObject::connect(m_cellArray[i][j], &Cell::lostGame, this, &Widget::setLoseScreen);
            QObject::connect(m_cellArray[i][j], &Cell::wonGame, this, &Widget::setWinScreen);



        }
    }

    // Set neighbours of cells by traversing over each cell.
    // Some cells have less neighbours as they are placed on the boundaries
    for(int i = 0; i < ROW_NUMBER; i++) {

        for(int j = 0; j < COLUMN_NUMBER; j++) {

            Cell* currentCell = m_cellArray[i][j];

            // If the current cell is not leftmost cell, it has left neighbours
            if(i > 0) {

                currentCell->addNeighbour(m_cellArray[i-1][j]);           // left neighbour

                // Current cell is not at the bottom so it has down neighbour
                if (j > 0) {
                    currentCell->addNeighbour(m_cellArray[i-1][j-1]);     //down_left neighbour

                }
                // Current cell is not at the top so it has up neighbour
                if (j < COLUMN_NUMBER - 1) {
                    currentCell->addNeighbour(m_cellArray[i-1][j+1]);     //up_left neighbour

                }

            }

            // Current cell is not rightmost, so it has right neighbours
            if(i < ROW_NUMBER - 1) {


                currentCell->addNeighbour(m_cellArray[i+1][j]);             // right neighbour

                // Current cell is not at the bottom so it has down neighbour
                if (j > 0) {

                    currentCell->addNeighbour(m_cellArray[i+1][j-1]);       //down_right neighbour

                }
                // Current cell is not at the top so it has up neighbour
                if (j < COLUMN_NUMBER - 1) {

                    currentCell->addNeighbour(m_cellArray[i+1][j+1]);       //up_right neighbour
                }

            }

            // Current cell is not at the bottom, so it has down neighbours
            if (j > 0) {

                currentCell->addNeighbour(m_cellArray[i][j-1]);             // down neighbour
            }

            // Current cell is not at the top, so it has up neighbours
            if (j < COLUMN_NUMBER - 1) {

                currentCell->addNeighbour(m_cellArray[i][j+1]);             // up neighbour
            }
        }
    }

    generateMines(MINE_AMOUNT);         // Assign mines to cells
    setMineNumbers();                   // Calculate the number associated with each non-mined cell



}


/*
 * Distribute the mines over cells randomly using a mersene-twister random number generation
 * mineAmount argument determines the total number of mines that should be distributed over cells
 */
void Widget::generateMines(int mineAmount)
{
    // Set the random number generator and the seed.
    std::mt19937 mt{std::random_device{}()};
    // Generated numbers should be limited to interval of (0, numberOfCells)
    std::uniform_int_distribution<> generateMineIndex{0, ROW_NUMBER * COLUMN_NUMBER - 1};


    int remainingMineAmount = mineAmount;

    // Randomly distribute mines until there is no mine left to place.
    while(remainingMineAmount > 0) {

        // Transform 1D number generated by random number generator to a 2D tuple of (rowIndex, columnIndex),
        // this  tuple is then used to locate the cell that mine should be placed to
        int mineIndex = generateMineIndex(mt);
        int rowIndex = mineIndex / COLUMN_NUMBER;
        int columnIndex = mineIndex % COLUMN_NUMBER;

        // If that cell already has a mine, get another random cell
        if(m_cellArray[rowIndex][columnIndex]->m_isMine) {
            continue;
        }

        // Place the mine to the cell
        m_cellArray[rowIndex][columnIndex]->m_isMine = true;

        remainingMineAmount--;
    }

}

/*
 * Reveals all unrevealed cells by iterating over each cell
 * This function is called when game ends with a win
 */
void Widget::revealAllCells() {

    // Iterate over each cell
    for(int i = 0; i < ROW_NUMBER; i++)
    {
        for (int j = 0; j < COLUMN_NUMBER; j++)
        {
            // get the cell at the current location
            Cell* currentCell = m_cellArray[i][j];

            // Deactivate the functionalities of the buttons of revealed cells
            QObject::disconnect(currentCell->m_cellButton, &CellButton::onLeftClick, m_cellArray[i][j], &Cell::revealCell);
            QObject::disconnect(currentCell->m_cellButton, &CellButton::onRightClick, m_cellArray[i][j], &Cell::flagCell);

            // Reveal unrevealed cells by making their button invisible and their label visible
            if(!currentCell->m_isRevealed) {

                currentCell->m_isRevealed = true;

                currentCell->m_cellButton->setVisible(false);
                currentCell-> m_cellLabel->setVisible(true);
            }
        }
    }
}


/*
 * Reveals all unrevealed mines by iterating over each cell
 * This function is called when game ends with a lose
 */
void Widget::revealAllMines() {

    // Iterate over each cell
    for(int i = 0; i < ROW_NUMBER; i++)
    {
        for (int j = 0; j < COLUMN_NUMBER; j++)
        {
            // get the cell at the current location
            Cell* currentCell = m_cellArray[i][j];

            // Deactivate the functionalities of the buttons of revealed cells
            QObject::disconnect(currentCell->m_cellButton, &CellButton::onLeftClick, m_cellArray[i][j], &Cell::revealCell);
            QObject::disconnect(currentCell->m_cellButton, &CellButton::onRightClick, m_cellArray[i][j], &Cell::flagCell);

            // Reveal unrevealed mines by making their button invisible and their label visible
            if(!currentCell->m_isRevealed && currentCell->m_isMine) {

                currentCell->m_isRevealed = true;
                currentCell->m_cellButton->setVisible(false);
                currentCell-> m_cellLabel->setVisible(true);
            }
        }
    }
}


/*
 * Assign a number for each cell label based on adjacent cells with mine,
 * this number is displayed after the cell is revealed
 */
void Widget::setMineNumbers()
{
    // Iterate over each cell
    for(int i = 0; i < ROW_NUMBER; i++) {
        for(int j = 0; j < COLUMN_NUMBER; j++) {
            // Assigning numbers is done via the call to a member function of Cell class
            m_cellArray[i][j]->setNumberOfNeighbouringMines();
        }
    }
}

/*
 * This function is a slot, triggered either on construction of widget or after restart button is clicked
 * Initializes the buttons, labels and cells displayed on the widget
 * Also calls the functions responsible for setting up game-logic such as distributing mines and assigning numbers to cells
 */
void Widget::setInitialState()
{

    // m_cellGrid is a subcontainer in mainLayout, contains buttons & labels of cells
    m_cellGrid = new QGridLayout(this);
    mainLayout->addLayout(m_cellGrid, 1, 0);


    // Each cell should be next to each other without additional spaces
    m_cellGrid->setSpacing(0);


    // Set the buttons and labels of each cell
    initializeCells();


    // Score label is used to display current score of the player.
    m_scoreLabel = new QLabel(this);
    m_scoreLabel->setText("Score : 0");
    mainLayout->addWidget(m_scoreLabel,0, 0, 1, 1);             // Score is displayed on the top left of the layout
    Cell::s_scoreLabel = m_scoreLabel;                          // Store the label in Cell class as a static variable


    // Restart button is used to allow player to start a new game with a different distribution of mines.
    m_restartButton = new QPushButton(this);
    m_restartButton->setText("Restart");
    mainLayout->addWidget(m_restartButton,0, 1, 1, 1);          // Restart button is displayed on the top right of layout
    // Connect the restart functionality with the button click
    QObject::connect(m_restartButton, &QPushButton::clicked, this, &Widget::restart);


    // Hint button is used to allow player to get a hint for a safe move
    m_hintButton = new QPushButton(this);
    m_hintButton->setText("Hint");
    mainLayout->addWidget(m_hintButton,0, 2, 1, 1);             // Hint button is displayed on the top right of layout
    // Connect the hint functionality with the button click
    QObject::connect(m_hintButton, &QPushButton::clicked, this, &Widget::giveHint);

}

/*  This function is triggered by the hint button,
 *  it gives a hint based on the information available to the player.
 *  A hint indicates a cell with no mine and suggested cell is displayed as a green cell
 *  Asking for a hint twice for a cell, reveals the cell
 */
void Widget::giveHint() {


    // Repeats the marking process until a pass with no change to cell marks is made.
    bool changeOccured = true;      // Determines whether a change is made or not
    while(changeOccured) {

        changeOccured = false;

        // Iterate over each cell, mark cells that are certain to contain a mine as unsafe
        for(int i = 0; i < ROW_NUMBER; i++) {
            for(int j = 0; j < COLUMN_NUMBER; j++) {

                Cell* currentCell = m_cellArray[i][j];

                // When a cell has unrevealed, unmarked neighbours as much as its neighbouring mines,
                // mark all unmarked neighbours as unsafe, since they certainly contain a mine
                if (currentCell->m_isRevealed && currentCell->m_numberOfNeighbouringMines != 0) {
                    if(currentCell->m_numberOfNeighbouringMines == (currentCell->getUnrevealedNeighbourAmount() - currentCell->getSafeNeighbourAmount()))
                    {
                        // If any cell is marked, a change is made so loop continues
                        bool change = currentCell->markNeighboursAsUnsafe();
                        if(change) {
                            changeOccured = true;
                        }

                    }
                }
            }
        }

        // Iterate over each cell, mark cells that are certain to not contain a mine as safe
        for(int i = 0; i < ROW_NUMBER; i++) {
            for(int j = 0; j < COLUMN_NUMBER; j++) {

                Cell* currentCell = m_cellArray[i][j];

                // When a cell has unmarked, unrevealed neighbours and all the cells containing mines are marked
                // mark all unmarked cells as safe since they certainly don't contain a mine
                if (currentCell->m_isRevealed && currentCell->m_numberOfNeighbouringMines != 0) {
                    if(currentCell->m_numberOfNeighbouringMines == currentCell->getUnsafeNeighbourAmount()) {

                        // If any cell is marked, a change is made so loop continues
                        bool change = currentCell->markUnmarkedNeighboursAsSafe();
                        if(change) {
                            changeOccured = true;
                        }
                    }
                }
            }
        }
    }

    // After the marking process is complete, iterate over each cell,
    // and give a safe marked unrevealed cell as hinted cell
    for(int i = 0; i < ROW_NUMBER; i++) {
        for(int j = 0; j < COLUMN_NUMBER; j++) {

            Cell* currentCell = m_cellArray[i][j];

            // determine whether the cell is eligible to be a hint suggestion or not
            if(currentCell->m_isMarkedSafe && !currentCell->m_isRevealed) {

                // If the cell is already hinted, reveal the cell
                if(currentCell->m_isHinted) {
                    currentCell->revealCell();
                }

                // Adjust member variables to make cell hinted.
                currentCell->m_isHinted = true;
                currentCell->m_cellButton->setIcon(QIcon(":/image/hint.png"));
                currentCell->m_isFlagged = false;       // A flagged cell, when hinted, should be unflagged
                return;
            }
        }
    }

}

/*
 * This function is triggered by restart button
 * Restart the game by resetting to initial state
 */
void Widget::restart() {
    destroyPreviousElements();
    setInitialState();
    Cell::s_revealedCellAmount = 0;
}

/*
 * Triggered by Cell::wonGame signal (When all non-mine cells are revealed)
 * Makes all cells revealed, unclickable and pops up a win message
 */
void Widget::setWinScreen()
{
    QObject::disconnect(m_hintButton, &QPushButton::clicked, this, &Widget::giveHint);
    revealAllCells();
    m_scoreLabel->setText("Score: " + QString::number(COLUMN_NUMBER * ROW_NUMBER));
    QMessageBox::information(this, "Winner!", "You Won!");

}


/*
 * Triggered by Cell::lostGame signal   (When a mine is revealed by player)
 * Makes all cells revealed, unclickable and pops up a lose message
 */

void Widget::setLoseScreen()
{
    QObject::disconnect(m_hintButton, &QPushButton::clicked, this, &Widget::giveHint);
    revealAllMines();
    QMessageBox::information(this, "Loser!", "You Lost!");

}

/*
 * Deletes previous UI elements
 */
void Widget::destroyPreviousElements() {

    // delete score and buttons
    delete(m_scoreLabel);
    delete(m_restartButton);
    delete(m_hintButton);

    // delete cell buttons and labels by iterating over each cell
    for (int i = 0; i < ROW_NUMBER; i++) {
        for(int j = 0; j < COLUMN_NUMBER; j++) {
            delete(m_cellArray[i][j]->m_cellButton);
            delete(m_cellArray[i][j]->m_cellLabel);
        }
    }
    delete(m_cellGrid);

}
