# 🎮 Tetris (C++)
Experience the nostalgia of Tetris in your terminal! This C++ implementation features colorful blocks, smooth controls, and a scoring system with high-score saving. Perfect for both casual gaming and coding inspiration!

## 🚀 Features
- Classic Tetris gameplay
- Color-coded Tetromino pieces (I, O, T, S, Z, J, L)
- Scoring system with high score tracking
- Pause/Resume functionality
- Soft drop (faster fall) and Hard drop (instant fall)
- Ghost piece preview
- Sound effects

## 🕹️ Controls
| Key          | Action                  |
|--------------|-------------------------|
| ← → Arrow    | Move piece left/right   |
| ↑ Arrow      | Rotate piece            |
| ↓ Arrow      | Soft drop               |
| Spacebar     | Hard drop               |
| ESC          | Pause game              |

## 📋 How to Play
1. Stack blocks to form complete horizontal lines
2. Completed lines will clear and award points
3. Game ends when blocks reach the top

## 🏆 Scoring
| Lines Cleared | Points |
|---------------|--------|
| 1             | 100    |
| 2             | 300    |
| 3             | 500    |
| 4             | 800    |

Speed increases with higher scores

## 🖥️ Installation and Running the Game
### Prerequisites
- **Windows OS** (due to Windows-specific console features)
- **C++ Compiler** (e.g., MinGW, Visual Studio, etc.)

### Steps to Compile and Run
1. **Clone the Repository**
   ```bash
   git clone <https://github.com/saumyashah0510/Tetris.git>
   cd tetris-CPP

2. **Compile the Code Using g++ compiler:**
    ```bash
    g++ tetris.cpp -o tetris -lwinmm

3. **Run the game**
    ```bash
    ./tetris.exe

<br>

## 🔊 Sound Support
- The game includes sound effects during gameplay.
- Ensure that sound.wav is in the same folder as the executable for proper functionality.

## Sample Gameplay video:


https://github.com/user-attachments/assets/b35cfe57-2516-494b-97cc-37689f77cd38



## License:
This project is open-source and can be modified as needed.
