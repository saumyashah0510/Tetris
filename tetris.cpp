#include <iostream>
#include <conio.h>   // For _kbhit() and _getch() (Windows real-time input)
#include <vector>
#include <cstdlib>   // For system("cls") to clear screen
#include <ctime>     // For clock() timing

using namespace std;

const int width = 10;
const int height = 20;
const int fallSpeed = 500; // Slower falling speed (500ms)

string tetromino[7] = {
    "..X...X...X...X.", // I
    "..X..XX...X.....", // T
    "..X..XX..X......", // S
    "...X..XX..X.....", // Z
    "..X..X...XX.....", // L
    "...X...X..XX....", // J
    ".XX..XX........."  // O
};

char board[height][width]; // Game board
int score = 0;

// Current piece state
int currentPiece, rotation, posX, posY;
clock_t lastFallTime;

// Initialize board
void initBoard() {
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            board[i][j] = '.';
}

// Rotate piece
int rotate(int x, int y, int r) {
    switch (r % 4) {
        case 0: return y * 4 + x;
        case 1: return 12 + y - (x * 4);
        case 2: return 15 - (y * 4) - x;
        case 3: return 3 - y + (x * 4);
    }
    return 0;
}

// Check if piece fits
bool doesPieceFit(int piece, int rotation, int px, int py) {
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++) {
            int i = rotate(x, y, rotation);
            int boardX = px + x, boardY = py + y;

            if (tetromino[piece][i] == 'X') {
                if (boardX < 0 || boardX >= width || boardY >= height) return false;
                if (boardY >= 0 && board[boardY][boardX] != '.') return false;
            }
        }
    return true;
}

// Lock piece into board
void lockPiece() {
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++) {
            int i = rotate(x, y, rotation);
            if (tetromino[currentPiece][i] == 'X')
                board[posY + y][posX + x] = 'X';
        }
}

// Check for full rows and clear them
void clearRows() {
    for (int row = height - 1; row >= 0; row--) {
        bool fullRow = true;
        for (int col = 0; col < width; col++) {
            if (board[row][col] == '.') {
                fullRow = false;
                break;
            }
        }

        if (fullRow) {
            score += 10;
            for (int i = row; i > 0; i--) // Shift everything down
                for (int j = 0; j < width; j++)
                    board[i][j] = board[i - 1][j];

            for (int j = 0; j < width; j++) // Clear top row
                board[0][j] = '.';

            row++; // Recheck same row (since everything moved down)
        }
    }
}

// Draw board in terminal (includes falling piece)
void drawBoard() {
    system("cls"); // Use "clear" on Linux/macOS
    cout << "Score: " << score << "\n\n";
    
    // Copy board for rendering
    char tempBoard[height][width];
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            tempBoard[i][j] = board[i][j];

    // Draw current falling piece
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++) {
            int i = rotate(x, y, rotation);
            if (tetromino[currentPiece][i] == 'X') {
                int boardX = posX + x, boardY = posY + y;
                if (boardY >= 0)
                    tempBoard[boardY][boardX] = 'X';
            }
        }

    // Print the board
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j)
            cout << tempBoard[i][j] << ' ';
        cout << '\n';
    }
    cout << "Controls: Left(←) | Right(→) | Rotate(↑) | Drop(↓)" << endl;
}

// Handle user input
void handleInput() {
    if (_kbhit()) {
        char key = _getch();
        if (key == 75 && doesPieceFit(currentPiece, rotation, posX - 1, posY)) posX--; // Left (←)
        if (key == 77 && doesPieceFit(currentPiece, rotation, posX + 1, posY)) posX++; // Right (→)
        if (key == 72 && doesPieceFit(currentPiece, rotation + 1, posX, posY)) rotation++; // Rotate (↑)
        if (key == 80 && doesPieceFit(currentPiece, rotation, posX, posY + 1)) posY++; // Drop (↓)
    }
}

// Spawn a new piece
bool spawnPiece() {
    currentPiece = rand() % 7;
    rotation = 0;
    posX = width / 2 - 1;
    posY = 0;
    
    lastFallTime = clock(); // Reset fall timer

    // If the piece cannot be placed, game over
    return doesPieceFit(currentPiece, rotation, posX, posY);
}

// Game loop
void gameLoop() {
    initBoard();
    if (!spawnPiece()) {
        cout << "Game Over!\n";
        return;
    }

    while (true) {
        drawBoard();
        handleInput();

        // **Make the piece fall only if enough time has passed**
        if ((clock() - lastFallTime) * 1000 / CLOCKS_PER_SEC > fallSpeed) {
            if (doesPieceFit(currentPiece, rotation, posX, posY + 1)) {
                posY++;  // Move piece down
            } else {
                lockPiece();  // Place piece
                clearRows();  // Clear completed rows
                if (!spawnPiece()) {
                    drawBoard();
                    cout << "Game Over!\n";
                    break;  // End game
                }
            }
            lastFallTime = clock(); // Reset fall timer
        }
    }
}

int main() {
    gameLoop();
    return 0;
}
