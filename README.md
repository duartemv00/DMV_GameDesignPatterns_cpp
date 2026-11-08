# Game Design Patterns in C++
A compendium of small, self-contained applications of game programming patterns. Each folder is one pattern with its own code and README.

| Pattern | What it shows |
|---------|---------------|
| [Bytecode](BytecodePattern/README.md) | A stack-based virtual machine that runs game behaviour stored as data |
| [Command](CommandPattern/README.md) | Player actions as objects, with rebindable input and undo/redo |

## Build and run
You only need Visual Studio with the "Desktop development with C++" workload. There are no solution or project files. Each pattern folder has its own `build.bat`, which creates `main.exe` in that folder:

```
.\build.bat        compile only
.\build.bat run    compile and run
```

In VS Code, use the matching **Run** configuration from the Run and Debug panel to build and debug a pattern.

## Adding a pattern
1. Create `<Name>Pattern/` with a `main.cpp` and a `build.bat` copied from another pattern. Adjust the source files on its `cl` line.
2. Add a `README.md` in the folder and a row in the table above.
3. For VS Code, copy one entry in `.vscode/tasks.json` and one in `.vscode/launch.json`.
