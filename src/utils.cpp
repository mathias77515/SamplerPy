#include "header/utils.h"
#include <iostream>
#include <chrono>

void printProgressBar(int current, int total, int barLength = 50) {
    if (current == 0 || total == 0) return;

    double percentage = (static_cast<double>(current) / total) * 100;
    int progressBar = static_cast<int>(percentage);

    std::string progressBarString = "";
    for (int i = 0; i < barLength; ++i) {
        if (progressBar >= (barLength - 1)) {
            progressBarString += "#";
        } else if (progressBar > 0) {
            progressBarString += "-";
        } else {
            progressBarString += " ";
        }
    }

    std::cout << "\rProgress: [" << progressBarString << "] " << percentage << "%"
              << ", Estimated time remaining: "
              << static_cast<double>(total - current) / (percentage / 100.0)
              << " seconds" << std::flush;
}