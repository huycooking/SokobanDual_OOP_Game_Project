#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <SFML/Graphics.hpp>
#include <optional>

class GameObject : public sf::Drawable {
public:
    GameObject(const sf::Texture* tex = nullptr, float desiredSize = 0.f);
    virtual ~GameObject();

    // public API
    virtual void setSize(const sf::Vector2f& size);
    virtual void setPosition(const sf::Vector2f& pos);
    virtual void setFillColor(const sf::Color& color);

    // semantics: true => blocks movement (cannot be walked through)
    bool blocks() const;

    // pure virtuals keep class abstract
    virtual bool isPushable() const = 0;
    virtual bool isPortal() const = 0;

protected:
    // derived classes set this flag in their constructors
    bool blocksFlag = false;

    sf::RectangleShape shape;
    std::optional<sf::Sprite> sprite; // optional sprite when a texture is provided

    // drawing handled by derived classes through sf::Drawable
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // GAMEOBJECT_HPP
