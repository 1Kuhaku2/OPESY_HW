#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>
#include <atomic>
#include <iostream>
#include <cstdlib>
#include <ctime>

class ConsoleMarquee {
public:
    ConsoleMarquee();
    void start();

private:
    void clearScreen();
    void moveToPosition(int x, int y);
    void displayFrame();
    void checkKeyboard();
    void updatePosition();
    void handleCommand();

    struct MovingText {
        int posX, posY;  // Coordinates
        int deltaX, deltaY; // Change in position
        const char* content; // Text content
    };

    const int MAX_WIDTH = 30;
    const int MAX_HEIGHT = 15;
    const int FRAME_RATE = 60;
    const int KEYBOARD_POLLING = 33;

    MovingText text;
    std::string inputBuffer;
    std::vector<std::string> executedCommands;
    std::atomic<bool> isRunning{ true };
};

ConsoleMarquee::ConsoleMarquee()
    : text{ rand() % (MAX_WIDTH - 6) + 1, rand() % (MAX_HEIGHT - 2) + 1, 1, 1, "Hello in marquee!" } {
    srand(static_cast<unsigned>(time(0)));
}

void ConsoleMarquee::clearScreen() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(consoleHandle, &bufferInfo);
    DWORD cells = bufferInfo.srWindow.Right * bufferInfo.srWindow.Bottom;
    DWORD writtenChars;
    FillConsoleOutputCharacter(consoleHandle, ' ', cells, { 0, 0 }, &writtenChars);
    SetConsoleCursorPosition(consoleHandle, { 0, 0 });
}

void ConsoleMarquee::moveToPosition(int x, int y) {
    COORD coordinates = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinates);
}

void ConsoleMarquee::displayFrame() {
    clearScreen();

    // Draw static frame
    std::cout << "*************************************************\n";
    std::cout << "* Displaying a marquee console *\n";
    std::cout << "*************************************************\n";

    // Draw the moving text
    moveToPosition(text.posX, text.posY + 4);
    std::cout << text.content;

    // Show command history
    int messageLine = MAX_HEIGHT + 6;
    for (const auto& msg : executedCommands) {
        moveToPosition(0, messageLine++);
        std::cout << msg;
    }

    // Draw the input prompt
    moveToPosition(0, MAX_HEIGHT + 5);
    std::cout << "Enter a command for MAQRUEE_CONSOLE: " << inputBuffer;
}

void ConsoleMarquee::checkKeyboard() {
    while (_kbhit()) {
        char keyPress = _getch();

        if (keyPress == '\r') { // Enter key
            handleCommand();
        }
        else if (keyPress == 8) { // Backspace
            if (!inputBuffer.empty()) inputBuffer.pop_back();
        }
        else if (isprint(keyPress)) { // Add printable characters
            inputBuffer += keyPress;
        }
    }
}

void ConsoleMarquee::handleCommand() {
    if (inputBuffer == "exit") {
        isRunning = false;
    }
    else {
        executedCommands.push_back("Command processed in MARQUEE_CONSOLE: " + inputBuffer);
        inputBuffer.clear();
    }
}

void ConsoleMarquee::updatePosition() {
    text.posX += text.deltaX;
    text.posY += text.deltaY;

    // Boundary conditions for text movement
    if (text.posX <= 1) text.deltaX = 1;
    if (text.posX >= MAX_WIDTH - 4) text.deltaX = -1;
    if (text.posY <= 1) text.deltaY = 1;
    if (text.posY >= MAX_HEIGHT - 2) text.deltaY = -1;
}

void ConsoleMarquee::start() {
    auto prevFrameTime = std::chrono::steady_clock::now();

    while (isRunning) {
        auto currentTime = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevFrameTime).count() >= (1000 / FRAME_RATE)) {
            updatePosition();
            displayFrame();
            prevFrameTime = currentTime;
        }

        checkKeyboard();
        std::this_thread::sleep_for(std::chrono::milliseconds(KEYBOARD_POLLING));
    }

    clearScreen();
    std::cout << "Program Terminated.\n";
}

int main() {
    ConsoleMarquee marqueeApp;
    marqueeApp.start();
    return 0;
}
