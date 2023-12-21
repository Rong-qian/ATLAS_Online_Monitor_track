#include "UILoop.h"

#include <chrono>
#include <cmath>
#include <thread>

#include "TSystem.h"

#include "UILock.h"
#include "UIException.h"

#include "AlertBox/AlertOperations.h"

#include "DAQMonitor/ProgramControl/Terminator.h"

using namespace std;
using namespace Muon;

/**
 * Runs a UI event loop step.
 * 
 * @return The time it took the loop step to complete.
 */
chrono::milliseconds runLoopStep();

void startLoop(double refreshRate) {

    // TODO: Explore using app.Run() instead of manually calling 
    //       ProcessEvents()

    // Run the UI update loop
    while(!Terminator::getInstance().isTerminated()) {

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

chrono::milliseconds runLoopStep() {

    // Record the loop start time
    auto UIUpdateStartTime = chrono::high_resolution_clock::now();

    // TODO: I don't like the UILock being used at this level.
    // Process UI events.
    UI::UILock.lock();
    try {

        gSystem->ProcessEvents();

    } catch (UIException &e) {

    	// TODO: I don't want this here.
        Error::popupAlert(
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