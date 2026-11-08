
#include <cstdio>
#include <memory>
#include <utility>
#include "Public/CommandHistory.h"
#include "Public/WarriorCommands.h"

/*
 * Using the Command Pattern, every player action becomes an object.
 * The input code only decides which command a key means, and CommandHistory
 * runs it without knowing what it does, which is what makes undo and redo possible.
 */

// Rebinding a control only means changing this function
std::unique_ptr<Command> commandForKey(char key, Warrior& player, Warrior& enemy)
{
    switch (key)
    {
        case 'w': return std::make_unique<MoveCommand>(player, 0, 1);
        case 'a': return std::make_unique<MoveCommand>(player, -1, 0);
        case 's': return std::make_unique<MoveCommand>(player, 0, -1);
        case 'd': return std::make_unique<MoveCommand>(player, 1, 0);
        case 'f': return std::make_unique<AttackCommand>(player, enemy);
        default: return nullptr;
    }
}

int main()
{
    Warrior player(100, 30);
    Warrior enemy(20, 5);
    CommandHistory history;

    // Stands in for real keyboard input: z is undo and y is redo
    const char* keys = "dwfzzyyzsy";

    for (const char* key = keys; *key != '\0'; key++)
    {
        if (*key == 'z')
        {
            history.undo();
        }
        else if (*key == 'y')
        {
            history.redo();
        }
        else if (std::unique_ptr<Command> command = commandForKey(*key, player, enemy))
        {
            history.execute(std::move(command));
        }

        std::printf("%c -> Player: (%d, %d) | Enemy health: %d\n",
            *key, player.getX(), player.getY(), enemy.getHealth());
    }

    return 0;
}
