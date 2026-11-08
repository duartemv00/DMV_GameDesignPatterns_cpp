# Command Pattern in C++

A small console game for practicing C++ and the **Command** game-programming pattern. A unit (`@`) moves around a grid with `W` `A` `S` `D`, every move can be undone with `U` and redone with `R`. `Q` quits.

```
@.........
..........
..........
```

## Introduction

### The problem

The first version handled input with a `switch`:

```cpp
switch (input) {
case 'w': unit.y -= 1; break;
case 'a': unit.x -= 1; break;
// ...
}
```

It works, but the key, the decision and the action are all mixed together in the game loop:

- **Key bindings are hard-coded.** Remapping a key means changing the code.
- **Actions are gone as soon as they happen.** Nothing remembers what was done, so there is nothing to undo, redo or replay.
- **Only the keyboard can drive the unit.** An AI or a network player would need its own copy of the movement logic.

### The idea

The Command pattern **turns an action into an object**. Instead of a key press *doing* something directly, it *creates* an object that represents the action, and that object is then executed:

```
before:  key ──► change the unit
after:   key ──► Command object ──► execute(unit)
```

Because the action is now an object, it can be stored, passed around, queued, given to an AI, put in a history, and undone.

### Structure in this project

| Piece | Its only job |
|---|---|
| `Command` | Abstract interface: every action has `execute`, `undo` and `clone`. |
| `MoveCommand` | A concrete command: moves the unit by `dx`/`dy`, clamps it to the grid, and remembers the previous position so it can undo. |
| `InputHandler` | Maps keys to command *templates*. Each key press returns a fresh copy. |
| `history` | A `std::vector` used as a stack of executed commands. Undo takes the last one off. |
| `redoStack` | A second stack holding undone commands. Redo takes the last one off and executes it again. |
| Game loop | Reads a key, executes whatever command it gets, draws. It never mentions specific keys or movement. |

Each piece knows as little as possible about the others. That separation is the point of the pattern.

### Key design decisions

**One class for all directions.** `MoveCommand(0, -1)` is up, `MoveCommand(1, 0)` is right, and so on. The movement logic lives in one place instead of four `case`s.

**Key bindings are data.** `InputHandler` stores an `unordered_map<char, unique_ptr<Command>>`. Rebinding a key is a call to `bind()`, even while the game runs. The game loop doesn't change.

**Undo restores state; it doesn't reverse the action.** Undoing a move by moving in the opposite direction fails at walls. At `x = 0`, pressing `A` doesn't move the unit (the clamp stops it), but "move right" as an undo would push it to `x = 1`. So each `MoveCommand` saves the unit's position *before* it moves, and `undo` puts it back there.

**Each key press gets its own command object (the Prototype pattern).** If the handler returned the same `MoveCommand` every time, each press would overwrite the previous position saved by the last one. Instead, the handler keeps one *template* per key and returns `clone()` of it, so every entry in the history has its own memory. Creating objects by copying an existing one is the **Prototype** pattern.

**Undo and redo move commands between two stacks.** Undo calls `undo` on the last command in `history` and moves it to `redoStack`. Redo calls `execute` on the last command in `redoStack` and moves it back to `history`. Redo runs `execute` again rather than restoring a stored position, so the command saves a fresh previous position and undo keeps working however many times you alternate.

**A new action clears the redo stack.** After undoing, a new move starts a different path, so the undone commands no longer make sense to redo. This is the same convention every editor follows.

**Every action is a `Command`, so every action is undoable.** A new action type (attack, pick up an item, …) only needs a new class deriving from `Command`. The input handler, history and game loop work with it unchanged.

### Roadmap

1. Move the unit with a `switch` (without the pattern). ✅
2. Introduce the `Command` interface and `MoveCommand`. ✅
3. `InputHandler`: key bindings as data. ✅
4. Undo with a command history. ✅
5. Redo with a second stack. ✅

Possible next step: **replay**. Reset the unit to its start position and re-run every command in `history`, drawing each step with a short pause. Many games implement replays this way: they store the commands, not the positions.

## Advantages
- Reduces a lot the size of a decision structure. If I wanted to add a functionality to invert the inputs, I would had to:
    1. Read the input.
    2. Store the result (probably via a Switch entry) into a flag (bool)
    3. In every Switch entry, check if the bool is true or false to decide what the input is going to do.

## C++ Concepts

### Headers

Include only the headers for what you use. Nothing is included automatically.

| Header | Used for |
|---|---|
| `<iostream>` | `std::cout` |
| `<algorithm>` | `std::clamp` |
| `<memory>` | `std::unique_ptr`, `std::make_unique` |
| `<unordered_map>` | `std::unordered_map` |
| `<vector>` | `std::vector` |
| `<conio.h>` | `_getch()`. Windows only, not standard C++. |

Avoid `#include <bits/stdc++.h>` (GCC only, slow to compile) and `using namespace std;` (it pulls every `std` name into your code and can cause name clashes).

### `main`

`main` must return `int`. The value is the program's exit code: `0` means success. `void main()` isn't valid C++. If execution reaches the end of `main` without a `return`, it returns `0` automatically.

### Initialization

A variable of a simple type that isn't initialized holds **garbage**: whatever was in memory before. Reading it is undefined behavior. Always initialize when you declare:

```cpp
int a = 0;          // ok
int x, y = 0;       // ❌ only y is initialized
int x = 0, y = 0;   // ✅ both are
```

Ways to initialize a struct:

```cpp
struct Unit {
    int x = 0;      // default member initializers
    int y = 0;
};

Unit a;                     // x = 0, y = 0
Unit b{2, 3};               // brace (aggregate) initialization
Unit c{.x = 2, .y = 3};     // designated initializers (C++20)
```

Braces also catch narrowing mistakes: `int i{3.7};` is a compile error, while `int i = 3.7;` silently becomes `3`.

A constructor initializes members with an **initializer list**:

```cpp
MoveCommand(int dx, int dy) : dx_(dx), dy_(dy) {}
```

The trailing `_` is a common naming convention for member variables.

`constexpr int kWidth = 10;` declares a constant known at compile time.

### Loops

```cpp
for (int i = 0; i < 10; ++i) { }        // classic: init; condition; step
for (const auto& item : container) { }  // range-based: every element, without copying
```

Start at `0` and use `<`: an array or grid of 10 has indexes `0` to `9`. That's also why the clamp uses `kWidth - 1`, the last valid index.

### `switch`

```cpp
switch (c) {
case 'w':
case 'W':           // grouped cases: falls through on purpose
    // ...
    break;          // without break, execution continues into the next case
default:
    break;
}
```

- It only works on integer-like values (`char`, `int`, `enum`), not `std::string`.
- Declaring a variable inside a `case` needs braces: `case 'w': { int n = 1; break; }`.
- `break` leaves the `switch`, not the surrounding loop.

### Strings and escape sequences

A backslash starts an escape sequence: `"\n"` is a line break, while `"/n"` is just the two characters `/` and `n`.

| Sequence | Meaning |
|---|---|
| `\n` | new line |
| `\t` | tab |
| `\\` | backslash |
| `\"` | double quote |
| `\x1b` | the Escape character |

`std::endl` is a new line that also flushes the output, which is slower. Prefer `'\n'`.

`"\x1b[2J\x1b[H"` is an ANSI escape code that clears the terminal and moves the cursor to the top-left corner. It's faster than `std::system("cls")`.

### Console input

| Method | Behavior |
|---|---|
| `std::cin >> c` | Standard. Waits for Enter. Skips whitespace. |
| `std::getline(std::cin, s)` | Standard. Reads a whole line, including spaces. |
| `_getch()` | Windows only (`<conio.h>`). Returns one key press immediately, without Enter, and doesn't echo it. |
| `_kbhit()` | Windows only. `true` if a key is waiting, so the game doesn't have to stop and wait. |

`_getch()` returns an `int`, so convert it explicitly: `static_cast<char>(_getch())`. Real games get input from a library such as SDL, SFML, GLFW or raylib.

### `std::clamp`

```cpp
x = std::clamp(x, 0, kWidth - 1);
```

It **returns** the value limited to `[low, high]`, so assign the result back. All three arguments must be the same type.

### `class` vs `struct`

They're identical except for default access: `struct` members are **public** by default, `class` members are **private**. Common style: `struct` for plain data (`Unit`), `class` for types with behavior (`Command`). Both need a `;` after the closing `}`. If it's missing, the error points at the *next* line.

### Inheritance and polymorphism

```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual void execute(Unit& unit) = 0;
};

class MoveCommand : public Command {
public:
    void execute(Unit& unit) override { /* ... */ }
};
```

- **`: public Command`**: `MoveCommand` *is a* `Command`.
- **`virtual`**: calling `execute` through a `Command` pointer or reference runs the version of the actual object (`MoveCommand`). The choice is made at runtime. This is **polymorphism**.
- **`= 0`**: a *pure virtual* function, declared but with no implementation. A class with one is **abstract**: you can't create an object of it (`Command c;` gives error C2259). You can only have pointers or references to it, which point to a derived class.
- **`override`**: marks a function that replaces a virtual one from the base class. If the signature doesn't match, it's a compile error instead of a silent bug.
- **Virtual destructor**: objects are deleted through a `Command` pointer. Without `virtual ~Command()`, only the `Command` part would be destroyed. **Any class with virtual functions needs a virtual destructor.**
- A derived class must implement *every* pure virtual function, or it's abstract too.

### Pointers and references

```cpp
Unit& u       // reference: an alias for an existing object; can't be null
const Unit& u // read-only reference: no copy, no changes
Command* p    // raw pointer: can be nullptr; doesn't own anything
```

- Pass by `const&` to read without copying, and by `&` to modify.
- `a->b` is short for `(*a).b`: follow the pointer, then access the member.
- `nullptr` means "points to nothing". Check before using: `if (p) { p->execute(unit); }`.
- `this` is a pointer to the current object, and `*this` is the object itself.

### `std::unique_ptr` and ownership

```cpp
std::unique_ptr<Command> cmd = std::make_unique<MoveCommand>(0, -1);
cmd->execute(unit);   // use like a pointer
Command* raw = cmd.get();  // the raw pointer inside, without giving up ownership
```

- A `unique_ptr` **owns** its object and deletes it automatically when it goes out of scope. You never write `new` or `delete`.
- Only one `unique_ptr` can own an object, so it **can't be copied**. A copy would mean two owners and a double delete. The compiler error mentions a "deleted function".
- A `unique_ptr<MoveCommand>` converts automatically to a `unique_ptr<Command>`.
- **Rule of thumb:** `unique_ptr` means "I own this". A raw pointer or reference means "I'm only using it". Never `delete` a raw pointer obtained from `.get()`.

### `std::move`

`std::move` **transfers ownership**. By itself it moves nothing: it marks a variable as "I'm done with this, you may take what's inside". The receiver does the taking:

```cpp
bindings_[key] = std::move(command);     // the map now owns it
history.push_back(std::move(command));   // the vector now owns it
```

- After `std::move(x)`, don't use `x` again (except to assign it a new value). A moved-from `unique_ptr` is `nullptr`, and using it crashes.
- So use it first, then move it: `command->execute(unit);` and only then `history.push_back(std::move(command));`.
- Moving out of a container element empties the element but **doesn't remove it**. After `redoStack.push_back(std::move(history.back()));` the vector still has the same size, with a `nullptr` at the end. Calling `history.back()->undo(unit)` again would crash. Remove the empty slot with `pop_back()`, and do it **after** the move: popping first would destroy the command along with its `unique_ptr`.
  ```cpp
  redoStack.push_back(std::move(history.back()));
  history.pop_back();   // removes the now-empty slot; deletes nothing
  ```
- Temporaries need no `std::move`: in `bind('w', std::make_unique<MoveCommand>(0, -1))` the value has no name, so the compiler moves it automatically. Only **named variables** need it.

### Copy constructor and `clone()`

C++ automatically generates a **copy constructor** that copies every member. `clone()` uses it to make an independent copy through the base class:

```cpp
std::unique_ptr<Command> clone() const override {
    return std::make_unique<MoveCommand>(*this);
}
```

The `const` after the parameter list promises that the function doesn't change the object it's called on.

### `std::unordered_map`

A key → value table with fast lookup.

```cpp
std::unordered_map<char, std::unique_ptr<Command>> bindings_;

bindings_['w'] = std::move(cmd);       // insert or overwrite

auto it = bindings_.find('w');         // look up without inserting
if (it != bindings_.end()) {           // end() means "not found"
    it->first;                          // the key
    it->second;                         // the value (a unique_ptr)
    it->second->clone();                // call a function on the Command it points to
}
```

- Use `find` to look things up. `operator[]` **inserts** an empty entry if the key is missing.
- `find` returns an **iterator**: a cursor pointing at one entry, which is a pair of `first` (key) and `second` (value).
- `.` calls a function on the object itself (`it->second.get()` is a `unique_ptr` function). `->` reaches through to what it points at (`it->second->clone()` is a `Command` function).
- `auto` lets the compiler work out the type, which is useful for long iterator types.

### `std::vector` as a stack

```cpp
std::vector<std::unique_ptr<Command>> history;

history.push_back(std::move(cmd));   // add to the end
history.back()->undo(unit);          // the last element
history.pop_back();                  // remove the last element (the unique_ptr deletes it)
history.empty();                     // true if there are no elements
redoStack.clear();                   // remove all elements (each unique_ptr deletes its command)
```

### Undefined behavior

Some operations **don't check anything**. Calling them when they're invalid is *undefined behavior*: the program may crash, show garbage, or seem to work and fail later.

- reading an uninitialized variable
- `back()`, `front()` or `pop_back()` on an empty container. Check `!v.empty()` first. (`if (v.back())` doesn't help: the call itself is already invalid.)
- `v[i]` with an out-of-range `i` (`v.at(i)` checks and throws an exception instead)
- dereferencing `nullptr`, including a moved-from `unique_ptr`

In MSVC Debug builds, many of these trigger an assertion popup instead of silent corruption.

### Compiler warnings and errors

- Compile with warnings on (`/W4`) and treat them as bugs. C4700 (uninitialized variable) and C4326 (`void main`) both pointed at real problems here.
- An error that makes no sense on its line is often caused by the line **above** it, typically a missing `;`.
- C++ reads a file top to bottom: a type or constant must be declared before it's used (`Unit` before `Command`, `kWidth` before `MoveCommand`).

## Building

Requires Visual Studio with the C++ tools. From a terminal in this folder:

```
.\build.bat        compile only
.\build.bat run    compile and run
```

`build.bat` sets up the MSVC environment and runs `cl /EHsc /std:c++20 /W4 main.cpp`.
