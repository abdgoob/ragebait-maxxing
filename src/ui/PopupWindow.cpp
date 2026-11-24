#include "ui/PopupWindow.h"

using namespace std;

PopupWindow::PopupWindow() { reset(); }

void PopupWindow::reset() {
    position_ = sf::Vector2f(540.f, 220.f);
    dragOffset_ = sf::Vector2f();
    open_ = true;
    enabled_ = false;
    dragging_ = false;
    placed_ = false;
    closeDodges_ = 0;
}

void PopupWindow::setEnabled(bool enabled) {
    if (!enabled) {
        enabled_ = false;
        open_ = false;
        dragging_ = false;
        placed_ = false;
        return;
    }
    reset();
    enabled_ = true;
}

bool PopupWindow::isEnabled() const { return enabled_; }

sf::FloatRect PopupWindow::closeBounds() const {
    const float x = closeDodges_ % 2 == 0 ? 276.f : 12.f;
    return sf::FloatRect(position_.x + x, position_.y + 6.f, 30.f, 26.f);
}

bool PopupWindow::press(const sf::Vector2f& mouse) {
    if (!enabled_ || !open_) return false;
    if (closeBounds().contains(mouse)) {
        if (closeDodges_ < 3) ++closeDodges_;
        else open_ = false;
        return true;
    }
    if (sf::FloatRect(position_.x, position_.y, 320.f, 38.f).contains(mouse)) {
        dragging_ = true;
        placed_ = false;
        dragOffset_ = mouse - position_;
        return true;
    }
    return sf::FloatRect(position_.x, position_.y, 320.f, 146.f).contains(mouse);
}

void PopupWindow::move(const sf::Vector2f& mouse) {
    if (!enabled_ || !open_ || !dragging_) return;
    position_ = mouse - dragOffset_;
    if (position_.x < 0.f) position_.x = 0.f;
    if (position_.x > 640.f) position_.x = 640.f;
    if (position_.y < 170.f) position_.y = 170.f;
    if (position_.y > 420.f) position_.y = 420.f;
}

void PopupWindow::release() {
    if (dragging_) placed_ = true;
    dragging_ = false;
}

void PopupWindow::draw(sf::RenderTarget& target, const GameUI& ui) const {
    if (!enabled_ || !open_) return;
    // a placed window folds into its title bar, so a low bridge doesnt cover the hud
    ui.box(target, position_.x, position_.y, 320.f, placed_ ? 38.f : 146.f, sf::Color(225, 218, 238));
    ui.box(target, position_.x, position_.y, 320.f, 38.f, sf::Color(100, 65, 150));
    ui.text(target, "experience update", position_.x + 48.f, position_.y + 10.f, 14);
    const sf::FloatRect close = closeBounds();
    ui.box(target, close.left, close.top, close.width, close.height, sf::Color(190, 60, 95));
    ui.text(target, "x", close.left + 10.f, close.top + 2.f, 16);
    if (placed_) return;
    ui.text(target, "YOUR EXPERIENCE MATTERS", position_.x + 16.f, position_.y + 54.f,
            17, sf::Color(35, 25, 50));
    ui.text(target, closeDodges_ > 0 ? "close button working as intended" : "drag the title bar. trust us.",
            position_.x + 16.f, position_.y + 91.f, 14, sf::Color(65, 45, 85));
    ui.text(target, "drop it to make a platform", position_.x + 16.f, position_.y + 116.f,
            14, sf::Color(65, 45, 85));
}

bool PopupWindow::isOpen() const { return enabled_ && open_; }
bool PopupWindow::isPlatform() const { return enabled_ && open_ && placed_ && !dragging_; }
sf::FloatRect PopupWindow::getPlatformBounds() const {
    return sf::FloatRect(position_.x, position_.y, 320.f, 12.f);
}
