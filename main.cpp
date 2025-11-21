#include <iostream>
#include "Game.h"

void runGame(Game& game) {
    std::cout << "Gues the number from 1 to 100\n";
    while (true) {
        int guess;
        std::cout << "Your try: ";

        if (!(std::cin >> guess)) {
            std::cout << "Invalid input, enter an integer.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        int answer = game.ask(guess);
        if (answer == 1) {
            std::cout << "Answer: 1 (the secret number is <)\n";
        }
        else if (answer == -1) {
            std::cout << "Answer: -1 (the secret number is >)\n";
        }
        else {
            std::cout << "Answer: 0 (you found the secret number!)\n";
            break;
        }
    }
}

int main() {
    auto game = guesser();
    runGame(game);
    return 0;
}