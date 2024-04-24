#include "UILoop.h"

#include <chrono>
#include <cmath>
#include <thread>

#include "TSystem.h"

#include "UILock.h"
#include "UIException.h"

#include "AlertBox/AlertOperations.h"

using namespace std;

bool isRunning = false;

/**
 * Runs a UI event loop step.
 * 
 * @return The time it took the loop step to complete.
 */
chrono::milliseconds runLoopStep();

void startLoop(double refreshRate) {

    if(isRunning) return;

    isRunning = true;

    // TODO: Explore using app.Run() instead of manually calling 
    //       ProcessEvents()

    // TODO: Remove condition on terminator, and have something call stopRun()
    //       at a higher level when the terminator terminates. Can do this by
    //       having the terminator be a TQObject and signaling when it 
    //       terminates.
    // Run the UI update loop
    while(isRunning) {

        chrono::milliseconds updateTime = runLoopStep();

        // Compute how long the thread should sleep to maintain a 
        // consistent frame rate
        int sleepTime = (int)(1000. / refreshRate) - (int)(updateTime.count());

        // Deal with the case where the update loop took too long
        if(sleepTime < 0) {

            sleepTime = 0;

        }

        // Sleep until the next update time.
        this_thread::sleep_for(
            chrono::milliseconds(sleepTime)
        );

    }

}

void stopLoop() {

    isRunning = false;

}

chrono::milliseconds runLoopStep() {

    // Record the loop start time
    auto UIUpdateStartTime = chrono::high_resolution_clock::now();

    // Process UI events.
    UI::UILock.lock();
    try {

        gSystem->ProcessEvents();

    } catch (UIException &e) {

    	// TODO: I don't want this here.
        AlertPopup::popupAlert(
            gClient->GetRoot(), "Error", e.what()
        );

    }
    UI::UILock.unlock();

    // Record the loop end time
    auto UIUpdateEndTime = chrono::high_resolution_clock::now();

    // Compute how long it took the loop to run
    chrono::duration<double> loopTime = UIUpdateEndTime - UIUpdateStartTime;

    return chrono::duration_cast<chrono::milliseconds>(loopTime);

}