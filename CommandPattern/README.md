# Command Pattern
Instead of calling game actions directly, wrap each action in an object. Once a request is an object, it can be stored, queued, replayed or reversed.

## How to
1. **Command**: An interface with `execute()` and `undo()`.
2. **Concrete Commands**: One class per action, holding everything needed to perform it (`MoveCommand`, `AttackCommand`).
3. **Invoker**: Runs commands without knowing what they do (`CommandHistory`).
4. **Receiver**: The object that is actually changed (`Warrior`).

## Advantages
- **Undo and Redo**: Each command knows how to reverse itself.
- **Decoupled Input**: Keys map to commands, so input code knows nothing about game rules and controls can be rebound.
- **Actions as Data**: A list of commands can be replayed, queued for later or sent over a network.

## Costs
- One small class per action.
- One heap allocation per action, unless commands are pooled.
- History grows with every action, so a real game needs a limit.

## Rules
1. A command captures the state it needs to undo inside `execute()`, not in its constructor, because the world can change between creating and running it.
2. Undo restores the captured state instead of applying the opposite action. An attack on a warrior with 20 health and 30 strength must give back 20, not 30.
3. Executing a new command discards the redo history, since those commands were recorded against a timeline that no longer exists.

## List of functionalities
| Key | Command         | Description                    |
|-----|-----------------|--------------------------------|
| `w` | `MoveCommand`   | Moves the player up            |
| `a` | `MoveCommand`   | Moves the player left          |
| `s` | `MoveCommand`   | Moves the player down          |
| `d` | `MoveCommand`   | Moves the player right         |
| `f` | `AttackCommand` | Player attacks the enemy       |
| `z` | —               | Undoes the last command        |
| `y` | —               | Redoes the last undone command |

## Use example
The demo plays the keys `dwfzzyyzsy`. The player starts at (0, 0) and the enemy has 20 health.

| Key | Player | Enemy health | What happens                                     |
|-----|--------|--------------|--------------------------------------------------|
| `d` | (1, 0) | 20           | Move right                                       |
| `w` | (1, 1) | 20           | Move up                                          |
| `f` | (1, 1) | 0            | Attack deals 20: health can't go below 0         |
| `z` | (1, 1) | 20           | Undo attack restores the captured health         |
| `z` | (1, 0) | 20           | Undo move up                                     |
| `y` | (1, 1) | 20           | Redo move up                                     |
| `y` | (1, 1) | 0            | Redo attack                                      |
| `z` | (1, 1) | 20           | Undo attack                                      |
| `s` | (1, 0) | 20           | New command: the undone attack can't be redone   |
| `y` | (1, 0) | 20           | Nothing to redo                                  |
