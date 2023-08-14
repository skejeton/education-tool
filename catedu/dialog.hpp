//
// Created by skeje on 7/20/2023.
//

#ifndef HPP_CATEDU_DIALOG
#define HPP_CATEDU_DIALOG

#include <queue>
#include <string>
#include <vector>


enum class DialogType {
    Info,       // For displaying information.
    Pending,    // For indicating wait.
    Warning,    // For indicating a warning.
    Error       // For indicating an error.
};


struct Dialog {
    DialogType type;
    std::string text;

    bool show();
};


struct DialogQueue {
    std::queue<Dialog> dialogs;

    void push(std::string text, DialogType type);
    void show();
};

#endif
