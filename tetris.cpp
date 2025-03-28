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
    struct Shape
    {
        char name;
        int states[4][4][4];
        int currState = 0;
    };

    static Shape shapes[7];

    Shape generateTetromino()
    {
        return shapes[rand() % 7];
    }

    static string getColor(char type)
    {
        switch (type)
        {
        case 'I':
            return "9"; // Cyan
        case 'O':
            return "14"; // Yellow
        case 'T':
            return "13"; // Magenta
        case 'S':
            return "2"; // Green
        case 'Z':
            return "4"; // Red
        case 'J':
            return "1"; // Blue
        case 'L':
            return "6"; // White
        default:
            return "7"; // Default white
        }
    }

    static void rotate(Shape &t)
    {
        t.currState = (t.currState + 1) % 4;
    }
};

Tetromino::Shape Tetromino::shapes[7] = {
    {'I', {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}}, {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}}}, 0},

    {'O', {{{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}, 0},

    {'T', {{{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}, {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}, {{0, 1, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}, 0},

    {'S', {{{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}, {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}}, 0},

    {'Z', {{{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}, {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}, 0},

    {'J', {{{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}, {{0, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}, {{0, 1, 0, 0}, {0, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}}}, 0},

    {'L', {{{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}, {{0, 0, 0, 0}, {1, 1, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}}, {{1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}}, 0}};

class Game
{
private:
    std::vector<std::vector<int>> grid;
    Tetromino::Shape currentPiece;
    Tetromino::Shape nextPiece;
    int pieceX, pieceY;
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

    bool collided(int x, int y, const Tetromino::Shape &t)
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (t.states[t.currState][i][j] == 0)
                    continue;

                int newX = x + j;
                int newY = y + i;

                if (newX < 0 || newX >= WIDTH || newY >= HEIGHT)
                    return true;
                if (newY >= 0 && grid[newY][newX] != 0)
                    return true;
            }
        }
        return false;
    }

    int findGhostY()
    {
        int ghostY = pieceY;
        while (!collided(pieceX, ghostY + 1, currentPiece))
        {
            ghostY++;
        }
        return ghostY;
    }

    void placePiece()
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (currentPiece.states[currentPiece.currState][i][j] == 1)
                {
                    int x = pieceX + j;
                    int y = pieceY + i;

                    if (y >= 0 && x >= 0 && x < WIDTH && y < HEIGHT)
                    {
                        grid[y][x] = stoi(Tetromino::getColor(currentPiece.name));
                    }
                }
            }
        }
        clearLines();

        // Get new piece
        currentPiece = nextPiece;
        nextPiece = Tetromino().generateTetromino();
        pieceX = WIDTH / 2 - 2;
        pieceY = 0;

        if (collided(pieceX, pieceY, currentPiece))
        {
            gameOver = true;
        }
    }

    void clearLines()
    {
        int linesCleared = 0;

        // Start from bottom and move up
        for (int y = HEIGHT - 1; y >= 0; y--)
        {
            bool full = true;
            for (int x = 0; x < WIDTH; x++)
            {
                if (grid[y][x] == 0)
                {
                    full = false;
                    break;
                }
            }

            if (full)
            {
                // Remove the full line
                grid.erase(grid.begin() + y);
                // Add a new empty line at the top
                grid.insert(grid.begin(), std::vector<int>(WIDTH, 0));
                linesCleared++;
                y++; // Important: After erasing and inserting, check the same y again
            }
        }

        // Update score based on lines cleared
        if (linesCleared > 0)
        {
            switch (linesCleared)
            {
            case 1:
                score += 100;
                break;
            case 2:
                score += 300;
                break;
            case 3:
                score += 500;
                break;
            case 4:
                score += 800;
                break;
            }

            // Increase speed slightly to make game harder
            if (speed > 100)
                speed -= 5;

            if (score > highScore)
            {
                highScore = score;
                saveHighScore();
            }
        }
    }

public:
    bool gameOver;

    Game() : grid(HEIGHT, std::vector<int>(WIDTH, 0)), score(0), speed(300), gameOver(false)
    {
        srand(static_cast<unsigned int>(time(0)));
        loadHighScore();
        Tetromino tetromino;
        currentPiece = tetromino.generateTetromino();
        nextPiece = tetromino.generateTetromino();
        pieceX = WIDTH / 2 - 2;
        pieceY = 0;
        clearScreen();
    }

    void setColor(int color)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
    }

    void draw()
    {
        COORD cursorPosition;
        cursorPosition.X = 0;
        cursorPosition.Y = 0;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);

        setColor(7); // White color for borders

        // Top border
        std::cout << char(219);
        for (int j = 0; j < WIDTH * 2; j++)
            std::cout << char(219);
        std::cout << char(219) << "  Next Piece:";

        // Draw next piece preview
        for (int i = 0; i < 4; i++)
        {
            cursorPosition.X = WIDTH * 2 + 4;
            cursorPosition.Y = i + 1;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);

            for (int j = 0; j < 4; j++)
            {
                if (nextPiece.states[nextPiece.currState][i][j])
                {
                    setColor(stoi(Tetromino::getColor(nextPiece.name)));
                    std::cout << "[]";
                    setColor(7);
                }
                else
                {
                    std::cout << "  ";
                }
            }
        }

        // Calculate ghost position
        int ghostY = findGhostY();

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

                // Draw current piece
                for (int pi = 0; pi < 4; pi++)
                {
                    for (int pj = 0; pj < 4; pj++)
                    {
                        if (currentPiece.states[currentPiece.currState][pi][pj] &&
                            j == pieceX + pj && i == pieceY + pi)
                        {
                            setColor(stoi(Tetromino::getColor(currentPiece.name)));
                            std::cout << "[]";
                            setColor(7);
                            isPieceCell = true;
                        }
                    }
                }

                // Draw ghost piece
                if (!isPieceCell)
                {
                    for (int pi = 0; pi < 4; pi++)
                    {
                        for (int pj = 0; pj < 4; pj++)
                        {
                            if (currentPiece.states[currentPiece.currState][pi][pj] &&
                                j == pieceX + pj && i == ghostY + pi)
                            {
                                setColor(8); // Grey
                                std::cout << "::";
                                setColor(7);
                                isGhostCell = true;
                            }
                        }
                    }
                }

                // Draw grid cells
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

        // Bottom border
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
        // Handle automatic falling
        if (!collided(pieceX, pieceY + 1, currentPiece))
        {
            pieceY++;
        }
        else
        {
            placePiece();
            if (gameOver)
            {
                showGameOver();
                resetGame();
            }
        }

        // Handle keyboard input - only process the most recent key
        if (_kbhit())
        {
            // Clear all buffered keys and only keep the most recent one
            char key = 0;
            while (_kbhit())
            {
                key = _getch();
                if (key == 0 || key == -32)
                {                   // Arrow key prefix
                    key = _getch(); // Get the actual arrow key
                }
            }

            // Process only the most recent key
            switch (key)
            {
            case 75: // Left arrow key
                if (!collided(pieceX - 1, pieceY, currentPiece))
                    pieceX--;
                break;

            case 77: // Right arrow key
                if (!collided(pieceX + 1, pieceY, currentPiece))
                    pieceX++;
                break;

            case 72: // Up arrow key (Rotate)
            {
                Tetromino::Shape temp = currentPiece;
                Tetromino::rotate(temp);
                if (!collided(pieceX, pieceY, temp))
                    currentPiece = temp;
                break;
            }

            case 80: // Down arrow key (Soft drop)
                if (!collided(pieceX, pieceY + 1, currentPiece))
                    pieceY++;
                break;

            case 32: // Spacebar (Hard drop)
                while (!collided(pieceX, pieceY + 1, currentPiece))
                    pieceY++;
                placePiece();
                if (gameOver)
                {
                    showGameOver();
                    resetGame();
                }
                break;

            case 27: // ESC key (Pause)
                pauseGame();
                break;
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
        gameOver = false;
        Tetromino tetromino;
        currentPiece = tetromino.generateTetromino();
        nextPiece = tetromino.generateTetromino();
        pieceX = WIDTH / 2 - 2;
        pieceY = 0;
        clearScreen();
        PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_LOOP);
    }

    void showGameOver()
    {
        if (score > highScore)
        {
            highScore = score;
            saveHighScore();
        }
        system("cls");
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

    GetConsoleScreenBufferInfo(console, &screen);
    DWORD cells = screen.dwSize.X * screen.dwSize.Y;

    FillConsoleOutputCharacter(console, ' ', cells, topLeft, &written);
    SetConsoleCursorPosition(console, topLeft);
}

void showWelcomeScreen()
{
    clearScreen();

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Title
    SetConsoleTextAttribute(hConsole, 13); // Purple
    std::cout << "=====================================\n";
    std::cout << "         WELCOME TO TETRIS\n";
    std::cout << "=====================================\n\n";

    // Controls header
    SetConsoleTextAttribute(hConsole, 11); // Light cyan
    std::cout << "CONTROLS:\n";

    // Controls items
    SetConsoleTextAttribute(hConsole, 7); // White
    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 10); // Light green
    std::cout << "Left Arrow";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "     : Move Left\n";

    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 10);
    std::cout << "Right Arrow";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "    : Move Right\n";

    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 10);
    std::cout << "Up Arrow";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "       : Rotate Piece\n";

    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 10);
    std::cout << "Down Arrow";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "     : Soft Drop (Faster fall)\n";

    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 10);
    std::cout << "Spacebar";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "       : Hard Drop (Instant fall)\n";

    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 10);
    std::cout << "ESC";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "            : Pause Game\n\n";

    // Scoring header
    SetConsoleTextAttribute(hConsole, 11); // Light cyan
    std::cout << "SCORING:\n";

    // Scoring items with colors matching the pieces
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 9); // Cyan (I piece)
    std::cout << "1 line";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << " cleared: ";
    SetConsoleTextAttribute(hConsole, 14); // Yellow
    std::cout << "100 points\n";

    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 2); // Green (S piece)
    std::cout << "2 lines";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << " cleared: ";
    SetConsoleTextAttribute(hConsole, 14);
    std::cout << "300 points\n";

    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 4); // Red (Z piece)
    std::cout << "3 lines";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << " cleared: ";
    SetConsoleTextAttribute(hConsole, 14);
    std::cout << "500 points\n";

    SetConsoleTextAttribute(hConsole, 7);
    std::cout << "  - ";
    SetConsoleTextAttribute(hConsole, 5); // Purple (T piece)
    std::cout << "4 lines";
    SetConsoleTextAttribute(hConsole, 7);
    std::cout << " cleared: ";
    SetConsoleTextAttribute(hConsole, 14);
    std::cout << "800 points\n\n";

    // How to play
    SetConsoleTextAttribute(hConsole, 11); // Light cyan
    std::cout << "HOW TO PLAY:\n";
    SetConsoleTextAttribute(hConsole, 7); // White
    std::cout << "  - Stack blocks to form complete lines.\n";
    std::cout << "  - Completed lines will clear and give you points.\n";
    std::cout << "  - The game ends when blocks reach the top.\n\n";

    // Footer
    SetConsoleTextAttribute(hConsole, 13); // Purple
    std::cout << "=====================================\n";
    SetConsoleTextAttribute(hConsole, 10); // Light green
    std::cout << "  PRESS ANY KEY TO START THE GAME\n";
    SetConsoleTextAttribute(hConsole, 13);
    std::cout << "=====================================\n";

    SetConsoleTextAttribute(hConsole, 7); // Reset to white

    _getch();
    clearScreen();
    PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC | SND_LOOP);
}