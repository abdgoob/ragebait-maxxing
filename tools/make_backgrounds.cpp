#include <SFML/Graphics.hpp>

// optional art tool, the game itself just loads the pngs
// plain rectangles and loops so we can explain where the scenery came from
void box(sf::RenderTexture& canvas, float x, float y, float w, float h, sf::Color color) {
    sf::RectangleShape shape(sf::Vector2f(w, h));
    shape.setPosition(x, y);
    shape.setFillColor(color);
    canvas.draw(shape);
}

int main() {
    sf::RenderTexture canvas;
    if (!canvas.create(960, 540)) return 1;
    for (int screen = 0; screen < 2; ++screen) {
        canvas.clear(sf::Color(13, 17, 32));
        // distant panels, deliberately dim so the actual traps stay readable
        for (int y = 0; y < 540; y += 40)
            box(canvas, 0, static_cast<float>(y), 960, 1, sf::Color(20, 26, 43));
        for (int x = 0; x < 960; x += 80)
            box(canvas, static_cast<float>(x), 0, 1, 540, sf::Color(20, 26, 43));
        for (int rack = 0; rack < 8; ++rack) {
            const float x = 28.f + rack * 120.f;
            const float y = 300.f + (rack % 3) * 26.f;
            box(canvas, x + 4, y + 4, 88, 184, sf::Color(8, 12, 24));
            box(canvas, x, y, 88, 180, sf::Color(28, 33, 53));
            box(canvas, x + 5, y + 5, 78, 170, sf::Color(17, 22, 38));
            for (int row = 0; row < 5; ++row) {
                box(canvas, x + 10, y + 12 + row * 28, 67, 21, sf::Color(35, 38, 59));
                box(canvas, x + 16, y + 18 + row * 28, 3, 4, sf::Color(56, 104, 117));
                box(canvas, x + 24, y + 18 + row * 28, 3, 4, sf::Color(110, 54, 107));
                box(canvas, x + 36, y + 18 + row * 28, 32, 2, sf::Color(17, 22, 38));
            }
        }
        // corner brackets frame the menu without putting bright art under labels
        if (screen == 1) {
            box(canvas, 120, 72, 720, 410, sf::Color(13, 17, 32, 235));
            for (int side = 0; side < 2; ++side) {
                const float x = side == 0 ? 120.f : 834.f;
                box(canvas, x, 72, 6, 50, sf::Color(75, 184, 193));
                box(canvas, x, 432, 6, 50, sf::Color(164, 74, 157));
                box(canvas, side == 0 ? 120.f : 790.f, 72, 50, 6, sf::Color(75, 184, 193));
                box(canvas, side == 0 ? 120.f : 790.f, 476, 50, 6, sf::Color(164, 74, 157));
            }
        }
        canvas.display();
        const char* path = screen == 0 ? "assets/images/background.png" : "assets/images/menu.png";
        if (!canvas.getTexture().copyToImage().saveToFile(path)) return 1;
    }
    return 0;
}
