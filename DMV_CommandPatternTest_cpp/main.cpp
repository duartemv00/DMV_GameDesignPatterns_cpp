#include <iostream>
#include <algorithm>
#include <cstdlib> // clear console before every draw
#include <conio.h> 
#include <memory>
#include <unordered_map>
#include <vector>

// 
struct Unit{
    int x = 0, y = 0;

    Unit(int startX, int startY) { x = startX; y = startY; }
};

// Interface (can't be created)
class Command {
public:
    virtual ~Command() = default;
    virtual void execute(Unit& unit) = 0;
    virtual void undo(Unit& unit) = 0;
    virtual std::unique_ptr<Command> clone() const = 0;
};

constexpr int kWidth = 10;
constexpr int kHeigth = 6;

// Implementation of Command (Move action)
class MoveCommand : public Command {
public:
    MoveCommand(int dx, int dy) : dx_(dx), dy_(dy) {}

    void execute(Unit& unit) override {
        prevX_ = unit.x;
        prevY_ = unit.y;

        unit.x += dx_;
        unit.y += dy_;

        unit.x = std::clamp(unit.x, 0, kWidth - 1);
        unit.y = std::clamp(unit.y, 0, kHeigth - 1);
    }

    void undo(Unit& unit) override {
        unit.x = prevX_;
        unit.y = prevY_;
    }

    std::unique_ptr<Command> clone() const override{
        return std::make_unique<MoveCommand>(*this);
    }

private:
    int dx_ = 0, dy_ = 0;
    int prevX_ = 0, prevY_ = 0;
};

//
class InputHandler{
public:
    void bind(char key, std::unique_ptr<Command> command) {
        bindings_[key] = std::move(command);
    }

    std::unique_ptr<Command> handleInput(char key){ // Command*
        auto it = bindings_.find(key); // iterator
        if(it == bindings_.end()){ // compare to after last entry
            return nullptr;
        }
        return it->second->clone();

        // return it->second.get();
        // it -- an iterator: a "cursor" pointing at one entry in the map
        // it->second -- the value of that entry: std::unique_ptr<Command>. Key is first
        // it->second.get() -- the raw pointer inside the unique_ptr: Command*
    }

private:
    std::unordered_map<char, std::unique_ptr<Command>> bindings_;
};

// 
void draw(const Unit& unit){
    std::cout << "\x1b[2J\x1b[H"; // clear console

    for (int y = 0; y < kHeigth; y++) {
        for (int x = 0; x < kWidth; x++) {
            if (unit.x == x && unit.y == y){
                std::cout << "@";
            } else std::cout << ".";
        }
        std::cout << "\n";
    }
}

int main() { // int argc, char* argv[]

    Unit unit(0,0);
    draw(unit);

    // MoveCommand up(0, -1);

    // MoveCommand templates
    std::unique_ptr up = std::make_unique<MoveCommand>(0, -1);
    std::unique_ptr down = std::make_unique<MoveCommand>(0, 1);
    std::unique_ptr right = std::make_unique<MoveCommand>(1, 0);
    std::unique_ptr left = std::make_unique<MoveCommand>(-1, 0);

    InputHandler handler;
    // must be input.bind('w', std::move(up)) because input.bind('w', up) tries to copy a unique_ptr, which isn't allowed
    handler.bind('w', std::move(up));
    handler.bind('s', std::move(down));
    handler.bind('d', std::move(right));
    handler.bind('a', std::move(left));

    // History
    std::vector<std::unique_ptr<Command>> history;
    std::vector<std::unique_ptr<Command>> redoStack;

    char input = '.';
    while(input != 'q'){
        input = static_cast<char>(_getch()); // std::cin >> input;

        // undo check
        if(input == 'u'){
            if(!history.empty()){ // cant check history.back() direclty here
                history.back()->undo(unit);
                redoStack.push_back(std::move(history.back()));
                history.pop_back();

                draw(unit);
            }
            continue;
        }

        // redo check
        if(input == 'r'){
            if(!redoStack.empty()){
                redoStack.back()->execute(unit);
                history.push_back(std::move(redoStack.back()));
                redoStack.pop_back();

                draw(unit);
            }
            continue;
        }

        std::unique_ptr<Command> command = handler.handleInput(input); // this is a copy

        if (command) {
            command->execute(unit);
            history.push_back(std::move(command));
            redoStack.clear();
            draw(unit);
        }
    }

    return 0;
}