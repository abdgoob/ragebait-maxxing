#include "stubs/Level.h"
#include "developer/DeveloperDirector.h"
#include "patches/PatchManager.h"
#include "stubs/Player.h"

using namespace std;

namespace {
// sfml only counts a hit when boxes overlap, just touching the top edge isnt enough
// so standing on a fake floor sinks the feet 1px in and the trap actually notices you
const float kStandSink = 1.f;
}

Level::Level() : Level("test chunk") {}
Level::Level(const string& name) : name_(name), trapCount_(0), baseGravity_(900.f), pitLeft_(0.f), pitRight_(0.f) {
    ground_.setSize(sf::Vector2f(960.f, 40.f));
    ground_.setPosition(0.f, 480.f);
    ground_.setFillColor(sf::Color(60, 60, 70));
    pit_.setFillColor(sf::Color(12, 10, 16));
    pit_.setSize(sf::Vector2f(0.f, 40.f));
    pit_.setPosition(0.f, 480.f);
    for (int i = 0; i < MAX_TRAPS; ++i) {
        traps_[i] = 0;
    }
}

Level::~Level() {
    for (int i = 0; i < trapCount_; ++i) {
        delete traps_[i];
        traps_[i] = 0;
    }
    trapCount_ = 0;
}

void Level::addTrap(Trap* trap) {
    if (!trap || trapCount_ >= MAX_TRAPS) return;
    traps_[trapCount_] = trap;
    ++trapCount_;
}

void Level::clearTraps() {
    for (int i = 0; i < trapCount_; ++i) {
        delete traps_[i];
        traps_[i] = 0;
    }
    trapCount_ = 0;
}

void Level::setName(const string& name) { name_ = name; }

void Level::setPit(float left, float right) {
    pitLeft_ = left;
    pitRight_ = right;
    if (right <= left) {
        pit_.setSize(sf::Vector2f(0.f, 40.f));
        return;
    }
    pit_.setSize(sf::Vector2f(right - left, 56.f));
    pit_.setPosition(left, 480.f);
}

void Level::setBaseGravity(float g) { baseGravity_ = g; }
float Level::getBaseGravity() const { return baseGravity_; }

void Level::update(float dt, Player& player, DeveloperDirector& dev, PatchManager& patches) {
    patches.update(dt, player, *this);

    for (int i = 0; i < trapCount_; ++i) {
        if (traps_[i]) traps_[i]->update(dt);
    }

    if (!player.isAlive()) return;

    // move first so traps check where the player is now
    player.applyGravity(baseGravity_, dt);
    player.update(dt);
    keepOnGround(player);

    if (!player.isAlive()) {
        dev.onPlayerDeath(player, 0);
        dev.maybeDropPatch(player, *this, patches);
        patches.applyQueued(player, *this);
        return;
    }

    checkTraps(player, dev, patches);
}

void Level::draw(sf::RenderTarget& target) const {
    target.draw(ground_);
    if (pitRight_ > pitLeft_) target.draw(pit_);
    for (int i = 0; i < trapCount_; ++i) {
        if (traps_[i]) traps_[i]->draw(target);
    }
}

void Level::checkTraps(Player& player, DeveloperDirector& dev, PatchManager& patches) {
    for (int i = 0; i < trapCount_; ++i) {
        Trap* trap = traps_[i];
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

void Level::keepOnGround(Player& player) {
    if (!player.isAlive()) return;

    const sf::FloatRect pb = player.getBounds();
    const sf::FloatRect gb = ground_.getGlobalBounds();

    // stand on fake floors until they dump you
    for (int i = 0; i < trapCount_; ++i) {
        if (!traps_[i] || !traps_[i]->holdsPlayer()) continue;
        const sf::FloatRect tb = traps_[i]->getBounds();
        const bool over =
            pb.left + pb.width > tb.left && pb.left < tb.left + tb.width;
        if (!over) continue;
        if (pb.top + pb.height < tb.top - 4.f) continue;
        if (pb.top + pb.height > tb.top + 16.f) continue;
        player.landOn(tb.top + kStandSink);
        return;
    }

    // pit in the floor, fake platforms are supposed to cover this
    const float feetX = pb.left + pb.width * 0.5f;
    if (pitRight_ > pitLeft_ && feetX > pitLeft_ && feetX < pitRight_) return;

    // off the end of the ground means you fall and die
    if (pb.left + pb.width <= gb.left || pb.left >= gb.left + gb.width) return;
    if (pb.top + pb.height < gb.top) return;
    // already sunk past the edge, dont pop back up through the side
    if (pb.top + pb.height > gb.top + pb.height) return;

    player.landOn(gb.top);
}

void Level::resetTraps() {
    for (int i = 0; i < trapCount_; ++i) {
        if (traps_[i]) traps_[i]->reset();
    }
}

const string& Level::getName() const { return name_; }
