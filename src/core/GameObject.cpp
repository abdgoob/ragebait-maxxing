#include "core/GameObject.h"

int GameObject::nextId_ = 1;
int GameObject::objectCount_ = 0;

GameObject::GameObject() : pos_(0.f, 0.f), tag_("thing"), id_(nextId_++) {
    ++objectCount_;
}

GameObject::GameObject(const Position& pos, const std::string& tag)
    : pos_(pos), tag_(tag), id_(nextId_++) {
    ++objectCount_;
}

GameObject::~GameObject() {
    --objectCount_;
}

void GameObject::setPosition(const Position& pos) { pos_ = pos; }

void GameObject::setPosition(float x, float y) {
    pos_.setX(x);
    pos_.setY(y);
}

const Position& GameObject::getPosition() const { return pos_; }

int GameObject::getId() const { return id_; }

const std::string& GameObject::getTag() const { return tag_; }

int GameObject::getTotalObjects() { return objectCount_; }