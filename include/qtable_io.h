#ifndef QTABLE_IO_H
#define QTABLE_IO_H

#include "bot.h"
#include <vector>
#include <string>

bool save_qtable(const std::vector<std::vector<float>>& Q, const std::string& filename);
bool load_qtable(std::vector<std::vector<float>>& Q, const std::string& filename);
bool save_qtable_text(const std::vector<std::vector<float>>& Q, const std::string& filename);

#endif