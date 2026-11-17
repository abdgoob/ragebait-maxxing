#include "stubs/Level.h"
#include "developer/DeveloperDirector.h"
#include "patches/PatchManager.h"
#include "stubs/Player.h"

using namespace std;

Level::Level() : Level("test chunk") {}

Level::Level(const string& name) : name_(name), trapCount_(0), baseGravity_(900.f) {
    ground_.setSize(sf::Vector2f(900.f, 40.f));
    ground_.setPosition(0.f, 480.f);
    ground_.setFillColor(sf::Color(60, 60, 70));
    for (int i = 0; i < MAX_TRAPS; ++i) {
        traps_[i].reset();
    }
}

void Level::addTrap(unique_ptr<Trap> trap) {
    if (!trap || trapCount_ >= MAX_TRAPS) return;
    traps_[trapCount_] = move(trap);
    ++trapCount_;
}

void Level::setBaseGravity(float g) { baseGravity_ = g; }
float Level::getBaseGravity() const { return baseGravity_; }

void Level::update(float dt, Player& player, DeveloperDirector& dev, PatchManager& patches) {
    patches.update(dt, player, *this);

    for (int i = 0; i < trapCount_; ++i) {
        if (traps_[i]) traps_[i]->update(dt);
    }

    if (player.isAlive()) {
        player.applyGravity(baseGravity_, dt);
        checkTraps(player, dev, patches);
    }
}

void Level::draw(sf::RenderTarget& target) const {
    target.draw(ground_);
    for (int i = 0; i < trapCount_; ++i) {
        if (traps_[i]) traps_[i]->draw(target);
    }
}

void Level::checkTraps(Player& player, DeveloperDirector& dev, PatchManager& patches) {
    for (int i = 0; i < trapCount_; ++i) {
        Trap* trap = traps_[i].get();
        if (!trap || !trap->isActive()) continue;

        const sf::FloatRect pb = player.getBounds();
        const sf::FloatRect tb = trap->getBounds();

        if (!pb.intersects(tb)) continue;

        trap->onPlayerTouch(player);

        if (trap->hurtsPlayer(player)) {
            player.kill();
            dev.onPlayerDeath(player, trap);
            dev.maybeDropPatch(player, *this, patches);
            patches.applyQueued(player, *this);
            return;
        }
    }
}

void Level::resetTraps() {
    for (int i = 0; i < trapCount_; ++i) {
        if (traps_[i]) traps_[i]->reset();
    }
}

const string& Level::getName() const { return name_; }