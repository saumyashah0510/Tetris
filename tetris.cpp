#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>   // For generating random pieces
#include <fstream> //For file handling

using namespace std;

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// Size of the grid
const int WIDTH = 10;
const int HEIGHT = 20;

void showWelcomeScreen();
void clearScreen();

class Tetromino
{
public:
    std::vector<std::vector<int>> shape;
    int x, y;
    int color; // Fixed color for this piece

    // constructor
    Tetromino(std::vector<std::vector<int>> s, int startX, int startY, int c)
        : shape(s), x(startX), y(startY), color(c) {}

    // rotates pieces 90 degrees clockwise
    void rotate()
    {
        std::vector<std::vector<int>> rotated(shape[0].size(), std::vector<int>(shape.size()));
        for (size_t i = 0; i < shape.size(); ++i)
        {
            for (size_t j = 0; j < shape[0].size(); ++j)
            {
                rotated[j][shape.size() - 1 - i] = shape[i][j];
            }
        }
        shape = rotated;
    }
};

class Game
{
private:
    std::vector<std::vector<int>> grid;
    Tetromino *currentPiece;
    Tetromino *nextPiece;
    int score;
    int speed;
    int highScore;

    void loadHighScore()
    {
        std::ifstream file("highscore.txt");
        if (!file.is_open())
        {
            std::ofstream newFile("highscore.txt");
            newFile << 0;
            newFile.close();
            highScore = 0;
        }
        else
        {
            file >> highScore;
            file.close();
        }
    }

    void saveHighScore()
    {
        std::ofstream file("highscore.txt");
        if (file.is_open())
        {
            file << highScore;
            file.close();
        }
    }

    Tetromino *getRandomPiece()
    {
        std::vector<std::vector<std::vector<int>>> pieces = {
            {{1, 1, 1, 1}},         // I
            {{1, 1}, {1, 1}},       // O
            {{0, 1, 0}, {1, 1, 1}}, // T
            {{0, 1, 1}, {1, 1, 0}}, // S
            {{1, 1, 0}, {0, 1, 1}}, // Z
            {{1, 0, 0}, {1, 1, 1}}, // J
            {{0, 0, 1}, {1, 1, 1}}  // L
        };

        int color[] = {9, 14, 13, 2, 4, 6, 5}; // Fixed color for each piece

        int index = rand() % pieces.size();
        return new Tetromino(pieces[index], WIDTH / 2 - pieces[index][0].size() / 2, -2, color[index]);
    }

    bool isValidMove(Tetromino *piece, int newX, int newY, bool isRotation = false)
    {
        // Check all blocks in the piece's shape
        for (size_t i = 0; i < piece->shape.size(); ++i)
        {
            for (size_t j = 0; j < piece->shape[0].size(); ++j)
            {
                if (piece->shape[i][j])
                {
                    int x = newX + j;
                    int y = newY + i;

                    // Boundary checks
                    if (x < 0 || x >= WIDTH || y >= HEIGHT)
                    {
                        return false;
                    }

                    // Collision with existing blocks
                    if (y >= 0 && grid[y][x])
                    {
                        return false;
                    }

                    // Additional rotation safety checks
                    if (isRotation)
                    {
                        // Check one block below each piece block
                        if (y + 1 < HEIGHT && grid[y + 1][x])
                        {
                            return false;
                        }
                        // Check if rotation would push piece into floor
                        if (y >= HEIGHT - piece->shape.size())
                        {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    void placePiece()
    {

        for (size_t i = 0; i < currentPiece->shape.size(); ++i)
        {
            for (size_t j = 0; j < currentPiece->shape[0].size(); ++j)
            {
                if (currentPiece->shape[i][j])
                {
                    int yPos = currentPiece->y + i;
                    int xPos = currentPiece->x + j;

                    // Boundary safety checks
                    if (yPos < 0)
                    {
                        // Piece is above the grid - allow placement but don't draw
                        continue;
                    }

                    if (xPos < 0 || xPos >= WIDTH || yPos >= HEIGHT)
                    {
                        // This should never happen if isValidMove() is working properly
                        showGameOver();
                        delete currentPiece;
                        currentPiece = nullptr;
                        return;
                    }
                }
            }
        }

        for (size_t i = 0; i < currentPiece->shape.size(); ++i)
        {
            for (size_t j = 0; j < currentPiece->shape[0].size(); ++j)
            {
                if (currentPiece->shape[i][j])
                {
                    int yPos = currentPiece->y + i;
                    int xPos = currentPiece->x + j;

                    // ✅ Ignore any blocks that are placed above the screen
                    if (yPos >= 0 && xPos >= 0 && xPos < grid[0].size())
                        grid[yPos][xPos] = currentPiece->color;
                }
            }
        }
        delete currentPiece;
        currentPiece = nullptr;

        // Immediately check if the placed piece caused a game over
        if (isGameOver())
        {
            showGameOver();
            resetGame();
        }
    }

    void clearLines()
    {
        for (int i = HEIGHT - 1; i >= 0; --i)
        {
            bool full = true;
            for (int j = 0; j < WIDTH; ++j)
            {
                if (!grid[i][j])
                {
                    full = false;
                    break;
                }
            }
            if (full)
            {
                grid.erase(grid.begin() + i);
                grid.insert(grid.begin(), std::vector<int>(WIDTH, 0));
                score += 100;
                speed -= 10;
                if (speed < 50)
                    speed = 50;
            }
        }
    }

public:
    // constructor initializing empty grid , piece, score and speed
    Game() : grid(HEIGHT, std::vector<int>(WIDTH, 0)), currentPiece(nullptr), nextPiece(getRandomPiece()), score(0), speed(300)
    {
        srand(static_cast<unsigned int>(time(0)));
        loadHighScore();
        clearScreen();
    }

    // destructor
    ~Game()
    {
        if (currentPiece)
            delete currentPiece;
        if (nextPiece)
            delete nextPiece;
    }

    int getSpeed()
    {
        return speed;
    }

    void setColor(int color)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

    int getPieceColor(const std::vector<std::vector<int>> &shape)
    {
        if (shape.size() == 1 || shape[0].size() == 4)
            return 9;
        if (shape.size() == 2 && shape[0].size() == 2)
            return 14;
        if (shape.size() == 2 && shape[0].size() == 3 && shape[1][0] == 1 && shape[1][2] == 1)
            return 13;
        if (shape.size() == 2 && shape[0][1] == 1 && shape[1][2] == 1)
            return 4;
        if (shape.size() == 2 && shape[1][0] == 1 && shape[0][1] == 1)
            return 2;
        if (shape.size() == 2 && shape[1][0] == 1)
            return 3;
        if (shape.size() == 2 && shape[0][2] == 1)
            return 8;

        return 7; // Default white
    }

    int calculateGhostY()
    {
        if (!currentPiece)
            return 0;

        int ghostY = currentPiece->y;
        while (isValidMove(currentPiece, currentPiece->x, ghostY + 1))
        {
            ghostY++;
        }
        return ghostY;
    }

    void draw()
    {
        COORD cursorPosition;
        cursorPosition.X = 0;
        cursorPosition.Y = 0;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);

        setColor(7); // White color for borders

        // Top border - now matches grid width
        std::cout << char(219);
        for (int j = 0; j < WIDTH * 2; j++)
            std::cout << char(219);
        std::cout << char(219) << "  Next Piece:";

        // Clear the next piece area (4 rows should be enough for any piece)
        for (int i = 1; i <= 4; i++)
        {
            cursorPosition.X = WIDTH * 2 + 4;
            cursorPosition.Y = i;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
            std::cout << "        "; // Clear about 4 blocks width
        }

        // Draw next piece preview
        if (nextPiece)
        {
            for (size_t pi = 0; pi < nextPiece->shape.size(); ++pi)
            {
                cursorPosition.X = WIDTH * 2 + 4;
                cursorPosition.Y = pi + 1;
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);

                for (size_t pj = 0; pj < nextPiece->shape[0].size(); ++pj)
                {
                    if (nextPiece->shape[pi][pj])
                    {
                        setColor(nextPiece->color);
                        std::cout << "[]";
                        setColor(7);
                    }
                    else
                    {
                        std::cout << "  ";
                    }
                }
            }
        }
        std::cout << "\n";

        // ... rest of the existing draw function ...

        // Calculate ghost position
        int ghostY = calculateGhostY();

        for (int i = 0; i < HEIGHT; ++i)
        {
            cursorPosition.X = 0;
            cursorPosition.Y = i + 1;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);

            std::cout << char(219); // Left border
            for (int j = 0; j < WIDTH; ++j)
            {
                bool isPieceCell = false;
                bool isGhostCell = false;

                // First check for actual piece (so it draws on top)
                if (currentPiece)
                {
                    for (size_t pi = 0; pi < currentPiece->shape.size(); ++pi)
                    {
                        for (size_t pj = 0; pj < currentPiece->shape[0].size(); ++pj)
                        {
                            int x = currentPiece->x + pj;
                            int y = currentPiece->y + pi;

                            if (currentPiece->shape[pi][pj] && x == j && y == i)
                            {
                                setColor(currentPiece->color);
                                std::cout << "[]";
                                setColor(7);
                                isPieceCell = true;
                            }
                        }
                    }
                }

                // Then draw ghost piece (only if not overlapping with actual piece)
                if (!isPieceCell && currentPiece && i >= ghostY && i < ghostY + (int)currentPiece->shape.size())
                {
                    for (size_t pi = 0; pi < currentPiece->shape.size(); ++pi)
                    {
                        for (size_t pj = 0; pj < currentPiece->shape[0].size(); ++pj)
                        {
                            int x = currentPiece->x + pj;
                            int y = ghostY + pi;

                            if (currentPiece->shape[pi][pj] && x == j && y == i)
                            {
                                // Check if this cell is on the border of the piece
                                bool isBorder = false;

                                // Left edge
                                if (pj == 0 || !currentPiece->shape[pi][pj - 1])
                                    isBorder = true;
                                // Right edge
                                else if (pj == currentPiece->shape[0].size() - 1 || !currentPiece->shape[pi][pj + 1])
                                    isBorder = true;
                                // Top edge
                                else if (pi == 0 || !currentPiece->shape[pi - 1][pj])
                                    isBorder = true;
                                // Bottom edge
                                else if (pi == currentPiece->shape.size() - 1 || !currentPiece->shape[pi + 1][pj])
                                    isBorder = true;

                                if (isBorder)
                                {
                                    setColor(8);                         // Grey color for ghost
                                    std::cout << char(219) << char(219); // Two block characters
                                    setColor(7);
                                    isGhostCell = true;
                                }
                            }
                        }
                    }
                }

                // Draw grid cells (only if neither piece nor ghost is here)
                if (!isPieceCell && !isGhostCell)
                {
                    if (grid[i][j])
                    {
                        setColor(grid[i][j]);
                        std::cout << "[]";
                        setColor(7);
                    }
                    else
                    {
                        std::cout << "  ";
                    }
                }
            }
            std::cout << char(219); // Right border
            std::cout << "\n";
        }

        // Bottom border - matches grid width
        cursorPosition.X = 0;
        cursorPosition.Y = HEIGHT + 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
        std::cout << char(219);
        for (int j = 0; j < WIDTH * 2; j++)
            std::cout << char(219);
        std::cout << char(219) << "\n";

        // Score display
        cursorPosition.X = 0;
        cursorPosition.Y = HEIGHT + 2;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
        std::cout << "Score: " << score << "   High Score: " << highScore << "  ";
    }

    void update()
    {
        if (!currentPiece)
        {
            currentPiece = nextPiece;
            nextPiece = getRandomPiece();
        }

        // Handle automatic falling
        if (isValidMove(currentPiece, currentPiece->x, currentPiece->y + 1))
        {
            currentPiece->y++;
        }
        else
        {
            placePiece();
            clearLines();
            if (isGameOver())
            {
                PlaySound(0, 0, 0);
                showGameOver();
                resetGame();
            }
        }

        // Handle keyboard input
        if (_kbhit())
        {
            char key = _getch();
            if (key == 0 || key == -32) // Handle special keys (arrow keys)
            {
                key = _getch(); // Get the actual key value
                switch (key)
                {
                case 75: // Left arrow key
                    if (isValidMove(currentPiece, currentPiece->x - 1, currentPiece->y))
                        currentPiece->x--;
                    break;

                case 77: // Right arrow key
                    if (isValidMove(currentPiece, currentPiece->x + 1, currentPiece->y))
                        currentPiece->x++;
                    break;

                case 72: // Up arrow key (Rotate)
                {
                    Tetromino temp = *currentPiece;
                    temp.rotate();

                    // Try normal rotation first
                    if (isValidMove(&temp, temp.x, temp.y, true))
                    {
                        currentPiece->rotate();
                    }
                    // Try wall kicks if normal rotation fails
                    else
                    {
                        // Try shifting left
                        if (isValidMove(&temp, temp.x - 1, temp.y, true))
                        {
                            currentPiece->rotate();
                            currentPiece->x--;
                        }
                        // Try shifting right
                        else if (isValidMove(&temp, temp.x + 1, temp.y, true))
                        {
                            currentPiece->rotate();
                            currentPiece->x++;
                        }
                        // Try shifting up (for ceiling kicks)
                        else if (isValidMove(&temp, temp.x, temp.y - 1, true))
                        {
                            currentPiece->rotate();
                            currentPiece->y--;
                        }
                    }

                    // Final wall boundary check (Step 3)
                    if (currentPiece->x < 0)
                    {
                        currentPiece->x = 0;
                    }
                    if (currentPiece->x + currentPiece->shape[0].size() > WIDTH)
                    {
                        currentPiece->x = WIDTH - currentPiece->shape[0].size();
                    }
                    break;
                }

                case 80: // Down arrow key (Soft drop)
                    if (isValidMove(currentPiece, currentPiece->x, currentPiece->y + 1))
                        currentPiece->y++;
                    break;
                }
            }
            else
            {
                switch (key)
                {
                case 32: // Spacebar (Hard drop)
                    while (isValidMove(currentPiece, currentPiece->x, currentPiece->y + 1))
                        currentPiece->y++;
                    placePiece();
                    clearLines();
                    break;

                case 27: // ESC key (Pause)
                    pauseGame();
                    break;
                }
            }
        }

        Sleep(speed);
    }

    void pauseGame()
    {
        clearScreen();
        PlaySound(0, 0, 0);
        std::cout << "==========================" << std::endl;
        std::cout << "        GAME PAUSED        " << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << "[1] Resume Game" << std::endl;
        std::cout << "[2] Exit Game" << std::endl;
        std::cout << "==========================" << std::endl;

        char choice;
        while (true)
        {
            choice = _getch();
            if (choice == '1')
            {
                clearScreen();
                draw();
                PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_LOOP);
                break;
            }
            else if (choice == '2')
            {
                exit(0);
            }
        }
    }

    void resetGame()
    {
        grid.assign(HEIGHT, std::vector<int>(WIDTH, 0));
        score = 0;
        speed = 300;
        clearScreen();

        PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_LOOP);
    }

    void showGameOver()
    {

        if (score > highScore)
        {
            highScore = score;
            saveHighScore(); // Save the new high score
        }
        system("cls"); // Clear screen when Game Over
        std::cout << "==========================" << std::endl;
        std::cout << "        GAME OVER!        " << std::endl;
        std::cout << "     Final Score: " << score << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << "[1] Restart Game" << std::endl;
        std::cout << "[2] Exit Game" << std::endl;
        std::cout << "==========================" << std::endl;

        char choice;
        while (true)
        {
            choice = _getch();
            if (choice == '1')
            {
                resetGame();
                break;
            }
            else if (choice == '2')
            {
                exit(0);
            }
        }
    }

    bool isGameOver()
    {
        for (int i = 0; i < WIDTH; ++i)
        {
            // Only consider game over if a BLOCK has SETTLED on row 0
            if (grid[0][i] && currentPiece == nullptr)
                return true;
        }
        return false;
    }
};

int main()
{
    showWelcomeScreen();
    Game game;
    while (true)
    {
        game.draw();
        game.update();
    }
    return 0;
}

void clearScreen()
{
    COORD topLeft = {0, 0};
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    // Get the number of character cells in the current buffer
    GetConsoleScreenBufferInfo(console, &screen);
    DWORD cells = screen.dwSize.X * screen.dwSize.Y;

    // Fill the entire screen with spaces
    FillConsoleOutputCharacter(console, ' ', cells, topLeft, &written);

    // Reset the cursor to the top left
    SetConsoleCursorPosition(console, topLeft);
}

void showWelcomeScreen()
{
    clearScreen();
    std::cout << "=====================================\n";
    std::cout << "         WELCOME TO TETRIS\n";
    std::cout << "=====================================\n";
    std::cout << "\n";
    std::cout << "CONTROLS:\n";
    std::cout << "  - Left Arrow     : Move Left\n";
    std::cout << "  - Right Arrow    : Move Right\n";
    std::cout << "  - Up Arrow       : Rotate Piece\n";
    std::cout << "  - Down Arrow     : Soft Drop (Faster fall)\n";
    std::cout << "  - Spacebar       : Hard Drop (Instant fall)\n";
    std::cout << "  - ESC            : Pause Game\n";
    std::cout << "\n";
    std::cout << "HOW TO PLAY:\n";
    std::cout << "  - Stack blocks to form complete lines.\n";
    std::cout << "  - Completed lines will clear and give you points.\n";
    std::cout << "  - The game ends when blocks reach the top.\n";
    std::cout << "\n";
    std::cout << "=====================================\n";
    std::cout << "  PRESS ANY KEY TO START THE GAME\n";
    std::cout << "=====================================\n";

    _getch(); // Wait for any key press
    clearScreen();
    PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_LOOP);
}