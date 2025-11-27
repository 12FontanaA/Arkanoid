#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include <string>

enum class MenuAction {
    Start,
    StartBot,
    Ranking,
    Quit
};

enum class GameOverAction {
    ReturnToMenu,
    Restart,
    Quit
};

MenuAction ShowMainMenu(float &speedMultiplier, bool &botMode);
void ShowRanking();
GameOverAction ShowGameOverMenu();

#endif