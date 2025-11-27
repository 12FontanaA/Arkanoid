#ifndef ITEMS_H
#define ITEMS_H

#include "raylib.h"
#include "game.h"
#include <vector>

enum class ItemType {
    EXTRA_LIFE,
    BIGGER_PADDLE,
    SMALLER_PADDLE,
    POINTS_BONUS,
    MULTIPLIER,
    SCORE_INVERTER
};

struct Item {
    Rectangle rect;
    ItemType type;
    bool active;
    float fallSpeed;
    Color color;
};

void SpawnItem(Vector2 position, std::vector<Item>& items);
void UpdateItems(std::vector<Item>& items, Rectangle& paddle, GameState& gameState);
void DrawItems(const std::vector<Item>& items);

#endif