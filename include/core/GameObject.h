#pragma once

#include "core/Position.h"
#include "core/AssetManager.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

class Player;

class GameObject {
public:
    GameObject();
    GameObject(const Position& pos, const string& tag);
    virtual ~GameObject();

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;

    const Position& getPosition() const;
    void setPosition(const Position& pos);
    void setPosition(float x, float y);

    int getId() const;
    void setAssets(const AssetManager* assets);
    const string& getTag() const;

    static int getTotalObjects();

protected:
    const AssetManager* assets_ = 0;
    Position pos_;
    string tag_;
    int id_;

    static int nextId_;
    static int objectCount_;

private:
    GameObject(const GameObject& other);
    GameObject& operator=(const GameObject& other);
};
