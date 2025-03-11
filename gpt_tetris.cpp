#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream> // Add this for file handling

const int WIDTH = 10;
const int HEIGHT = 20;

class Tetromino
{
public:
    std::vector<std::vector<int>> shape;
    int x, y;
    int color; // ✅ Fixed color for this piece

    Tetromino(std::vector<std::vector<int>> s, int startX, int startY, int c)
        : shape(s), x(startX), y(startY), color(c) {}

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
    int score;
    int speed;
    int highScore;

    void loadHighScore()
    {
        std::ifstream file("highscore.txt");
        if (file.is_open())
        {
            file >> highScore;
            file.close();
        }
        else
        {
            highScore = 0; // Default value if no file exists
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

        int color[] = {9, 14, 13, 2, 4, 6, 5}; // ✅ Fixed color for each piece

        int index = rand() % pieces.size();
        return new Tetromino(pieces[index], WIDTH / 2 - pieces[index][0].size() / 2, -2, color[index]);
    }

    bool isValidMove(Tetromino *piece, int newX, int newY)
    {
        for (size_t i = 0; i < piece->shape.size(); ++i)
        {
            for (size_t j = 0; j < piece->shape[0].size(); ++j)
            {
                if (piece->shape[i][j])
                {
                    int x = newX + j;
                    int y = newY + i;
                    if (x < 0 || x >= WIDTH || y >= HEIGHT || (y >= 0 && grid[y][x]))
                    {
                        return false;
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

                    // ✅ Ignore any blocks that are placed above the screen
                    if (yPos >= 0)
                        grid[yPos][xPos] = currentPiece->color;
                }
            }
        }
        delete currentPiece;
        currentPiece = nullptr;
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
                speed -= 5;
                if (speed < 50)
                    speed = 50;
            }
        }
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

public:
    Game() : grid(HEIGHT, std::vector<int>(WIDTH, 0)), currentPiece(nullptr), score(0), speed(300)
    {
        srand(static_cast<unsigned int>(time(0)));
        loadHighScore();
        clearScreen();
    }

    ~Game()
    {
        if (currentPiece)
            delete currentPiece;
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

    void draw()
    {
        COORD cursorPosition;
        cursorPosition.X = 0;
        cursorPosition.Y = 0;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);

        for (int i = 0; i < HEIGHT; ++i)
        {
            for (int j = 0; j < WIDTH; ++j)
            {
                bool isPieceCell = false;
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
                                // ✅ Apply the correct color based on piece type
                                setColor(currentPiece->color);
                                std::cout << "O ";
                                setColor(7); // Reset color to white
                                isPieceCell = true;
                            }
                        }
                    }
                }

                if (!isPieceCell)
                {
                    if (grid[i][j])
                    {
                        // ✅ Retain the color of placed blocks
                        setColor(grid[i][j]);
                        std::cout << "X ";
                        setColor(7);
                    }
                    else
                    {
                        std::cout << ". ";
                    }
                }
            }
            std::cout << std::endl;
        }

        std::cout << "Score: " << score << "   High Score: " << highScore << std::endl;
    }

    void update()
    {
        if (!currentPiece)
        {
            currentPiece = getRandomPiece();
        }

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
                showGameOver();
                resetGame();
            }
        }

        if (_kbhit())
        {
            char key = _getch();
            switch (key)
            {
            case 'a':
                if (isValidMove(currentPiece, currentPiece->x - 1, currentPiece->y))
                    currentPiece->x--;
                break;
            case 'd':
                if (isValidMove(currentPiece, currentPiece->x + 1, currentPiece->y))
                    currentPiece->x++;
                break;
            case 'w':
            {
                Tetromino temp = *currentPiece;
                temp.rotate();
                if (isValidMove(&temp, temp.x, temp.y))
                    currentPiece->rotate();
                break;
            }
            case 's':
                if (isValidMove(currentPiece, currentPiece->x, currentPiece->y + 1))
                    currentPiece->y++;
                break;
            case 32:
                while (isValidMove(currentPiece, currentPiece->x, currentPiece->y + 1))
                    currentPiece->y++;
                placePiece();
                clearLines();
                break;
            case 27:
                exit(0);
            }
        }

        Sleep(speed);
    }

    void resetGame()
    {
        grid.assign(HEIGHT, std::vector<int>(WIDTH, 0));
        score = 0;
        speed = 300;
        clearScreen();
    }

    void showGameOver()
    {

        if (score > highScore)
        {
            highScore = score;
            saveHighScore(); // Save the new high score
        }
        system("cls"); // ✅ Clear screen when Game Over
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
            // ✅ Only consider game over if a BLOCK has SETTLED on row 0
            if (grid[0][i] && currentPiece == nullptr)
                return true;
        }
        return false;
    }
};

int main()
{
    Game game;
    while (true)
    {
        game.draw();
        game.update();
    }
    return 0;
}
