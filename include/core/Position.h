#pragma once

using namespace std;

// small 2d point for the game world, hussan might replace later idk

class Position {
public:
    Position();
    Position(float x, float y);

    float getX() const;
    float getY() const;
    void setX(float x);
    void setY(float y);

    Position operator+(const Position& other) const;
    Position& operator+=(const Position& other);
    Position operator*(float scale) const;
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;

private:
    float x_;
    float y_;
};