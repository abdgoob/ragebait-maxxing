#include "core/Position.h"

#include <cmath>

using namespace std;

Position::Position() : x_(0.f), y_(0.f) {}

Position::Position(float x, float y) : x_(x), y_(y) {}

float Position::getX() const { return x_; }
float Position::getY() const { return y_; }

void Position::setX(float x) { x_ = x; }
void Position::setY(float y) { y_ = y; }

Position Position::operator+(const Position& other) const {
    return Position(x_ + other.x_, y_ + other.y_);
}

Position& Position::operator+=(const Position& other) {
    x_ += other.x_;
    y_ += other.y_;
    return *this;
}

bool Position::operator==(const Position& other) const {
    const float eps = 0.001f;
    return fabs(x_ - other.x_) < eps && fabs(y_ - other.y_) < eps;
}

bool Position::operator!=(const Position& other) const {
    return !(*this == other);
}