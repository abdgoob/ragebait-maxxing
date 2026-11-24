#pragma once

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

enum class Art { Player, Spike, Floor, Control, Checkpoint, Exit, Background, Menu };

// abdullah: these textures live as long as game, sprites only borrow them
class AssetManager {
public:
    AssetManager();
    explicit AssetManager(const string& folder);
    bool load(const string& folder);
    const sf::Texture* getTexture(Art art) const;
    sf::IntRect getRect(Art art) const;
    bool draw(sf::RenderTarget& target, Art art, const sf::FloatRect& box,
              const sf::Color& tint = sf::Color::White) const;

private:
    sf::Texture textures_[3];
    bool loaded_[3];
    int textureIndex(Art art) const;
    AssetManager(const AssetManager& other);
    AssetManager& operator=(const AssetManager& other);
};
