#include "../Public/CommandHistory.h"

#include <utility>

void CommandHistory::execute(std::unique_ptr<Command> command)
{
    // A new action starts a new timeline, so whatever was undone can't be redone anymore
    commands_.resize(nextCommand_);

    command->execute();
    commands_.push_back(std::move(command));
    nextCommand_++;
}

bool CommandHistory::undo()
{
    if (nextCommand_ == 0) return false;

    commands_[--nextCommand_]->undo();
    return true;
}

bool CommandHistory::redo()
{
    if (nextCommand_ == commands_.size()) return false;

    commands_[nextCommand_++]->execute();
    return true;
}
