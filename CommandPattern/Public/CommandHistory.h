#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "Command.h"

/*
 * Runs commands and remembers them, so they can be undone and redone
 */
class CommandHistory
{
public:
    CommandHistory() : nextCommand_(0) {}

    void execute(std::unique_ptr<Command> command);
    bool undo();
    bool redo();

private:
    std::vector<std::unique_ptr<Command>> commands_;
    size_t nextCommand_; // Commands before this index are done, the ones from it onwards are undone
};
