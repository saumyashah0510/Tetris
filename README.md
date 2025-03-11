# Tetris (C++)

## Description:
A console-based Tetris game built in C++ with colorful blocks, scoring system, and high-score saving. Enjoy a classic Tetris experience with intuitive controls and a nostalgic feel. 
<br>

## Team: Silicon Syndicates
202401472 - Saumya Shah <br>
202404038 - Manav Shah  <br>
202404030 - Pranshu Vaghani <br>
202404017 - Krishh Agrawal <br>

## 🚀Features:
✅ Classic Tetris gameplay  
✅ Color-coded Tetromino pieces  
✅ Scoring system with high score tracking  
✅ Pause and Resume feature  
✅ Soft drop and Hard drop for faster gameplay  
✅ Save and load high scores automatically  
✅ Sound effects to enhance the gaming experience

<br>

## 🎮 Controls
| Key          | Action                  |
|:--------------|:------------------------|
| **Left Arrow**   | Move piece left          |
| **Right Arrow**  | Move piece right         |
| **Up Arrow**      | Rotate piece clockwise   |
| **Down Arrow**    | Soft drop (faster fall)  |
| **Spacebar**      | Hard drop (instant fall) |
| **ESC**           | Pause the game           |

<br>

## 📋 How to Play
1. **Stack blocks** to form complete horizontal lines.
2. **Completed lines** will be cleared, and points will be awarded.
3. The game ends when the **blocks reach the top** of the grid.

<br>

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

## 🏆 Scoring System
- 100 Points for each completed line
- Game speed increases as you score more points
- Achieving a new high score will save automatically

<br>

## 🔊 Sound Support
- The game includes sound effects during gameplay.
- Ensure that sound.wav is in the same folder as the executable for proper functionality.

## License:
This project is open-source and can be modified as needed.
