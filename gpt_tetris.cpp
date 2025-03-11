#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>

const int WIDTH = 10;
const int HEIGHT = 20;

class Tetromino
{
public:
    std::vector<std::vector<int>> shape;
    int x, y;

    Tetromino(std::vector<std::vector<int>> s, int startX, int startY) : shape(s), x(startX), y(startY) {}

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

    std::vector<std::vector<int>> getRandomPiece()
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
        return pieces[rand() % pieces.size()];
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
                        grid[yPos][xPos] = 1;
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
        system("cls"); // Clear the console screen
    }

public:
    Game() : grid(HEIGHT, std::vector<int>(WIDTH, 0)), currentPiece(nullptr), score(0), speed(300)
    {
        srand(static_cast<unsigned int>(time(0)));
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
                                std::cout << "O ";
                                isPieceCell = true;
                            }
                        }
                    }
                }
                if (!isPieceCell)
                    std::cout << (grid[i][j] ? "X " : ". ");
            }
            std::cout << std::endl;
        }
        std::cout << "Score: " << score << std::endl;
    }

    void update()
    {
        if (!currentPiece)
        {
            std::vector<std::vector<int>> pieceShape = getRandomPiece();
            currentPiece = new Tetromino(pieceShape, WIDTH / 2 - pieceShape[0].size() / 2, -2);
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
        system("cls"); // ✅ Clear screen when restarting
    }

    void showGameOver()
    {
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
