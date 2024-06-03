#include "DataRunOperations.h"

#include <string>
#include <thread>
#include <fstream>
#include <sstream>

#include "TObjArray.h"
#include "TObjString.h"

#include "DataCaptureOperations.h"
#include "PacketDecodingOperations.h"
#include "DAQState.h"
#include "LockableStream.h"

#include "ProgramControl/Threads.h"
#include "DataModel/DAQData.h"

#include "Logging/ErrorLogger.h"

#include "GUI/Core/UIException.h"
#include "GUI/Core/UISignals.h"
#include "GUI/Core/UILock.h"

#include "analysis/MonitorHooks.h"

#include "MuonReco/Geometry.h"
#include "MuonReco/ConfigParser.h"

using namespace std;
using namespace State;
using namespace MonitorHooks;
using namespace MuonReco;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

const string CONF_PATH = "../conf/";

bool runStarted = false;


void   initializeDataStream(LockableStream &dataStream   );
string getCurrentTimestamp (const string   &format       );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void initializeDataStream(LockableStream &dataStream) {

    dataStream.stream = nullptr;

    State::DAQState state = State::DAQState::getState();

    if(state.persistentState.dataSource == DAT_FILE_SOURCE) {

        cout << "Reading data from file: " << state.persistentState.inputFilename << endl;

        fstream *fileStream = new fstream(state.persistentState.inputFilename);
        if(!fileStream->is_open()) {
                            
            delete fileStream;
            fileStream = nullptr;

            throw logic_error("Could not open input file stream.");

        }

        dataStream.stream = fileStream;

    } else if(state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

        dataStream.stream = new stringstream();

    }

}

bool DataRun::isRunning() { return runStarted; }

void DataRun::stopRun() {

    if(!isRunning()) {

        throw UIException(
            "Please start a run."
        );

    }

    DataCapture::stopDataCapture();
    Decode::stopDecoding();

}

void DataRun::startRun() {

    // Reset the error logger
    // TODO: This relegates the error logger to a per-run role, so it
    //       should not be used at the level of the wider program.
    //         -- Also, semantically, this sorta makes us want to 
    //            let the error logger not be a singleton anymore.
    ErrorLogger::getInstance().clear();

    DAQState state = DAQState::getState();

    string runLabel = "";

    // Abort if a run is already in progress
    if(isRunning()) {

        throw UIException(
            "Please finish the current run before starting a new run."
        );

    }

    // Set up the input source
    if(state.persistentState.dataSource == DAT_FILE_SOURCE) {

        // Check that the file name is nonempty
        if(state.persistentState.inputFilename == "") {

            throw UIException("Please select a DAT file.");

        }

        // Check that the file exists
        if(!fstream(state.persistentState.inputFilename).is_open()) {

            throw UIException(
                string("File \"") 
                + state.persistentState.inputFilename 
                + "\" could not be opened."
            );

        }

        string filename = state.persistentState.inputFilename;

        cout << endl << "File source selected" << endl;
        cout << "Filename: " << filename << endl;

        size_t extensionPos = filename.find_last_of(".");

        if(extensionPos == string::npos) {

            runLabel = filename;

        } else {

            runLabel = filename.substr(0, extensionPos);

        }

    } else if (state.persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

        if(state.persistentState.inputDevicename == "") {

            throw UIException("Please select a network device.");

        }

        cout << endl << "Network source selected" << endl;
        cout << "Ethernet device: " << state.persistentState.inputDevicename << endl;

        runLabel = string("run_") + getCurrentTimestamp("%Y%m%d_%H%M%S");

    }

    cout << endl;

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    DAQData &data = DAQData::getInstance();
    
    // Clear the DAQData of any data from a previous run

    data.lock  ();
    data.clear ();
    data.unlock();

    // Configure the geometry and time correction table.

    // NOTE: Write operations on data.geo MUST NOT be performed concurrently
    //       with the decode loop in the current implementation, or thread
    //       safety is not guaranteed. So if data.geo needs to be reconfigured
    //       during a run, the configuration and decoding operations must be
    //       synchronized.
    //         -- TL;DR the decode loop does not lock 'data' before reading 
    //            from geo, so be careful not to change geo while the decode 
    //            loop is running.

    ConfigParser cp(CONF_PATH + state.persistentState.confFilename);

    data.lock();

    try{

        // TODO: Consider moving some of this into the DAQData constructor
        data.geo.Configure(cp.items("Geometry"));
	data.Reload();
        data.tc = TimeCorrection(cp);
        // data.tc.Read();

        data.recoUtil = RecoUtility(cp.items("RecoUtility"));

        data.rtp = RTParam(cp);
        //data.rtp.Initialize("../conf/T0.root", data.plots);
        //TFile autocal("../conf/autocalibration/autoCalibratedRT_0_100000.root");
        
        data.T0Path = cp.items("RecoRTP").getStr("T0Path" , "./conf/T0.root");
        data.RTPath = cp.items("RecoRTP").getStr("RTPath" , "../conf/autocalibration/autoCalibratedRT_0_100000.root");
        
        data.rtp.Initialize(data.T0Path.c_str(), data.plots);
        TFile autocal(data.RTPath.c_str());
        data.rtp.Load(&autocal);

        data.eventDisplay.SetRT(&data.rtp);

    } catch(int e) {

        data.unlock();

        throw UIException(
            string("Failed to configure from ") 
                + state.persistentState.confFilename
                + string(".\n Does the file exist?")
        );


    }

    data.unlock();

    cout << "Conf file: " << state.persistentState.confFilename << endl;

    // NOTE: Following the legacy code, runN is in YYYYMMDD format and does
    //       not include hours/minutes/seconds
    // NOTE: This assumes the DAT filenames are formatted as 
    //       "run_YYYYMMDD_HHMMSS.dat"
    // NOTE: DecodeRawData from the sMDT reco library never sets runN, so I'm 
    //       leaving it out here for now.
    /*
    int runN = (
        (TObjString*)(TString(
            runLabel.substr(3, runLabel.size()).data()
        ).Tokenize("_")->At(0))
    )->String().Atoi();
    data.geo.SetRunN(runN);
    */

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    runStarted = true;
    state.tempState.runLabel = runLabel;
    state.commit(); // TODO: This shouldn't fail, but better if it's robust

    // TODO: Hook up error handling on a per-thread basis. Threads should
    //       report to a threadsafe error handler that does the error handling
    // TODO: It's not obvious why we make a parent thread. It's because we want
    //       a few things to happen after the capture and decode threads are
    //       joined. Make that more obvious.
    ProgramFlow::threadLock.lock();
    ProgramFlow::threads.emplace_back(thread([&data, runLabel]() {

        MonitorHooks::beforeStartRun(data);

        cout << endl << "Starting run: " << runLabel << endl; 

        UI::UILock.lock();
        UISignalBus::getInstance().onRunStart();
        UI::UILock.unlock();

        LockableStream dataStream;
        initializeDataStream(dataStream);

        // DATA CAPTURE LOOP
        thread dataCaptureThread([&dataStream, &data, runLabel]() {

            if(DAQState::getState().persistentState.dataSource == NETWORK_DEVICE_SOURCE) {

                DataCapture::startDataCapture(dataStream, data, runLabel);

            }

        });
	
        // DECODE LOOP
        thread decodeThread([&dataStream, &data, runLabel](){

            Decode::startDecoding(dataStream, data, runLabel);

        });

        MonitorHooks::startedRun(data);

        dataCaptureThread.join();
        decodeThread     .join();

        // TODO: Again, I would rather avoid caring about the type of stream.
        dataStream.lock();
        fstream *temp = dynamic_cast<fstream*>(dataStream.stream);
        if(temp) {
            temp->close();
        }
        dataStream.unlock();

        if(dataStream.stream) delete dataStream.stream;
        dataStream.stream = nullptr;

        DAQState state = DAQState::getState();
        runStarted = false;
        state.commit();

        UI::UILock.lock();
        UISignalBus::getInstance().onRunStop();
        UI::UILock.unlock();

        MonitorHooks::finishedRun(data);

        cout << endl;
        cout << "Run finished!" << endl;

        // TODO: Yet another hard-to-find place we do this.
        UI::UILock.lock();
        UISignalBus::getInstance().onUpdate();
        UI::UILock.unlock();

    }));

    ProgramFlow::threadLock.unlock();

}

string getCurrentTimestamp(const string &format) {

    char formatBuffer[40];
    time_t sys_time;
    struct tm *timeinfo;
    sys_time = time(0);
    timeinfo = localtime(&sys_time);
    memset(formatBuffer, 0, sizeof(formatBuffer));
    strftime(formatBuffer, 40, format.data(), timeinfo);

    return string(formatBuffer);

}
