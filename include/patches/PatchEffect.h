#pragma once

#include <string>

class Player;
class Level;

class PatchEffect {
public:
    virtual ~PatchEffect() = default;

    virtual void apply(Player& player, Level& level) = 0;
    virtual void revert(Player& player, Level& level) = 0;

    virtual std::string getVersion() const = 0;
    virtual std::string getAnnouncement() const = 0;
    virtual bool isTimed() const;
    virtual float getDuration() const;

    bool isActive() const;
    void markActive(bool on);

protected:
    bool active_ = false;
};