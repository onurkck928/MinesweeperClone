#ifndef WIDGET_H
#define WIDGET_H

#include <cell.h>
#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <cellbutton.h>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>

/*
 * This class is responsible for setting the UI elements such as buttons, labels, layouts
 * as well as managing game logic like restarting, giving hint, distributing mines
 */
class Widget : public QWidget
{
    Q_OBJECT



private:


    Cell* m_cellArray[ROW_NUMBER][COLUMN_NUMBER];   // Allows easy access for each cell by storing them using row and column index.

    // ************** UI elements ***************
    QGridLayout* mainLayout;                        // Constructs the skeleton of the widget. Contains every other UI element                                                    // Contains all cells which holds buttons and labels for
    QGridLayout* m_cellGrid;                        // Contains every button and label corresponding to the cells.
    QPushButton* m_hintButton;                      // When clicked, gives the player a hint based on known information so far.
    QPushButton* m_restartButton;                   // When clicked, restarts the game with a different distribution of the mines.
    QLabel* m_scoreLabel;                           // Displays the current score of the player.

    void destroyPreviousElements();                 // Destroys the previous UI elements after restart button is clicked
public:

    Widget(QWidget *parent = nullptr);
    ~Widget();


    int setCellSize(int columnNum, int rowNum);     // Sets the size of each cell based on total number of cells
    void initializeCells();                         // Instantiates a predetermined amount of cells with their corresponding buttons
    void generateMines(int mineAmount);             // Distributes mines randomly on the cells and the quantity is given by mineAmount
    void setMineNumbers();                          // Sets the number to be displayed on each cell, once a cell is revealed.
    void revealAllMines();                          // Makes every cell with a mine reveal
    void revealAllCells();                          // Makes every cell reveal, displaying its number
    void setInitialState();                         // Sets the beginning state by calling functions related to game logic and UI elements.

public slots:

    // Slots related to a button press
    void restart();                                 // Defines the actions to be taken when m_restartButton is clicked
    void giveHint();                                // Defines the actions to be taken when m_hintButton is clicked

    // Slots related to the game logic
    void setLoseScreen();                           // Called when lose condition(Player reveals all a mine cells) is triggered
    void setWinScreen();                            // Called when win condition(Player reveals all non-mine cells) is triggered

};
#endif // WIDGET_H
