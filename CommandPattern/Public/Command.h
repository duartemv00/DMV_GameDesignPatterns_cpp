#pragma once

/*
 * An action wrapped in an object, so it can be stored and reversed
 */
class Command
{
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual void undo() = 0;
};
