
#pragma once
#include <unordered_map>


namespace farixEngine {
enum class Key {
    W,
    A,
    S,
    D,
    Q,
    E,
    Space,
    Shift,
    Escape};
struct Controller {
    // Keyboard state
    std::unordered_map<Key, bool> keyState;

    // Mouse state
    bool rightClick = false;
    bool inMotion = false;
    int dx = 0;
    int dy = 0;


    void setKeyState(Key key, bool pressed) {
        keyState[key] = pressed;
    }

    bool isKeyPressed(Key key) const {
        auto it = keyState.find(key);
        return it != keyState.end() && it->second;
    }

    void resetMotion() {
        dx = 0;
        dy = 0;
        inMotion = false;
    }
};

}

