#include "Parser.hpp"
#include <fstream>
#include <iostream>

Parser::Parser() : width(0), height(0) {}

bool Parser::loadMapFromFile(const std::string& filename)

{
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error: Failed to open " << filename << std::endl;
        return false;
    }

    map.clear();

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::vector<TileType> row;
        for (char c : line) {
            if (c == 'c') {
                row.push_back(TileType::COIN);
            } else if (c == 'e') {
                row.push_back(TileType::ZAPPER);
            } else {
                row.push_back(TileType::EMPTY);
            }
        }
        
        if (width == 0) {
            width = row.size();
        } else if (row.size() != static_cast<size_t>(width)) {
            row.resize(width, TileType::EMPTY);
        }
        
        map.push_back(row);
    }
    
    height = map.size();
    file.close();
    
    return true;
}