#pragma once

class Warrior
{
public:
    Warrior(int startHealth, int startStrength) :
    x_(0), y_(0), health_(startHealth), strength_(startStrength) {}

    int getX() const { return x_; }
    int getY() const { return y_; }
    int getHealth() const { return health_; }
    int getStrength() const { return strength_; }

    void setPosition(int x, int y) { x_ = x; y_ = y; }
    void setHealth(int health) { health_ = health < 0 ? 0 : health; }

private:
    int x_;
    int y_;
    int health_;
    int strength_;
};
