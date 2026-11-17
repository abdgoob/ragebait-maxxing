#pragma once

#include "core/Position.h"
#include <SFML/Graphics.hpp>
#include <string>

class Player;

class GameObject {
public:
    GameObject();
    GameObject(const Position& pos, const std::string& tag);
    virtual ~GameObject();

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;

    const Position& getPosition() const;
    void setPosition(const Position& pos);
    void setPosition(float x, float y);

    int getId() const;
    const std::string& getTag() const;

    static int getTotalObjects();

protected:
    Position pos_;
    std::string tag_;
    int id_;

    static int nextId_;
    static int objectCount_;
};