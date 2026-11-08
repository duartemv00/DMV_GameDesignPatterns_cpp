#pragma once

#include "Command.h"
#include "Warrior.h"

class MoveCommand : public Command
{
public:
    MoveCommand(Warrior& warrior, int deltaX, int deltaY) :
    warrior_(warrior), deltaX_(deltaX), deltaY_(deltaY), previousX_(0), previousY_(0) {}

    void execute() override;
    void undo() override;

private:
    Warrior& warrior_;
    int deltaX_;
    int deltaY_;
    int previousX_;
    int previousY_;
};

class AttackCommand : public Command
{
public:
    AttackCommand(const Warrior& attacker, Warrior& target) :
    attacker_(attacker), target_(target), previousHealth_(0) {}

    void execute() override;
    void undo() override;

private:
    const Warrior& attacker_;
    Warrior& target_;
    int previousHealth_;
};
