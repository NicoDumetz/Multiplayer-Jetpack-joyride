#include "Map.hpp"
#include <iostream>

Map::Map(sf::Vector2u windowSize) : background(windowSize), windowSize(windowSize)
{
    if (!coinTexture.loadFromFile("Assets/coin.png")) {
        std::cerr << "Error: Could not load coin texture" << std::endl;
    }
    
    if (!zapperTexture.loadFromFile("Assets/zapper.png")) {
        std::cerr << "Error: Could not load zapper texture" << std::endl;
    }
    
    coinTexture.setSmooth(true);
    zapperTexture.setSmooth(true);
}

Map::~Map() {}

bool Map::loadMap(const std::string& filename)
{
    if (!parser.loadMapFromFile(filename)) {
        return false;
    }
    
    coins.clear();
    zappers.clear();
    createObjects(parser.getMap());
    
    return true;
}

void Map::createObjects(const std::vector<std::vector<TileType>>& mapData)
{
    float topLimit = 150.0f;
    float bottomLimit = windowSize.y - 150.0f;
    
    for (int y = 0; y < parser.getHeight(); ++y) {
        for (int x = 0; x < parser.getWidth(); ++x) {
            float yPosition = topLimit + (bottomLimit - topLimit) * y / std::max(1, parser.getHeight() - 1);
            float xPosition = x * tileSize + windowSize.x;
            
            if (mapData[y][x] == TileType::COIN) {
                coins.emplace_back(coinTexture, xPosition, yPosition, tileSize);
            } else if (mapData[y][x] == TileType::ZAPPER) {
                zappers.emplace_back(zapperTexture, xPosition, yPosition, tileSize);
            }
        }
    }
}

void Map::update(float dt)
{
    background.update(dt);
    float moveX = scrollSpeed * dt;
    mapOffset += moveX;
    updateObjects(dt, moveX);
}

void Map::updateObjects(float dt, float moveX)
{
    for (auto& coin : coins) {
        coin.update(dt);
        coin.move(-moveX, 0);
        
        if (coin.getPosition().x + tileSize/2 < 0) {
            float yPos = coin.getPosition().y;
            coin.setPosition(coin.getPosition().x + windowSize.x + parser.getWidth() * tileSize, yPos);
        }
    }
    
    for (auto& zapper : zappers){
        zapper.update(dt);
        zapper.move(-moveX, 0);
        
        if (zapper.getPosition().x + tileSize/2 < 0) {
            float yPos = zapper.getPosition().y;
            zapper.setPosition(zapper.getPosition().x + windowSize.x + parser.getWidth() * tileSize, yPos);
        }
    }
}

void Map::draw(sf::RenderWindow& window)
{
    background.draw(window);
    
    for (const auto& coin : coins) {
        coin.draw(window);
    }
    
    for (const auto& zapper : zappers) {
        zapper.draw(window);
    }
}