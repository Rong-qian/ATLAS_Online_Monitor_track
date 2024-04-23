#include "DAQManager.h"

#include "../../DAQMonitor/PlotSavingOperations.h"

using namespace std;

ClassImp(DAQManager);

// NOTE: We MUST addFrame every GUI element to ensure proper cleanup

void DAQManager::makeConnections() {

    exitButton ->Connect("Clicked()", "DAQManager", this, "handlePressExit()");
    startButton->Connect("Clicked()", "DAQManager", this, "handlePressStart()");
    stopButton ->Connect("Clicked()", "DAQManager", this, "handlePressStop()");

    confFileSelector->Connect(
        "entryChanged(const char*)",
        "DAQManager",
        this,
        "selectedConfFile(const char*)"
    );

    dataSourcePanel->Connect(
        "selectedFileSource()",
        "DAQManager",
        this,
        "selectedFileSource()"
    );

    dataSourcePanel->Connect(
        "selectedDeviceSource()",
        "DAQManager",
        this,
        "selectedDeviceSource()"
    );

    dataSourcePanel->Connect(
        "selectedDevice(const char*)", 
        "DAQManager", 
        this, 
        "selectedDevice(const char*)"
    );

    dataSourcePanel->Connect(
        "selectedFile(const char*)",
        "DAQManager",
        this,
        "selectedFile(const char*)"
    );

}

void DAQManager::selectedFileSource  () { Emit("selectedFileSource()"  ); }
void DAQManager::selectedDeviceSource() { Emit("selectedDeviceSource()"); }

void DAQManager::selectedDevice(const char *selection) {

    Emit("selectedDevice(const char*)", selection);

}

void DAQManager::selectedFile(const char *selection) {

    Emit("selectedFile(const char*)", selection);

}

void DAQManager::selectedConfFile(const char *selection) {

    Emit("selectedConfFile(const char*)", selection);

}

DAQManager::DAQManager(
        const TGWindow *p, 
        int w, 
        int h, 
        int options
) : TGMainFrame(p, w, h, options), viewer(nullptr) {

    ///////////////////////////////////////////////////////////////////////////
    // Build the menu bar
    ///////////////////////////////////////////////////////////////////////////

    menuBar = new Menu(this);
    AddFrame(menuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft));

    Submenu *fileMenu = menuBar->AddSubmenu("&File");

        fileMenu->AddEntry("Save Plots", [](int id) {

            PlotSaving::savePlots();

        });

    Submenu *viewMenu = menuBar->AddSubmenu("&View");

    menuLine = new TGHorizontal3DLine(this);
    AddFrame(menuLine, new TGLayoutHints(kLHintsTop | kLHintsExpandX));

    ///////////////////////////////////////////////////////////////////////////

    mainPanel = new TGHorizontalFrame(this);
    AddFrame(mainPanel, new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));

        leftPanel = new TGVerticalFrame(mainPanel, 430, 300, kRaisedFrame);
        mainPanel->AddFrame(leftPanel, new TGLayoutHints(kLHintsExpandY | kLHintsLeft));
        leftPanel->ChangeOptions(leftPanel->GetOptions() | kFixedWidth);

            viewer = new RunView(leftPanel);
            leftPanel->AddFrame(viewer, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

            settings = new TGGroupFrame(leftPanel, "Settings", kVerticalFrame);
            leftPanel->AddFrame(settings, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));

                confFileSelector = new ConfFileSelector(settings);
                settings->AddFrame(confFileSelector);

                dataSourcePanel = new DataSourcePanel(settings);
                settings->AddFrame(dataSourcePanel);

        // splitter = new TGVSplitter(mainPanel);
        // mainPanel->AddFrame(splitter, new TGLayoutHints(kLHintsExpandY));

        // splitter->SetFrame(leftPanel, true);

        viewport = new TabPanel(mainPanel, 1250, 850);
        mainPanel->AddFrame(viewport, new TGLayoutHints(kLHintsExpandY));

            viewport->AttachToMenu(viewMenu);


    ///////////////////////////////////////////////////////////////////////////

    bottomPanel = new TGHorizontalFrame(this);
    AddFrame(bottomPanel, new TGLayoutHints(kLHintsExpandX));

        statusTag = new TGLabel(bottomPanel, "");
        bottomPanel->AddFrame(statusTag, new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 30, 5, 5, 5));

        buttonGroup = new TGButtonGroup(bottomPanel, "", kHorizontalFrame);
        bottomPanel->AddFrame(buttonGroup, new TGLayoutHints(kLHintsRight, 5, 50, 5, 5));

            startButton = new TGTextButton(buttonGroup, "Start Run");
            buttonGroup->AddFrame(startButton, new TGLayoutHints(kLHintsRight));

            stopButton = new TGTextButton(buttonGroup, "Stop Run");
            buttonGroup->AddFrame(stopButton, new TGLayoutHints(kLHintsRight));

            exitButton  = new TGTextButton(buttonGroup, "Exit");
            buttonGroup->AddFrame(exitButton, new TGLayoutHints(kLHintsRight));

    makeConnections();

    disableStopButton();

}

DAQManager::~DAQManager() {

}

void DAQManager::setDeviceSelectorEntry(const string &entry) {

    dataSourcePanel->setDeviceSelectorEntry(entry);

}

void DAQManager::setFileSelectorEntry(const string &entry) {

    dataSourcePanel->setFileSelectorEntry(entry);

}

void DAQManager::setDeviceSelectorOptions(const vector<string> &entries) {

    dataSourcePanel->setDeviceSelectorOptions(entries);

}

void DAQManager::setConfFileSelectorEntry(const string &entry) {

    confFileSelector->setEntry(entry);

}

void DAQManager::setFileDataSource() {

    dataSourcePanel->setFileSource();

}

void DAQManager::setDeviceDataSource() {

    dataSourcePanel->setDeviceSource();

}

void DAQManager::update(const UpdatePacket &packet) {

    viewer->update(packet);

    statusTag->SetText(packet.statusTag.data());

    statusTag->Resize(statusTag->GetDefaultSize());
    
}

void DAQManager::disable() {

    disableStartButton    ();
    disableStopButton     ();
    disableExitButton     ();
    disableDataSourcePanel();

}

void DAQManager::disableStartButton     () { startButton->SetEnabled(false); }
void DAQManager::enableStartButton      () { startButton->SetEnabled(true ); }

void DAQManager::disableStopButton      () { stopButton->SetEnabled (false); }
void DAQManager::enableStopButton       () { stopButton->SetEnabled (true ); }

void DAQManager::disableExitButton      () { exitButton->SetEnabled (false); }
void DAQManager::enableExitButton       () { exitButton->SetEnabled (true ); }

void DAQManager::disableConfFileSelector() { confFileSelector->disable()   ; }
void DAQManager::enableConfFileSelector () { confFileSelector->enable()    ; }

void DAQManager::disableDataSourcePanel () { dataSourcePanel->disable()    ; }
void DAQManager::enableDataSourcePanel  () { dataSourcePanel->enable ()    ; }

// TODO: Use these
void DAQManager::pressedStart() { Emit("pressedStart()"); }
void DAQManager::pressedStop () { Emit("pressedStop()" ); }
void DAQManager::pressedExit () { Emit("pressedExit()" ); }

void DAQManager::handlePressStart() {

    pressedStart();

}

void DAQManager::handlePressStop() {

    pressedStop();

}

void DAQManager::handlePressExit() {

    pressedExit();

}