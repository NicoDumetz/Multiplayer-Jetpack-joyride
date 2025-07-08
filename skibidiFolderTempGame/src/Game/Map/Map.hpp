#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include "Parser/Parser.hpp"
#include "MapRenderer.hpp"
#include "Coin/Coin.hpp"
#include "Zapper/Zapper.hpp"


class Map {
public:
    Map(sf::Vector2u windowSize);
    ~Map();
    
    bool loadMap(const std::string& filename);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    
    const std::vector<Coin>& getCoins() const { return coins; }
    std::vector<Coin>& getCoins() { return coins; }
    
    const std::vector<Zapper>& getZappers() const { return zappers; }
    std::vector<Zapper>& getZappers() { return zappers; }

private:
    Parser parser;
    MapRenderer background;
    
    sf::Texture coinTexture;
    sf::Texture zapperTexture;
    
    std::vector<Coin> coins;
    std::vector<Zapper> zappers;
    
    float scrollSpeed = 200.0f;
    float tileSize = 40.0f;
    
    sf::Vector2u windowSize;
    float mapOffset = 0.0f;
    
    void createObjects(const std::vector<std::vector<TileType>>& mapData);
    void updateObjects(float dt, float moveX);
};