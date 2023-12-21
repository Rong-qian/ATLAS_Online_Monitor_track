#include "EntryView.h"

#include "macros/UIFramework/UISignals.h"

#include "MainMenu/EntryOperations.h"

#include "src/ProgramControl/Terminator.h"

using namespace std;
using namespace Muon;

// NOTE: We MUST addFrame every GUI element to ensure proper cleanup

void EntryView::makeConnections() {

    UISignalBus &bus = UISignalBus::getInstance();
    
    bus.Connect("onRunStop()", "EntryView"      , this           , "disableStopButton()");
    bus.Connect("onRunStop()", "EntryView"      , this           , "enableStartButton()");
    bus.Connect("onRunStop()", "DataSourcePanel", dataSourcePanel, "enable()"           );

    bus.Connect("onRunStart()", "EntryView"      , this           , "enableStopButton()"  );
    bus.Connect("onRunStart()", "EntryView"      , this           , "disableStartButton()");
    bus.Connect("onRunStart()", "DataSourcePanel", dataSourcePanel, "disable()"           );

    bus.Connect("onUpdate()", "EntryView", this, "update()");

    exitButton->Connect("Clicked()", "EntryOperations", nullptr, "exitAll()");

    startButton->Connect("Clicked()", "EntryOperations", nullptr, "startRun()");

    stopButton->Connect("Clicked()", "EntryOperations" , nullptr, "stopRun()");

}

EntryView::EntryView(
        const TGWindow *p, 
        int w, 
        int h, 
        int options, 
        Pixel_t back
) : TGCompositeFrame(p, w, h, options, back)/*, viewer(nullptr) */{

    menuBar = new RunMenuBar(this);
    AddFrame(menuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft));

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

                dataSourcePanel = new DataSourcePanel(settings);
                settings->AddFrame(dataSourcePanel);

        // splitter = new TGVSplitter(mainPanel);
        // mainPanel->AddFrame(splitter, new TGLayoutHints(kLHintsExpandY));

        // splitter->SetFrame(leftPanel, true);

        viewport = new TGTab(mainPanel);
        mainPanel->AddFrame(viewport, new TGLayoutHints(kLHintsExpandY));

            // TODO: Base tab can be expanded into an info panel of some sort -- like a 
            //       readme or like the info screens that pop up sometimes talking about e.g.
            //       program title/version/usage
            baseTab = new TGCompositeFrame(viewport, 1250, 850, kFixedSize);
            viewport->AddTab("Home", baseTab);

                baseLabel = new TGLabel(baseTab, "Open tabs from the view menu");
                baseTab->AddFrame(baseLabel, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY));

        // TODO: This is an ugly solution
        menuBar->setTabber(viewport);


    ///////////////////////////////////////////////////////////////////////////

    bottomPanel = new TGHorizontalFrame(this);
    AddFrame(bottomPanel, new TGLayoutHints(kLHintsExpandX));

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

EntryView::~EntryView() {

}

void EntryView::update() {

    viewer->update();
    
}

void EntryView::disableStartButton() { startButton->SetEnabled(false); }
void EntryView::enableStartButton () { startButton->SetEnabled(true ); }

void EntryView::disableStopButton () { stopButton->SetEnabled (false); }
void EntryView::enableStopButton  () { stopButton->SetEnabled (true ); }