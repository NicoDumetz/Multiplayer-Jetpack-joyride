#include "Game/Game.hpp"
#include <iostream>
#include <fstream>

bool checkMap(const std::string& mapFile)
{

    if (mapFile.substr(mapFile.find_last_of(".") + 1) != "map") {
        std::cerr << "Error: File must be .map extension\n";
        return false;
    }

    std::ifstream mapStream(mapFile);
    if (!mapStream.is_open()) {
        std::cerr << "Error: Failed opening " << mapFile << "\n";
        return false;
    }
    
    mapStream.close();
    return true;
}

int main(int argc, char** argv)
{
    try {
        std::string mapFile = "default.map";

        if (argc > 1) {
            mapFile = argv[1];
            if (!checkMap(mapFile)) {
                return 84;
            }
        } else {
            std::ifstream src("map.txt");
            if (!src.is_open()) {
                std::cerr << "Error: Could not find map.txt\n";
                return 84;
            }
            
            std::ofstream dst("default.map");
            if (!dst.is_open()) {
                std::cerr << "Error: Could not create default.map\n";
                return 84;
            }
            
            dst << src.rdbuf();
            src.close();
            dst.close();
        }
        
        Game game(mapFile);
        game.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    
    return 0;
}