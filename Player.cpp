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
    dead = false;
    leftBound = 512.f;
    rightBound = 1024.f;
}

void Player::update(float deltaTime, std::string input) {
    //slow down horizontal movement
    //velocity.x *= 0.5f;

    //move left
    if(input == "left") {
        updateTexture(leftText.x, leftText.y);
        velocity.x += -PLAYER_SPEED * deltaTime;
    }

    //move right
    if(input == "right") {
        updateTexture(rightText.x, rightText.y);
        velocity.x += PLAYER_SPEED * deltaTime;
    }

    //jumping
    if(input == "jump" && jump) {
        jump = false;
        if(velocity.y < MAX_VERTICAL_VELOCITY) {
            velocity.y += -JUMP_HEIGHT * deltaTime;
        }
    }

    //velocity.y += GRAVITY * deltaTime; //apply gravity
    // if(deltaTime == 0) {
    //     velocity.x = 0.0f;
    //     velocity.y = 0.0f;
    // }
    moveSprite(velocity);
}

void Player::applyGravity(float deltaTime) {
    velocity.x *= 0.5f;
    velocity.y += GRAVITY * deltaTime; //apply gravity
    if(deltaTime == 0) {
        velocity.x = 0.0f;
        velocity.y = 0.0f;
    }
    moveSprite(velocity);
}

std::string Player::updateEvent() {
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        std::cout << "cord event" << std::endl;
    }

    //move left
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        return "left";
    }

    //move right
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        return "right";
    }

    //jumping
    if((sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))) {
        return "jump";
    }

    return "noEvent";
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

std::string Player::checkCollision(Entity& entity) {
    sf::FloatRect playerBounds = sprite.getGlobalBounds();
    sf::FloatRect entityBounds = entity.getGlobalBounds();

    if(entityBounds.intersects(playerBounds)) {

        //bot collision
        if(playerBounds.top < entityBounds.top 
            && playerBounds.top + playerBounds.height < entityBounds.top + entityBounds.height
            && playerBounds.left < entityBounds.left + entityBounds.width
            && playerBounds.left + playerBounds.width > entityBounds.left) 
        {
            //velocity.y = 0.f;
            //setSpritePosition(playerBounds.left, entityBounds.top - playerBounds.height);
            jump = true;
            return "botCollision";
        }
        //top collision
        else if(playerBounds.top > entityBounds.top 
            && playerBounds.top + playerBounds.height > entityBounds.top + entityBounds.height
            && playerBounds.left < entityBounds.left + entityBounds.width
            && playerBounds.left + playerBounds.width > entityBounds.left) 
        {
            //velocity.y = 0.f;
            //setSpritePosition(playerBounds.left, entityBounds.top + entityBounds.height);
            return "topCollision";
        }
        //right collision
        else if(playerBounds.left < entityBounds.left 
            && playerBounds.left + playerBounds.width < entityBounds.left + entityBounds.width
            && playerBounds.top < entityBounds.top + entityBounds.height
            && playerBounds.top + playerBounds.height > entityBounds.top) 
        {
            //velocity.x = 0.f;
            //setSpritePosition(entityBounds.left - playerBounds.width, playerBounds.top);
            return "rightCollision";
        }
        //left collision
        else if(playerBounds.left > entityBounds.left 
            && playerBounds.left + playerBounds.width > entityBounds.left + entityBounds.width
            && playerBounds.top < entityBounds.top + entityBounds.height
            && playerBounds.top + playerBounds.height > entityBounds.top) 
        {
            //velocity.x = 0.f;
            //setSpritePosition(entityBounds.left + entityBounds.width, playerBounds.top);
            return "leftCollision";
        }
    }
    return "noCollision";
}

void Player::wallCollision(sf::RenderWindow& window, sf::View& view) {
    if(sprite.getPosition().x < 0.f) { //hit left wall
        setSpritePosition(0.f, sprite.getPosition().y);
    }

    if(sprite.getPosition().x + sprite.getGlobalBounds().width > 1536.f) { //hit right wall
        setSpritePosition(1536.f - sprite.getGlobalBounds().width, sprite.getPosition().y);
    }

    if(sprite.getPosition().y < 0.f) { //bottom
        setSpritePosition(sprite.getPosition().x, 0.f);
    }

    // if(sprite.getPosition().x < leftBound) {
    //     view.setCenter(leftBound, window.getSize().y / 2);
    //     window.setView(view);
    //     return;
    // }

    // if(sprite.getPosition().x > rightBound) {
    //     view.setCenter(rightBound, window.getSize().y / 2);
    //     window.setView(view);
    //     return;
    // }

    // //center the view onto the players position
    // view.setCenter(sprite.getPosition().x, window.getSize().y / 2);
    // window.setView(view);


    // if(sprite.getPosition().y + sprite.getGlobalBounds().height > WINDOW_HEIGHT) { //top
    //     setSpritePosition(sprite.getPosition().x, WINDOW_HEIGHT - sprite.getGlobalBounds().height);
    // }
}

void Player::setState(bool state) {
    dead = state;
}

bool Player::checkState() {
    //set velocity to zero when dead before respawn
    if(dead) {
        setVelocity(0, 0);
    }
    return dead;
}

void Player::setVelocity(float x, float y) {
    velocity.x = x;
    velocity.y = y;
}

sf::Vector2f Player::getVelocity() {
    return velocity;
}



