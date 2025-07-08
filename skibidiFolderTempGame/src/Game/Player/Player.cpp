#include "Player.hpp"

Player::Player() : velocityY(0.f), currentState(PlayerState::RUNNING), 
                   animationTimer(0.f), currentFrame(0), wasOnGround(true) {

    if (!texture.loadFromFile("Assets/player.png")) {
        throw std::runtime_error("Erreur: impossible de charger player.png");
    }

    float scale = 0.7f;
    
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(0, runningY, frameWidth, frameHeight));
    sprite.setScale(scale, scale);
    
    sprite.setPosition(100, 300);
    
    minY = 0.0f;
    
    float floorLevel = 450.0f;
    maxY = floorLevel - (frameHeight * scale);
}

void Player::setPosition(sf::Vector2f pos)
{
    pos.y = std::max(minY, std::min(pos.y, maxY));
    sprite.setPosition(pos);
}

sf::Vector2f Player::getPosition() const
{
    return sprite.getPosition();
}

void Player::setScale(float scale)
{
    sprite.setScale(scale, scale);
    float floorLevel = 450.0f;
    maxY = floorLevel - (frameHeight * scale);
    sf::Vector2f currentPos = sprite.getPosition();
    currentPos.y = std::max(minY, std::min(currentPos.y, maxY));
    sprite.setPosition(currentPos);
}

float Player::getScale() const
{
    return sprite.getScale().x;
}

bool Player::isJetpackActive() const
{
    return sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
}

void Player::update(float dt)
{
    if (isDead) {
        updateAnimation(dt);
        return;
    }
    updatePhysics(dt);
    updateState();
    updateAnimation(dt);
}

void Player::updatePhysics(float dt)
{
    if (isJetpackActive()) {
        velocityY = jetpackForce;
    } else {
        velocityY += gravity * dt;
    }

    sf::Vector2f newPosition = sprite.getPosition();
    newPosition.y += velocityY * dt;
    
    newPosition.y = std::max(minY, std::min(newPosition.y, maxY));
    sprite.setPosition(newPosition);
}

void Player::updateState()
{
    bool isNowOnGround = (sprite.getPosition().y >= maxY);
    
    if (!wasOnGround && isNowOnGround) {
        setAnimationState(PlayerState::LANDING);
    } else if (isJetpackActive() || (!isNowOnGround && velocityY != 0)) {
        setAnimationState(PlayerState::JETPACK);
    } else if (isNowOnGround && currentState == PlayerState::LANDING) {
    } else if (isNowOnGround) {
        setAnimationState(PlayerState::RUNNING);
    }
    
    wasOnGround = isNowOnGround;
}

void Player::updateAnimation(float dt)
{
    animationTimer += dt;
    
    if (animationTimer >= frameDuration) {
        animationTimer -= frameDuration;
        currentFrame++;
        
        int maxFrames = getMaxFrames();
        
        if (currentFrame >= maxFrames) {
            if (currentState == PlayerState::LANDING) {
                setAnimationState(PlayerState::RUNNING);
            } else if (currentState == PlayerState::DEATH_AIR || 
                       currentState == PlayerState::DEATH_GROUND) {
                currentFrame = maxFrames - 1;
                deathAnimationCompleted = true;
            } else {
                currentFrame = 0;
            }
        }
        
        int yPos = getYPosition();
        
        if (currentState == PlayerState::DEATH_AIR && currentFrame >= 4) {
            yPos = deathAirY + frameHeight;
            sprite.setTextureRect(sf::IntRect((currentFrame - 4) * frameWidth, yPos, frameWidth, frameHeight));
        } else {
            sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, yPos, frameWidth, frameHeight));
        }
    }
}

int Player::getMaxFrames() const
{
    switch (currentState) {
        case PlayerState::RUNNING: return runFrameCount;
        case PlayerState::JETPACK: return jetpackFrameCount;
        case PlayerState::LANDING: return landingFrameCount;
        case PlayerState::DEATH_AIR: return deathAirFrameCount;
        case PlayerState::DEATH_GROUND: return deathGroundFrameCount;
        default: return 0;
    }
}

int Player::getYPosition() const
{
    switch (currentState) {
        case PlayerState::RUNNING: return runningY;
        case PlayerState::JETPACK: return jetpackY;
        case PlayerState::LANDING: return landingY;
        case PlayerState::DEATH_AIR: return deathAirY;
        case PlayerState::DEATH_GROUND: return deathGroundY;
        default: return 0;
    }
}

void Player::setAnimationState(PlayerState state)
{
    if (state == currentState)
        return;
    
    currentState = state;
    animationTimer = 0.f;
    currentFrame = 0;
    
    int yPos = getYPosition();
    sprite.setTextureRect(sf::IntRect(0, yPos, frameWidth, frameHeight));
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

sf::FloatRect Player::getBounds() const
{
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    
    float reduction = 0.7f;
    float widthReduction = spriteBounds.width * (1.0f - reduction) / 2.0f;
    float heightReduction = spriteBounds.height * (1.0f - reduction) / 2.0f;
    
    return sf::FloatRect(
        spriteBounds.left + widthReduction,
        spriteBounds.top + heightReduction,
        spriteBounds.width * reduction,
        spriteBounds.height * reduction
    );
}

void Player::die(bool inAir)
{
    if (isDead)
        return;
    
    isDead = true;
    deathAnimationCompleted = false;
    
    if (inAir) {
        setAnimationState(PlayerState::DEATH_AIR);
    } else {
        setAnimationState(PlayerState::DEATH_GROUND);
    }
}

bool Player::isDeathAnimationCompleted() const
{
    return isDead && deathAnimationCompleted;
}