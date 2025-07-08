#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

enum class TileType {
    EMPTY,
    COIN,
    ZAPPER
};

class Parser {
public:
    Parser();
    bool loadMapFromFile(const std::string& filename);
    
    const std::vector<std::vector<TileType>>& getMap() const { return map; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    std::vector<std::vector<TileType>> map;
    int width;
    int height;
};