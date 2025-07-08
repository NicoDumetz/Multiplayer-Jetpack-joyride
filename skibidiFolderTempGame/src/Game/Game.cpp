#include "Game.hpp"

Game::Game(const std::string& mapFile) :
    window(sf::VideoMode(800, 600), "Jetpack Client", sf::Style::Titlebar | sf::Style::Close),
    map(window.getSize()),
    mapFile(mapFile),
    score(0),
    isGameRunning(true)
{
    initWindow();
    initPlayer();
    loadMap();
    initUI();
    playMusic("theme");
}

Game::~Game()
{
    music.stop();
    scoreText = sf::Text();
}

void Game::initWindow()
{
    window.setFramerateLimit(60);
}

void Game::initPlayer()
{
    player.setPosition({200, 300});
    player.setScale(0.45f);
}

void Game::loadMap()
{
    if (!map.loadMap(mapFile)) {
        std::cerr << "Error: Failed to load map file: " << mapFile << std::endl;
    }
}

void Game::initUI()
{
    if (!font.loadFromFile("Assets/font.ttf")) {
        std::cerr << "Error: Failed to load font" << std::endl;
    }
    
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20, 20);
    scoreText.setString("Score: 0");
}

void Game::playMusic(const std::string& filename, float volume)
{
    std::string filepath = "Assets/" + filename + ".ogg";
    std::ifstream fileCheck(filepath);
    if (!fileCheck) {
        return;
    }
    fileCheck.close();
    
    if (!music.openFromFile(filepath)) {
        std::cerr << "Error: Failed to load music: " << filename << std::endl;
        return;
    }
    
    music.setLoop(true);
    music.setVolume(volume);
    music.play();
}

void Game::playSound(const std::string& name, float volume)
{

    std::string filepath = "Assets/" + name + ".ogg";
    std::ifstream fileCheck(filepath);
    if (!fileCheck) {
        return;
    }
    fileCheck.close();
    
    auto it = soundBuffers.find(name);
    
    if (it == soundBuffers.end()) {
        sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(filepath)) {
            std::cerr << "Error: Failed to load sound: " << name << std::endl;
            return;
        }
        it = soundBuffers.insert(std::make_pair(name, buffer)).first;
    }
    
    sounds.erase(
        std::remove_if(sounds.begin(), sounds.end(), 
            [](const sf::Sound& s) { return s.getStatus() == sf::Sound::Stopped; }
        ),
        sounds.end()
    );
    
    sounds.emplace_back(it->second);
    sounds.back().setVolume(volume);
    sounds.back().play();
}

void Game::run()
{
    while (window.isOpen()) {
        processEvents();
        float dt = clock.restart().asSeconds();
        update(dt);
        render();
    }
}

void Game::processEvents()
{
    sf::Event event;

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) { // TODO : Fix this car ça marche pas XDDDD MEME
            map.loadMap(mapFile);
            score = 0;
            updateScoreDisplay();
        }
    }
}

void Game::update(float dt)
{
    if (!isGameRunning)
        return;
    
    map.update(dt);
    player.update(dt);
    
    if (player.isDying()) {
        if (player.isDeathAnimationCompleted()) {
            gameOver();
        }
        return;
    }
    
    checkCoinCollisions();
    checkZapperCollisions();
}

void Game::render()
{
    window.clear();
    
    map.draw(window);
    player.draw(window);
    window.draw(scoreText);
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
        renderDebugHitboxes();
    }
    
    window.display();
}

void Game::renderDebugHitboxes()
{
    drawHitbox(player.getBounds(), sf::Color::Green);
    
    for (const auto& coin : map.getCoins()) {
        drawHitbox(coin.getBounds(), sf::Color::Yellow);
    }
    
    for (const auto& zapper : map.getZappers()) {
        drawHitbox(zapper.getBounds(), sf::Color::Red);
    }
}

void Game::drawHitbox(const sf::FloatRect& bounds, const sf::Color& color)
{
    sf::RectangleShape hitbox(sf::Vector2f(bounds.width, bounds.height));

    hitbox.setPosition(bounds.left, bounds.top);
    hitbox.setFillColor(sf::Color::Transparent);
    hitbox.setOutlineColor(color);
    hitbox.setOutlineThickness(1);
    window.draw(hitbox);
}

void Game::checkCoinCollisions()
{
    sf::FloatRect playerBounds = player.getBounds();

    auto& coins = map.getCoins();
    
    for (auto it = coins.begin(); it != coins.end();) {
        if (playerBounds.intersects(it->getBounds())) {
            score += 10;
            updateScoreDisplay();
            //playSound("coin", 80.0f);
            it = coins.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::checkZapperCollisions()
{
    if (player.isDying())
        return;
    
    sf::FloatRect playerBounds = player.getBounds();
    
    for (const auto& zapper : map.getZappers()) {
        if (playerBounds.intersects(zapper.getBounds())) {
            bool playerInAir = player.getPosition().y < player.getMaxY() - 10;
            player.die(playerInAir);
            //playSound("hit", 90.0f);
            return;
        }
    }
}

void Game::updateScoreDisplay()
{
    scoreText.setString("Score: " + std::to_string(score));
}

void Game::gameOver()
{
    isGameRunning = false;
    
    // Fade out music
    float volume = music.getVolume();
    while (volume > 0) {
        volume -= 2;
        music.setVolume(volume);
        sf::sleep(sf::milliseconds(50));
    }
    music.stop();
    
    //playSound("gameover", 100.0f);
    
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(64);
    gameOverText.setFillColor(sf::Color::Red);
    
    sf::FloatRect textRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textRect.left + textRect.width/2.0f, 
                           textRect.top + textRect.height/2.0f);
    gameOverText.setPosition(window.getSize().x/2.0f, window.getSize().y/2.0f - 50);
    
    sf::Text finalScoreText;
    finalScoreText.setFont(font);
    finalScoreText.setString("Final Score: " + std::to_string(score));
    finalScoreText.setCharacterSize(32);
    finalScoreText.setFillColor(sf::Color::White);

    textRect = finalScoreText.getLocalBounds();
    finalScoreText.setOrigin(textRect.left + textRect.width/2.0f, 
                             textRect.top + textRect.height/2.0f);
    finalScoreText.setPosition(window.getSize().x/2.0f, window.getSize().y/2.0f + 50);
    
    while (window.isOpen() && !isGameRunning) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                resetGame();
                return;
            }
        }
        
        window.clear(sf::Color::Black);
        window.draw(gameOverText);
        window.draw(finalScoreText);
        window.display();
    }
}

void Game::resetGame()
{
    score = 0;
    updateScoreDisplay();
    map.loadMap(mapFile);
    player.setPosition({200, 300});
    isGameRunning = true;
    playMusic("theme");
}