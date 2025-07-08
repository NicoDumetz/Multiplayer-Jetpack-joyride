#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Player/Player.hpp"
#include "Map/Map.hpp"

#include <iostream>
#include <algorithm>
#include <fstream>

class Game {
public:
    Game(const std::string& mapFile = "default.map");
    ~Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void checkCoinCollisions();
    void checkZapperCollisions();
    void updateScoreDisplay();
    void renderDebugHitboxes();
    void gameOver();
    void resetGame();
    
    void initWindow();
    void initPlayer();
    void loadMap();
    void initUI();
    void drawHitbox(const sf::FloatRect& bounds, const sf::Color& color);
    void playSound(const std::string& name, float volume = 100.0f);
    void playMusic(const std::string& filename, float volume = 50.0f);
    
    sf::RenderWindow window;
    sf::Clock clock;
    Player player;
    Map map;
    std::string mapFile;
    int score;
    bool isGameRunning = true;
    sf::Font font;
    sf::Text scoreText;
    sf::Music music;
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::vector<sf::Sound> sounds;
    
    
    
};