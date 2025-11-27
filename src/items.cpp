#include "items.h"
#include <random>

void SpawnItem(Vector2 position, std::vector<Item>& items) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> typeDist(0, 5);
    
    Item item;
    item.rect = { position.x - 15, position.y, 30, 30 };
    item.type = static_cast<ItemType>(typeDist(gen));
    item.active = true;
    item.fallSpeed = 100.0f;
    
    switch (item.type) {
        case ItemType::EXTRA_LIFE:
            item.color = RED;
            break;
        case ItemType::BIGGER_PADDLE:
            item.color = GREEN;
            break;
        case ItemType::SMALLER_PADDLE:
            item.color = BLUE;
            break;
        case ItemType::POINTS_BONUS:
            item.color = YELLOW;
            break;
        case ItemType::MULTIPLIER:
            item.color = PURPLE;
            break;
        case ItemType::SCORE_INVERTER:
            item.color = ORANGE;
            break;
    }
    
    items.push_back(item);
}

void UpdateItems(std::vector<Item>& items, Rectangle& paddle, GameState& gameState) {
    for (auto it = items.begin(); it != items.end();) {
        it->rect.y += it->fallSpeed * GetFrameTime();
        
        if (CheckCollisionRecs(it->rect, paddle)) {
            switch (it->type) {
                case ItemType::EXTRA_LIFE:
                    gameState.lives++;
                    break;
                case ItemType::BIGGER_PADDLE:
                    paddle.width = (paddle.width + 20.0f < 200.0f) ? paddle.width + 20.0f : 200.0f;
                    break;
                case ItemType::SMALLER_PADDLE:
                    paddle.width = (paddle.width - 20.0f > 50.0f) ? paddle.width - 20.0f : 50.0f;
                    break;
                case ItemType::POINTS_BONUS:
                    gameState.score += 100;
                    break;
                case ItemType::MULTIPLIER:
                    gameState.score = (int)(gameState.score * 1.5f);
                    break;
                case ItemType::SCORE_INVERTER:
                    gameState.scoreInverted = true;
                    gameState.inversionTimer = 10.0f;
                    break;
            }
            it = items.erase(it);
        } else if (it->rect.y > GetScreenHeight()) {
            it = items.erase(it);
        } else {
            ++it;
        }
    }
}

void DrawItems(const std::vector<Item>& items) {
    for (const auto& item : items) {
        DrawRectangleRec(item.rect, item.color);
        DrawRectangleLinesEx(item.rect, 2, BLACK);
        
        switch (item.type) {
            case ItemType::EXTRA_LIFE:
                DrawText("H", item.rect.x + 8, item.rect.y + 5, 20, WHITE);
                break;
            case ItemType::BIGGER_PADDLE:
                DrawText("B", item.rect.x + 8, item.rect.y + 5, 20, WHITE);
                break;
            case ItemType::SMALLER_PADDLE:
                DrawText("S", item.rect.x + 8, item.rect.y + 5, 20, WHITE);
                break;
            case ItemType::POINTS_BONUS:
                DrawText("$", item.rect.x + 8, item.rect.y + 5, 20, WHITE);
                break;
            case ItemType::MULTIPLIER:
                DrawText("X", item.rect.x + 8, item.rect.y + 5, 20, WHITE);
                break;
            case ItemType::SCORE_INVERTER:
                DrawText("!", item.rect.x + 8, item.rect.y + 5, 20, WHITE);
                break;
        }
    }
}