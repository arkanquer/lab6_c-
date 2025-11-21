#include "Game.h"
#include <random>

inputTag input{};

Game guesser() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution dist(1, 100);

    int secretNumber = dist(generator);

    while (true) {
        int guessNumber = co_await input;
        int reply;

        if (guessNumber > secretNumber) {
            reply = 1;
        }
        else if (guessNumber < secretNumber) {
            reply = -1;
        }
        else {
            reply = 0;
        }

        co_yield reply;
        if (reply == 0) {
            co_return;
        }
    }
}