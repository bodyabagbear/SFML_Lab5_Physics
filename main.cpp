#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <cmath>
#include <string>
#include <iostream>
#include <optional>

int main()
{
    // Створюємо вікно для нашої лаби, розмір 800 на 600
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Lab 5 - Variant 17 - Physics");
    window.setFramerateLimit(60); // Щоб не лагало

    // Фізичні константи з методички
    float g = 9.81f;           // Гравітація
    float angleDeg = 30.0f;    // Початковий кут в градусах
    float mu = 0.3f;           // Тертя

    float s = 0.0f;            // Шлях, який проїхав кубик
    float v = 0.0f;            // Його швидкість
    float a = 0.0f;            // Прискорення

    sf::Clock clock; // Таймер для розрахунку часу між кадрами

    // Малюємо саму площину (біла лінія)
    sf::RectangleShape plane(sf::Vector2f({ 700.0f, 5.0f }));
    plane.setFillColor(sf::Color::White);
    plane.setPosition({ 50.0f, 200.0f });

    // Наш кубик (червоний)
    sf::RectangleShape box(sf::Vector2f({ 40.0f, 40.0f }));
    box.setFillColor(sf::Color::Red);
    box.setOrigin({ 0.0f, 40.0f }); // Центр прив'язки знизу, щоб рівно стояв на лінії

    // Завантажуємо стандартний шрифт вінди
    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Error loading font!\n";
    }

    // Текст для виводу розрахунків
    sf::Text infoText(font);
    infoText.setCharacterSize(20);
    infoText.setFillColor(sf::Color::Yellow);
    infoText.setPosition({ 20.0f, 450.0f });

    // Текст з інструкцією
    sf::Text controlsText(font);
    controlsText.setCharacterSize(18);
    controlsText.setFillColor(sf::Color::Cyan);
    controlsText.setPosition({ 450.0f, 450.0f });
    controlsText.setString(
        "Controls:\n"
        "UP / DOWN - change angle\n"
        "LEFT / RIGHT - change friction (mu)\n"
        "SPACE - restart simulation"
    );

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        // Перевіряємо, чи не натиснув юзер якісь кнопки
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    // Повертаємо кубик на старт
                    s = 0.0f;
                    v = 0.0f;
                }
                else if (keyPressed->code == sf::Keyboard::Key::Up) {
                    angleDeg += 2.0f; // Піднімаємо площину
                    if (angleDeg > 80.0f) angleDeg = 80.0f;
                    s = 0; v = 0;
                }
                else if (keyPressed->code == sf::Keyboard::Key::Down) {
                    angleDeg -= 2.0f; // Опускаємо площину
                    if (angleDeg < 0.0f) angleDeg = 0.0f;
                    s = 0; v = 0;
                }
                else if (keyPressed->code == sf::Keyboard::Key::Right) {
                    mu += 0.05f; // Збільшуємо тертя
                    if (mu > 1.0f) mu = 1.0f;
                    s = 0; v = 0;
                }
                else if (keyPressed->code == sf::Keyboard::Key::Left) {
                    mu -= 0.05f; // Зменшуємо тертя
                    if (mu < 0.0f) mu = 0.0f;
                    s = 0; v = 0;
                }
            }
        }

        //Блок з фізикою
        float angleRad = angleDeg * 3.14159265f / 180.0f; // Переводимо в радіани для sin/cos
        float forceGravity = std::sin(angleRad);
        float forceFriction = mu * std::cos(angleRad);

        // Якщо сила тяжіння більша за тертя, то їдемо
        if (forceGravity > forceFriction) {
            a = g * (forceGravity - forceFriction);
        }
        else {
            a = 0.0f;
            v = 0.0f;
        }

        // Рахуємо нову позицію кубика
        if (s < 650.0f) {
            v += a * dt;
            s += v * dt * 50.0f; // Множимо на 50 для нормального масштабу на екрані
        }
        else {
            v = 0.0f; // Зупиняємося в кінці лінії
        }

        //Блок малювання
        plane.setRotation(sf::degrees(angleDeg));
        box.setRotation(sf::degrees(angleDeg));

        float startX = plane.getPosition().x;
        float startY = plane.getPosition().y;

        // Рухаємо кубик вздовж похилої площини
        box.setPosition({ startX + s * std::cos(angleRad), startY + s * std::sin(angleRad) });

        // Оновлюємо текст з показниками
        std::string info = "Angle: " + std::to_string((int)angleDeg) + " deg\n" +
            "Friction (mu): " + std::to_string(mu).substr(0, 4) + "\n\n" +
            "Acceleration (a): " + std::to_string(a).substr(0, 4) + " m/s^2\n" +
            "Velocity (v): " + std::to_string(v).substr(0, 4) + " m/s\n";

        if (a == 0.0f) {
            info += "\nSTATE: At rest (friction > gravity)";
        }
        else {
            info += "\nSTATE: Sliding down";
        }
        infoText.setString(info);

        // Очищуємо екран і малюємо все по-новій
        window.clear(sf::Color({ 30, 30, 30 }));
        window.draw(plane);
        window.draw(box);
        window.draw(infoText);
        window.draw(controlsText);
        window.display();
    }

    return 0;
}