#include "core/AssetManager.h"

AssetManager::AssetManager() {
    for (int i = 0; i < 3; ++i) loaded_[i] = false;
}

AssetManager::AssetManager(const string& folder) : AssetManager() { load(folder); }

bool AssetManager::load(const string& folder) {
    const char* files[3] = { "sprites.png", "background.png", "menu.png" };
    bool allLoaded = true;
    for (int i = 0; i < 3; ++i) {
        loaded_[i] = textures_[i].loadFromFile(folder + "images/" + files[i]);
        textures_[i].setSmooth(false);
        if (!loaded_[i]) allLoaded = false;
    }
    return allLoaded;
}

int AssetManager::textureIndex(Art art) const {
    if (art == Art::Background) return 1;
    if (art == Art::Menu) return 2;
    return 0;
}

const sf::Texture* AssetManager::getTexture(Art art) const {
    const int index = textureIndex(art);
    return loaded_[index] ? &textures_[index] : 0;
}

sf::IntRect AssetManager::getRect(Art art) const {
    const sf::Texture* texture = getTexture(art);
    if (!texture) return sf::IntRect();
    const int width = static_cast<int>(texture->getSize().x);
    const int height = static_cast<int>(texture->getSize().y);
    if (art == Art::Background || art == Art::Menu) return sf::IntRect(0, 0, width, height);
    // crop each picture close to its outline so the art matches the collision box
    // coordinates are for the bundled 1536x1024 sheet, scaled if it is resized
    const sf::IntRect pictures[6] = {
        sf::IntRect(116, 132, 268, 290), sf::IntRect(558, 181, 421, 198),
        sf::IntRect(1050, 180, 455, 190), sf::IntRect(32, 695, 449, 203),
        sf::IntRect(649, 546, 276, 438), sf::IntRect(1082, 574, 393, 389)
    };
    const int cell = static_cast<int>(art);
    if (cell < 0 || cell >= 6) return sf::IntRect();
    const sf::IntRect& source = pictures[cell];
    return sf::IntRect(source.left * width / 1536, source.top * height / 1024,
                       source.width * width / 1536, source.height * height / 1024);
}

bool AssetManager::draw(sf::RenderTarget& target, Art art, const sf::FloatRect& box,
                        const sf::Color& tint) const {
    const sf::Texture* texture = getTexture(art);
    if (!texture) return false; // caller draws its old shape instead
    const sf::IntRect rect = getRect(art);
    if (rect.width <= 0 || rect.height <= 0) return false;
    sf::Sprite sprite(*texture, rect);
    sprite.setPosition(box.left, box.top);
    sprite.setScale(box.width / rect.width, box.height / rect.height);
    sprite.setColor(tint);
    target.draw(sprite);
    return true;
}
