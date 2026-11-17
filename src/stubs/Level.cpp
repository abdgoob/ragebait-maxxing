#include "stubs/Level.h"
#include "developer/DeveloperDirector.h"
#include "patches/PatchManager.h"
#include "stubs/Player.h"

Level::Level() : Level("test chunk") {}

Level::Level(const std::string& name) : name_(name), baseGravity_(900.f) {
    ground_.setSize(sf::Vector2f(900.f, 40.f));
    ground_.setPosition(0.f, 480.f);
    ground_.setFillColor(sf::Color(60, 60, 70));
}

void Level::addTrap(std::unique_ptr<Trap> trap) {
    if (trap) traps_.push_back(std::move(trap));
}

void Level::setBaseGravity(float g) { baseGravity_ = g; }
float Level::getBaseGravity() const { return baseGravity_; }

void Level::update(float dt, Player& player, DeveloperDirector& dev, PatchManager& patches) {
    patches.update(dt, player, *this);

    for (auto& trap : traps_) {
        if (trap) trap->update(dt);
    }

    if (player.isAlive()) {
        player.applyGravity(baseGravity_, dt);
        checkTraps(player, dev, patches);
    }
}

void Level::draw(sf::RenderTarget& target) const {
    target.draw(ground_);
    for (const auto& trap : traps_) {
        if (trap) trap->draw(target);
    }
}

void Level::checkTraps(Player& player, DeveloperDirector& dev, PatchManager& patches) {
    for (auto& trap : traps_) {
        if (!trap || !trap->isActive()) continue;

        const Trap* raw = trap.get();

        const sf::FloatRect pb = player.getBounds();
        const sf::FloatRect tb = trap->getBounds();

        if (!pb.intersects(tb)) continue;

        trap->onPlayerTouch(player);

        if (trap->hurtsPlayer(player)) {
            player.kill();
            dev.onPlayerDeath(player, raw);
            dev.maybeDropPatch(player, *this, patches);
            patches.applyQueued(player, *this);
            return;
        }
    }
}

void Level::resetTraps() {
    for (auto& trap : traps_) {
        if (trap) trap->reset();
    }
}

const std::string& Level::getName() const { return name_; }