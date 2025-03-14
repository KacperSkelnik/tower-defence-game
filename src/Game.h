#pragma once

#include "EconomyState.h"
#include "interface/BuildingSelector.h"
#include "interface/EconomyPanel.h"
#include "interface/Grid.h"
#include <vector>

class Game {
  private:
    // Board
    std::vector<std::unique_ptr<Building>> buildings;

    // State
    std::optional<BuildingType> selectedBuilding;
    EconomyState                economyState;

    // Interface
    Grid             grid;
    BuildingSelector buildingSelector;
    EconomyPanel     economyPanel;
    bool             screenCanBeDragged;

    static void onClose();
    void        onMousePress(const sf::Event::MouseButtonPressed* event);
    void        onMouseRelease(const sf::Event::MouseButtonReleased* event);
    void        onMouseScroll(const sf::Event::MouseWheelScrolled* event) const;

    void handleEvent(const sf::Event& event);
    void draw() const;

    Game(std::initializer_list<BuildingType> buildingTypes);

  public:
    Game() = delete;
    ~Game();
    static Game create(std::initializer_list<BuildingType> buildingTypes);
    void        run();
};