#include <iostream>
#include <coroutine>
#include <random>

struct inputTag {};
inputTag input{};

class Game {
public:
    struct promise_type;
    using handle = std::coroutine_handle<promise_type>;

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&& other) noexcept : coro(std::exchange(other.coro, {})) {}
    ~Game() {
        if (coro) {
            coro.destroy();
        }
    }

    int ask(int guessNumber) {
        if (!coro || coro.done()) {
            return 0;
        }

        auto& p = coro.promise();
        p.guessValue = guessNumber;
        coro.resume();

        int reply = p.hint;

        if (reply == 0 && !coro.done()) {
            coro.resume();
        }
        return reply;
    }

    struct promise_type {
        int guessValue = 0, hint = 0;

        Game get_return_object() {
            return Game{handle::from_promise(*this)};
        }

        auto initial_suspend() noexcept {
            return std::suspend_always{};
        }
        auto final_suspend() noexcept {
            return std::suspend_always{};;
        }
        void unhandled_exception() {
            std::terminate();
        }
        void return_void() {}

        struct inputAwaiter {
            promise_type* p;
            int await_resume() noexcept {return p->guessValue;}
            bool await_ready() const { return true; }
            void await_suspend(std::coroutine_handle<>) noexcept {}
        };

        inputAwaiter await_transform(inputTag) noexcept {
            return inputAwaiter{this};
        }

        auto yield_value(int reply) noexcept {
            hint = reply;
            return std::suspend_always{};
        }
    };
private:
    explicit Game(handle h) : coro(h) {}
    handle coro;
};

Game guesser() {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution dist(1, 100);

    int secretNumber = dist(generator);

    while (true) {
        int guessNumber = co_await input;
        int reply;

        if (guessNumber > secretNumber) {
            reply = -1;
        }
        else if (guessNumber < secretNumber) {
            reply = 1;
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

void runGame(Game& game) {
    std::cout << "Guess the number from 1 to 100\n";
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
            std::cout << "Answer: 1 (the secret number is >)\n";
        }
        else if (answer == -1) {
            std::cout << "Answer: -1 (the secret number is <)\n";
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