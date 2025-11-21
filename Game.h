#pragma once

#include <coroutine>
#include <utility>
#include <stdexcept>

struct inputTag {};

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

Game guesser();