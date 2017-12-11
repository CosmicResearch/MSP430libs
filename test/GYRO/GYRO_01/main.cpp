#define SENSCAPE_MAIN

#include "Senscape.h"

int main(void) {
    init();

    initTaskman();

    /* booter */
    Booter::bootAll();
    while (runNextTask());

    if (Booter::getResult() == SUCCESS) {
        /* user setup */
        setup();

        /* main loop */
        for (;;) {
            loop();

            runNextTask();

            /* wait for serial events */
            if (serialEventRun) {
                serialEventRun();
            }
        }
    }

    return 0;
}
