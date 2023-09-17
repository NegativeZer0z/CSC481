#include "Player.h"

Player::Player(sf::Vector2f position, sf::Vector2f size) : Entity(position, size) {
    setPosition(position);
    setSize(size);
    setFillColor(sf::Color::Green);
    velocity.x = 0.f;
    velocity.y = 0.f;
    leftOffset = 0;
    botOffset = 0;
    startOffset = 0;
    jump = false;
}

void Player::update(float deltaTime) {
    //slow down horizontal movement
    velocity.x *= 0.5f;

    //move left
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        updateTexture(leftText.x, leftText.y);
        velocity.x += -PLAYER_SPEED * deltaTime;
    }

    //move right
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        updateTexture(rightText.x, rightText.y);
        velocity.x += PLAYER_SPEED * deltaTime;
    }

    //jumping
    if((sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && jump) {
        jump = false;
        if(velocity.y < MAX_VERTICAL_VELOCITY) {
            velocity.y += -JUMP_HEIGHT * deltaTime;
        }
    }

    velocity.y += GRAVITY * deltaTime; //apply gravity
    moveSprite(velocity);
}

void Player::initTexture(std::string path, int width, int height, sf::Vector2i left, sf::Vector2i right, int leftOffset, int botOffset, int startOffset) {
    if(!texture.loadFromFile(path)) {
        std::cout << "Can't load texture" << path << "\n";
    }

    textureWidth = width;
    textureHeight = height;
    leftText = left;
    rightText = right;
    this->leftOffset = leftOffset;
    this->botOffset = botOffset;
    this->startOffset = startOffset;

    sprite.setPosition(position);
    sprite.setTexture(texture);

    //get the specfic sprite for the player
    sf::Vector2u textSize = texture.getSize();
    textSize.x /= width;
    textSize.y /= height;
    sprite.setTextureRect(sf::IntRect(right.x + startOffset, textSize.y * right.y, textSize.x - leftOffset, textSize.y - botOffset));
}

void Player::updateTexture(int x, int y) {
    sf::Vector2u textSize = texture.getSize();
    textSize.x /= textureWidth;
    textSize.y /= textureHeight;
    sprite.setTextureRect(sf::IntRect(x + startOffset, textSize.y * y, textSize.x - leftOffset, textSize.y - botOffset));
}

bool Player::checkCollision(Entity& entity) {
    sf::FloatRect playerBounds = sprite.getGlobalBounds();
    sf::FloatRect entityBounds = entity.getGlobalBounds();
    
    if(entityBounds.intersects(playerBounds)) {

        //bot collision
        if(playerBounds.top < entityBounds.top 
            && playerBounds.top + playerBounds.height < entityBounds.top + entityBounds.height
            && playerBounds.left < entityBounds.left + entityBounds.width
            && playerBounds.left + playerBounds.width > entityBounds.left) 
        {
            velocity.y = 0.f;
            setSpritePosition(playerBounds.left, entityBounds.top - playerBounds.height);
            jump = true;
        }
        //top collision
        else if(playerBounds.top > entityBounds.top 
            && playerBounds.top + playerBounds.height > entityBounds.top + entityBounds.height
            && playerBounds.left < entityBounds.left + entityBounds.width
            && playerBounds.left + playerBounds.width > entityBounds.left) 
        {
            velocity.y = 0.f;
            setSpritePosition(playerBounds.left, entityBounds.top + entityBounds.height);
        }
        //right collision
        else if(playerBounds.left < entityBounds.left 
            && playerBounds.left + playerBounds.width < entityBounds.left + entityBounds.width
            && playerBounds.top < entityBounds.top + entityBounds.height
            && playerBounds.top + playerBounds.height > entityBounds.top) 
        {
            velocity.x = 0.f;
            setSpritePosition(entityBounds.left - playerBounds.width, playerBounds.top);
        }
        //left collision
        else if(playerBounds.left > entityBounds.left 
            && playerBounds.left + playerBounds.width > entityBounds.left + entityBounds.width
            && playerBounds.top < entityBounds.top + entityBounds.height
            && playerBounds.top + playerBounds.height > entityBounds.top) 
        {
            velocity.x = 0.f;
            setSpritePosition(entityBounds.left + entityBounds.width, playerBounds.top);
        }

        return true;
    }
    return false;
}

void Player::wallCollision() {
    if(sprite.getPosition().x < 0.f) { //left side
        setSpritePosition(0.f, sprite.getPosition().y);
    }
    if(sprite.getPosition().y < 0.f) { //bottom
        setSpritePosition(sprite.getPosition().x, 0.f);
    }
    if(sprite.getPosition().x + sprite.getGlobalBounds().width > WINDOW_WIDTH) { //right side
        setSpritePosition(WINDOW_WIDTH - sprite.getGlobalBounds().width, sprite.getPosition().y);
    }
    if(sprite.getPosition().y + sprite.getGlobalBounds().height > WINDOW_HEIGHT) { //top
        setSpritePosition(sprite.getPosition().x, WINDOW_HEIGHT - sprite.getGlobalBounds().height);
    }
}

