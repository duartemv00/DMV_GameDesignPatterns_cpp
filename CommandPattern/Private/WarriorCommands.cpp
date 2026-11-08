#include "../Public/WarriorCommands.h"

void MoveCommand::execute()
{
    previousX_ = warrior_.getX();
    previousY_ = warrior_.getY();
    warrior_.setPosition(previousX_ + deltaX_, previousY_ + deltaY_);
}

void MoveCommand::undo()
{
    warrior_.setPosition(previousX_, previousY_);
}

void AttackCommand::execute()
{
    previousHealth_ = target_.getHealth();
    target_.setHealth(previousHealth_ - attacker_.getStrength());
}

void AttackCommand::undo()
{
    target_.setHealth(previousHealth_);
}
