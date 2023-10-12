/**
 * @file Threads.cpp
 *
 * @brief Stores program threads in a common place.
 *
 * @author Robert Myers
 * Contact: romyers@umich.edu
 */

#pragma once

#include <vector>
#include <thread>

using namespace std;

namespace Muon {
namespace ProgramFlow {

    vector<thread> threads;

    mutex threadLock;

    // NOTE: This logic is a little complex because we need the threads vector
    //       to 'refresh' every time we join. We also need to make sure 
    //       the threads vector isn't modified while we're validating and
    //       retrieving a thread from it, but we can't join while the
    //       threadLock is locked, or the program will deadlock.
    //       
    //       This implementation acts as an iteration step, checking the
    //       threads vector for threads to join, grabbing a thread off the
    //       vector, and joining it. If there are no threads to join, it
    //       returns false.
    //
    //       Used as a condition in a while loop, this will join all threads
    //       in the thread vector, even if the thread was added to the vector
    //       after the first join call.
    bool joinNextThread() {

        threadLock.lock();

        bool empty = threads.empty();
        if(empty) {

            threadLock.unlock();
            return false;

        }

        thread t = move(threads.back());
        threads.pop_back();

        threadLock.unlock();

        t.join();

        return true;

    }

    void joinAllThreads() {

        while(joinNextThread()) {}

    }

}
}