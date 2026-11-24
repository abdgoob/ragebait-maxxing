#include "stubs/Level.h"
#include "developer/DeveloperDirector.h"
#include "patches/PatchManager.h"
#include "stubs/Player.h"
#include "traps/TrapFactory.h"
#include "patches/ControlPatch.h"

using namespace std;

namespace {
// sfml only counts a hit when boxes overlap, just touching the top edge isnt enough
// so standing on a fake floor sinks the feet 1px in and the trap actually notices you
const float kStandSink = 1.f;

void sceneryBox(sf::RenderTarget& target, float x, float y, float w, float h, sf::Color color) {
    sf::RectangleShape shape(sf::Vector2f(w, h));
    shape.setPosition(x, y);
    shape.setFillColor(color);
    target.draw(shape);
}
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
    clearWorld();
    for (int i = 0; i < trapCount_; ++i) {
        delete traps_[i];
        traps_[i] = 0;
    }
    trapCount_ = 0;
}

void Level::addTrap(Trap* trap) {
    if (!trap) return;
    if (trapCount_ >= MAX_TRAPS) { delete trap; return; }
    trap->setAssets(assets_);
    traps_[trapCount_] = trap;
    ++trapCount_;
}

void Level::clearTraps() {
    clearWorld();
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
    // same visible depth as ground, otherwise a black strip gives the hidden lid away
    pit_.setSize(sf::Vector2f(right - left, 40.f));
    pit_.setPosition(left, 480.f);
}

void Level::setBaseGravity(float g) { baseGravity_ = g; }
float Level::getBaseGravity() const { return baseGravity_; }

void Level::update(float dt, Player& player, DeveloperDirector& dev, PatchManager& patches) {
    patches.update(dt, player, *this);
    if (plate_) {
        plate_->setPressed(plate_->touches(player) || (ghostOn_ && plate_->touches(ghostBounds_)));
        plate_->update(dt);
    }
    if (door_) door_->update(dt);
    const sf::FloatRect oldBounds = player.getBounds();
    for (int i = 0; i < platformCount_; ++i) {
        const sf::FloatRect before = platforms_[i]->getBounds();
        const bool riding = player.isOnGround() && oldBounds.left + oldBounds.width > before.left
            && oldBounds.left < before.left + before.width
            && oldBounds.top + oldBounds.height >= before.top - 1.f
            && oldBounds.top + oldBounds.height <= before.top + 1.f;
        platforms_[i]->update(dt);
        if (riding) player.setPosition(player.getPosition() + platforms_[i]->getLastMove());
    }

    for (int i = 0; i < trapCount_; ++i) {
        if (traps_[i]) traps_[i]->update(dt);
    }

    if (!player.isAlive()) return;

    // move first so traps check where the player is now
    player.applyGravity(baseGravity_, dt);
    player.update(dt);
    if (door_ && door_->isSolid() && player.getBounds().intersects(door_->getBounds())) {
        const sf::FloatRect wall = door_->getBounds();
        if (oldBounds.left + oldBounds.width <= wall.left)
            player.setPosition(wall.left - oldBounds.width, player.getPosition().getY());
        else if (oldBounds.left >= wall.left + wall.width)
            player.setPosition(wall.left + wall.width, player.getPosition().getY());
        else player.setPosition(oldBounds.left, player.getPosition().getY());
    }
    for (int i = 0; i < platformCount_; ++i)
        if (platforms_[i]->isSolid()) landOnSurface(player, oldBounds, platforms_[i]->getBounds());
    if (popupOn_) landOnSurface(player, oldBounds, popupBounds_);
    keepOnGround(player);

    if (!player.isAlive()) {
        dev.onPlayerDeath(player, 0);
        dev.maybeDropPatch(player, *this, patches);
        patches.applyQueued(player, *this);
        return;
    }

    checkTraps(player, dev, patches);
    if (!player.isAlive()) return;
    if (chapter_ == 6) {
        boss_.update(dt, player);
        if (!player.isAlive()) {
            dev.onPlayerDeath(player, 0);
            dev.maybeDropPatch(player, *this, patches);
            return;
        }
        if (boss_.takeFakeWin()) {
            event_ = LevelEvent::BossFakeWin;
            glitchTimer_ = 3.f;
        }
        else if (boss_.isDefeated()) event_ = LevelEvent::BossWin;
        return;
    }
    if (betrayed_) betrayalTimer_ += dt;
    if (glitchTimer_ > 0.f) glitchTimer_ -= dt;
    if (checkpoint_) checkpoint_->update(dt);
    for (int i = 0; i < 2; ++i) if (exits_[i]) exits_[i]->update(dt);
    checkWorld(player, dev, patches);
}

void Level::draw(sf::RenderTarget& target) const {
    drawBackground(target);
    target.draw(ground_);
    if (pitRight_ > pitLeft_) target.draw(pit_);
    if (chapter_ == 1 && betrayed_ && betrayalTimer_ >= 0.45f) {
        sf::RectangleShape hole(sf::Vector2f(60.f, 56.f));
        hole.setPosition(810.f, 480.f);
        hole.setFillColor(sf::Color(12, 10, 16));
        target.draw(hole);
    }
    if (checkpoint_) checkpoint_->draw(target);
    for (int i = 0; i < platformCount_; ++i) platforms_[i]->draw(target);
    if (plate_) plate_->draw(target);
    if (door_) door_->draw(target);
    if (chapter_ == 6) boss_.draw(target);
    for (int i = 0; i < 2; ++i) if (exits_[i]) exits_[i]->draw(target);
    for (int i = 0; i < trapCount_; ++i) {
        if (traps_[i]) traps_[i]->draw(target);
    }
    // arsalan: tiny glitch bars, no random collision changes hidden in drawing
    if (glitchTimer_ > 0.f) {
        for (int i = 0; i < 4; ++i) {
            sf::RectangleShape line(sf::Vector2f(150.f, 3.f));
            line.setPosition(70.f + i * 210.f, 275.f + i * 28.f + glitchTimer_ * 8.f);
            line.setFillColor(sf::Color(220, 70, 200, 120));
            target.draw(line);
        }
    }
}

void Level::drawBackground(sf::RenderTarget& target) const {
    // same pixel landscape family as the menu, different mood for each chapter
    const sf::Color sky[6] = {sf::Color(12, 25, 43), sf::Color(35, 22, 39), sf::Color(21, 18, 44),
        sf::Color(14, 30, 35), sf::Color(29, 24, 39), sf::Color(33, 15, 30)};
    const sf::Color far[6] = {sf::Color(22, 57, 73), sf::Color(77, 43, 62), sf::Color(46, 38, 85),
        sf::Color(24, 67, 64), sf::Color(66, 54, 77), sf::Color(77, 30, 58)};
    const sf::Color near[6] = {sf::Color(18, 42, 58), sf::Color(52, 31, 48), sf::Color(31, 28, 62),
        sf::Color(17, 47, 47), sf::Color(44, 37, 57), sf::Color(49, 22, 40)};
    const sf::Color light[6] = {sf::Color(107, 201, 194), sf::Color(225, 146, 128), sf::Color(158, 134, 220),
        sf::Color(113, 217, 173), sf::Color(224, 188, 130), sf::Color(239, 106, 142)};
    const int n = chapter_ >= 1 && chapter_ <= 6 ? chapter_ - 1 : 0;
    sceneryBox(target, 0, 0, 960, 540, sky[n]);
    for (int i = 0; i < 32; ++i) {
        const float x = static_cast<float>((i * 137 + 33 + n * 79) % 944);
        const float y = static_cast<float>(90 + (i * 67 + n * 31) % 245);
        sceneryBox(target, x, y, i % 4 == 0 ? 4.f : 2.f, 2, light[n]);
    }
    const float moonX[6] = {764, 148, 690, 130, 755, 715};
    const float moonY[6] = {187, 213, 175, 200, 220, 177};
    sceneryBox(target, moonX[n], moonY[n], 64, 48, light[n]);
    sceneryBox(target, moonX[n] - 8, moonY[n] + 8, 80, 32, light[n]);
    sceneryBox(target, moonX[n] + 34, moonY[n] + 18, 16, 12, far[n]);
    if (n == 2 || n == 5) {
        // broken moon and static trails sell the unstable chapter, just decoration
        sceneryBox(target, moonX[n] - 12, moonY[n] + 14, 98, 5, sky[n]);
        sceneryBox(target, moonX[n] + 54, moonY[n] + 37, 40, 4, light[n]);
    }
    for (int layer = 0; layer < 2; ++layer) {
        for (int i = 0; i < 7; ++i) {
            const float x = i * 175.f - 120.f + n * 27.f;
            const float peak = 235.f + ((i * 43 + n * 51) % 110) + layer * 65.f;
            sf::ConvexShape mountain;
            mountain.setPointCount(3);
            mountain.setPoint(0, sf::Vector2f(x, 480));
            mountain.setPoint(1, sf::Vector2f(x + 110, peak));
            mountain.setPoint(2, sf::Vector2f(x + 240, 480));
            mountain.setFillColor(layer == 0 ? far[n] : near[n]);
            target.draw(mountain);
        }
    }
    const float ruins[6][3] = {{32, 450, 865}, {85, 375, 805}, {15, 290, 820},
        {70, 380, 810}, {25, 455, 875}, {45, 280, 820}};
    for (int i = 0; i < 3; ++i) {
        const float x = ruins[n][i];
        const float y = 330.f + ((i + n) % 3) * 29.f;
        sceneryBox(target, x, y, 76, 480 - y, sky[n]);
        sceneryBox(target, x + 6, y + 6, 64, 474 - y, far[n]);
        for (int row = 0; row < 3; ++row) {
            sceneryBox(target, x + 12, y + 14 + row * 27, 52, 19, near[n]);
            sceneryBox(target, x + 17, y + 20 + row * 27, 4, 4, light[n]);
        }
    }
    if (n == 0) {
        // quiet little antenna posts, the supposedly friendly chapter
        for (int i = 0; i < 3; ++i) {
            const float x = 200.f + i * 270.f;
            sceneryBox(target, x, 373, 4, 107, near[n]);
            sceneryBox(target, x - 12, 368, 28, 4, far[n]);
            sceneryBox(target, x, 364, 4, 4, light[n]);
        }
    } else if (n == 1) {
        // distant route signs point both ways, no collision or real directions here
        for (int i = 0; i < 2; ++i) {
            const float x = 255.f + i * 365.f;
            sceneryBox(target, x, 394, 4, 86, near[n]);
            sceneryBox(target, x - 25, 388, 54, 18, far[n]);
            sceneryBox(target, x - 25, 416, 54, 18, far[n]);
            sceneryBox(target, x + 23, 384, 6, 26, far[n]);
            sceneryBox(target, x - 25, 412, 6, 26, far[n]);
        }
    } else {
        for (int i = 0; i < 5; ++i) {
            const float x = 120.f + i * 163.f;
            sceneryBox(target, x, 290.f + (i % 3) * 29.f, 48, 3, far[n]);
            sceneryBox(target, x + 17, 299.f + (i % 3) * 29.f, 25, 2, light[n]);
        }
        // fractured signal tower in the distance
        sceneryBox(target, 552, 281, 10, 199, near[n]);
        sceneryBox(target, 533, 300, 48, 7, far[n]);
        sceneryBox(target, 543, 274, 27, 7, light[n]);
    }
    // ground below the play surface has the menu's little stone flecks
    sceneryBox(target, 0, 520, 960, 20, sf::Color(35, 28, 49));
    for (int i = 0; i < 40; ++i)
        sceneryBox(target, i * 24.f + 4, 526.f + (i % 2) * 8.f, 6, 4, near[n]);
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
    if (player.isOnGround()) return; // an elevated platform already caught this frame

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
    if (chapter_ == 1 && betrayed_ && betrayalTimer_ >= 0.45f && feetX > 810.f && feetX < 870.f) return;
    if (pitRight_ > pitLeft_ && feetX > pitLeft_ && feetX < pitRight_) return;

    // off the end of the ground means you fall and die
    if (pb.left + pb.width <= gb.left || pb.left >= gb.left + gb.width) return;
    if (pb.top + pb.height < gb.top) return;
    // already sunk past the edge, dont pop back up through the side
    if (pb.top + pb.height > gb.top + pb.height) return;

    player.landOn(gb.top);
}

void Level::resetTraps() {
    boss_.reset();
    ghostOn_ = false;
    popupOn_ = false;
    if (plate_) plate_->reset();
    if (door_) door_->reset();
    for (int i = 0; i < platformCount_; ++i) platforms_[i]->reset();
    for (int i = 0; i < trapCount_; ++i) {
        if (traps_[i]) traps_[i]->reset();
    }
    betrayed_ = false;
    betrayalTimer_ = 0.f;
    glitchTimer_ = 0.f;
    event_ = LevelEvent::None;
    if (finishSpike_) finishSpike_->setActive(false);
    for (int i = 0; i < 2; ++i) if (exits_[i]) exits_[i]->reset();
    if (exits_[1]) exits_[1]->setEnabled(false);
    // real checkpoint stays reached; fake one gets to lie again next attempt
    if (checkpoint_ && checkpoint_->isFake()) checkpoint_->reset();
}

void Level::clearWorld() {
    delete door_;
    door_ = 0;
    delete plate_;
    plate_ = 0;
    for (int i = 0; i < platformCount_; ++i) { delete platforms_[i]; platforms_[i] = 0; }
    platformCount_ = 0;
    ghostOn_ = false;
    popupOn_ = false;
    boss_.reset();
    delete checkpoint_;
    checkpoint_ = 0;
    for (int i = 0; i < 2; ++i) { delete exits_[i]; exits_[i] = 0; }
    finishSpike_ = 0;
    event_ = LevelEvent::None;
    betrayed_ = false;
    betrayalTimer_ = 0.f;
    glitchTimer_ = 0.f;
}

void Level::setupTrolls(int chapter) {
    clearWorld();
    chapter_ = chapter;
    if (chapter == 6) { setAssets(assets_); return; }
    if (chapter == 4) {
        plate_ = new PressureSwitch(Position(220, 472), 70);
        door_ = new Door(Position(650, 170), 28, 310, plate_);
        door_->setColor(sf::Color(54, 111, 109));
        addPlatform(new MovingPlatform(Position(360, 412), 80, 16, Position(50, 0), 45));
    }
    const float flagX = chapter == 1 ? 560.f : (chapter == 2 ? 590.f : 640.f);
    if (chapter <= 3) checkpoint_ = new Checkpoint(Position(flagX, 424.f), chapter == 2);
    exits_[0] = new Exit(Position(805.f, 416.f), 48.f, 64.f, true);
    exits_[1] = new Exit(Position(905.f, 416.f), 48.f, 64.f, false);
    exits_[1]->setEnabled(false);
    if (chapter == 2 && trapCount_ < MAX_TRAPS) {
        finishSpike_ = TrapFactory::makeSpike(Position(850.f, 456.f), 0.f, 0.f);
        addTrap(finishSpike_);
        finishSpike_->setActive(false);
    }
    setAssets(assets_);
}

void Level::setAssets(const AssetManager* assets) {
    assets_ = assets;
    for (int i = 0; i < trapCount_; ++i) if (traps_[i]) traps_[i]->setAssets(assets);
    if (checkpoint_) checkpoint_->setAssets(assets);
    for (int i = 0; i < 2; ++i) if (exits_[i]) exits_[i]->setAssets(assets);
}

void Level::checkWorld(Player& player, DeveloperDirector& dev, PatchManager& patches) {
    if (checkpoint_ && !checkpoint_->isReached() && checkpoint_->touches(player.getBounds())) {
        checkpoint_->markReached();
        if (checkpoint_->isFake()) {
            // fake progress never touches the real save or the actual spawn
            event_ = LevelEvent::FakeCheckpoint;
            dev.onFakeCheckpoint();
        } else {
            player.setSpawnPoint(Position(checkpoint_->getPosition().getX(), checkpoint_->getFloorY() - 36.f));
            event_ = LevelEvent::Checkpoint;
        }
    }
    if (exits_[0] && exits_[0]->isReachedBy(player)) {
        exits_[0]->use();
        betrayed_ = true;
        betrayalTimer_ = 0.f;
        event_ = LevelEvent::FakeExit;
        dev.onFakeExit(chapter_);
        if (chapter_ == 3) {
            glitchTimer_ = 3.f;
            patches.queuePatch(new ControlPatch());
            patches.applyQueued(player, *this);
        }
        return;
    }
    if (betrayed_ && betrayalTimer_ >= 0.45f) {
        if (exits_[1]) exits_[1]->setEnabled(true);
        if (finishSpike_) finishSpike_->setActive(true);
    }
    if (exits_[1] && exits_[1]->isReachedBy(player)) event_ = LevelEvent::RealExit;
}

void Level::setInteractionBounds(bool ghostOn, const sf::FloatRect& ghost, bool popupOn, const sf::FloatRect& popup) {
    ghostOn_ = ghostOn;
    ghostBounds_ = ghost;
    popupOn_ = popupOn;
    popupBounds_ = popup;
}

void Level::addPlatform(Platform* platform) {
    if (!platform) return;
    if (platformCount_ >= 8) { delete platform; return; }
    platforms_[platformCount_++] = platform;
}

void Level::landOnSurface(Player& player, const sf::FloatRect& oldBounds, const sf::FloatRect& surface) {
    const sf::FloatRect box = player.getBounds();
    if (player.getVelocity().getY() < 0.f) return;
    if (box.left + box.width <= surface.left || box.left >= surface.left + surface.width) return;
    if (oldBounds.top + oldBounds.height > surface.top + 1.f) return;
    if (box.top + box.height < surface.top) return;
    player.landOn(surface.top);
}

bool Level::isGhostDoorOpen() const { return door_ && door_->isOpen(); }
const DeveloperBoss& Level::getBoss() const { return boss_; }

LevelEvent Level::takeEvent() {
    const LevelEvent result = event_;
    event_ = LevelEvent::None;
    return result;
}

bool Level::hasBetrayed() const { return betrayed_; }

const string& Level::getName() const { return name_; }
