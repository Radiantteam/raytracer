#ifndef RAYTRACING_PROGRESSBAR_H
#define RAYTRACING_PROGRESSBAR_H

#include <iostream>


class ProgressBar {
public:
    ProgressBar(int total) : totalLines(total) {}

    void update(int current) {
        if (current % 100 == 0) {
            int percent = (100 * current) / totalLines;
            std::cout << "Progress: " << percent << "%\r" << std::flush;
        }
    }

    void finish() {
        std::cout << "\nProgress: 100%\n";
    }

private:
    int totalLines;
};



#endif //RAYTRACING_PROGRESSBAR_H