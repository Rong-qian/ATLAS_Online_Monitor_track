cmake_minimum_required(VERSION 2.8.12)
project(DAQManager)

# Point CMAKE at the project directory when it looks for e.g. FindPCAP.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/")

#
# Include the ROOT library
#

# If compilation fails with undefined references to variables from the STL,
# check that this matches the version ROOT was built with.
message(Using\ g++\ version\ ${CMAKE_CXX_COMPILER_VERSION})

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree Net Gui GuiHtml)
find_package(PCAP REQUIRED)

message(ROOT\ include\ directory:\ ${ROOT_INCLUDE_DIRS})
include_directories(${ROOT_INCLUDE_DIRS})


#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})
include_directories(${ROOT_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

###############################################################################

set(recoObjs_HEADERS
	MuonReco/RecoObject.h
	MuonReco/Cluster.h
	MuonReco/Event.h
	MuonReco/EventID.h
	MuonReco/Geometry.h
	MuonReco/Hit.h
	MuonReco/Observable.h
	MuonReco/Signal.h
	MuonReco/Track.h
)

set(recoObjs_SOURCES
	src/reco/Cluster.cxx
	src/reco/Event.cxx
	src/reco/EventID.cxx
	src/reco/Geometry.cxx
	src/reco/Hit.cxx
	src/reco/Observable.cxx
	src/reco/Signal.cxx
	src/reco/Track.cxx
)

set(p2Objs_HEADERS 
	MuonReco/RecoObject.h
	MuonReco/RecoUtility.h
	MuonReco/IOUtility.h
	MuonReco/EventID.h
	MuonReco/ArgParser.h
	MuonReco/ConfigParser.h
	MuonReco/ParameterSet.h
	MuonReco/ConfigParser.h
	MuonReco/ArgParser.h
	MuonReco/TimeCorrection.h
	MuonReco/Hit.h
	MuonReco/Signal.h
	MuonReco/Cluster.h
	MuonReco/Event.h
	MuonReco/Track.h
	MuonReco/Geometry.h
)

set(p2Objs_SOURCES
	src/reco/RecoUtility.cxx
	src/reco/IOUtility.cxx
	src/reco/EventID.cxx
	src/reco/ArgParser.cxx
	src/reco/ConfigParser.cxx
	src/reco/ParameterSet.cxx
	src/reco/ConfigParser.cxx
	src/reco/TimeCorrection.cxx
	src/reco/Hit.cxx
	src/reco/Signal.cxx
	src/reco/Cluster.cxx
	src/reco/Event.cxx
	src/reco/Track.cxx
	src/reco/Geometry.cxx
)

set(p2Core_HEADERS
	Logging/ErrorLogger.h
)

set(p2Core_SOURCES
	Logging/ErrorLogger.cpp
)

set(p2ecap_HEADERS
	EthernetCapture/src/DeviceManager.h
	EthernetCapture/src/DeviceSelector.h
	EthernetCapture/src/NetworkDeviceException.h
	EthernetCapture/src/PCapDevice.h
	EthernetCapture/src/PCapSessionHandler.h
)

set(p2ecap_SOURCES
	EthernetCapture/src/DeviceManager.cpp
	EthernetCapture/src/DeviceSelector.cpp
	EthernetCapture/src/NetworkDeviceException.cpp
	EthernetCapture/src/PCapDevice.cpp
	EthernetCapture/src/PCapSessionHandler.cpp
)

set(p2Decode_HEADERS 
	Decoder/src/Decoder.h
	Decoder/src/EventDecoding.h
	Decoder/src/SignalDecoding.h
	Decoder/src/EventBufferValidator.h
)

set(p2Decode_SOURCES
	Decoder/src/Decoder.cpp
	Decoder/src/EventDecoding.cpp
	Decoder/src/SignalDecoding.cpp
	Decoder/src/EventBufferValidator.cpp
)

set(monitor_HEADERS
	SignalHandlers.h
	GUI/BuildGUI.h
	GUI/Core/UIException.h
	GUI/Core/UILock.h
	GUI/Core/UISignals.h
	GUI/Core/UILoop.h
	GUI/Core/AlertBox/AlertOperations.h
	GUI/Components/DAQManager.h
	GUI/Components/ConfFileSelector.h
	GUI/Components/DataSourcePanel.h
	GUI/Components/DeviceSelectorPanel.h
	GUI/Components/FileSelectorPanel.h
	GUI/Components/RunView.h
	GUI/Components/PairTable.h
	GUI/Components/Menu.h
	GUI/Components/Submenu.h
	GUI/Components/ProgressBar.h
	GUI/Tabs/TabPanel.h
	GUI/Tabs/UITab.h
	GUI/Tabs/Displays/CanvasTab.h
	GUI/Tabs/Displays/PlotWindow.h
	GUI/Tabs/Displays/GraphPlotter.h
	GUI/Tabs/Displays/HistogramPlotter.h
	GUI/Tabs/Displays/ErrorView.h
	GUI/Tabs/Displays/EventDisplay.h
	GUI/Tabs/Displays/ResidualsDisplay.h
	GUI/Tabs/Displays/EfficiencyDisplay.h
	DAQMonitor/DAQState.h
	DAQMonitor/DataRunOperations.h
	DAQMonitor/LockableStream.h
	DAQMonitor/PlotSavingOperations.h
	DAQMonitor/PacketDecodingOperations.h
	DAQMonitor/DataCaptureOperations.h
	DAQMonitor/DataModel/DAQData.h
	DAQMonitor/DataModel/Plots.h
	DAQMonitor/ProgramControl/SigHandlers.h
	DAQMonitor/ProgramControl/Threads.h
	DAQMonitor/FileManagement/FileManager.h
	analysis/MonitorHooks.h
)

set(monitor_SOURCES
	SignalHandlers.cpp
	GUI/BuildGUI.cpp
	GUI/Core/UIException.cpp
	GUI/Core/UILock.cpp
	GUI/Core/UISignals.cpp
	GUI/Core/UILoop.cpp
	GUI/Core/AlertBox/AlertOperations.cpp
	GUI/Components/DAQManager.cpp
	GUI/Components/ConfFileSelector.cpp
	GUI/Components/DataSourcePanel.cpp
	GUI/Components/DeviceSelectorPanel.cpp
	GUI/Components/FileSelectorPanel.cpp
	GUI/Components/RunView.cpp
	GUI/Components/PairTable.cpp
	GUI/Components/Menu.cpp
	GUI/Components/Submenu.cpp
	GUI/Components/ProgressBar.cpp
	GUI/Tabs/TabPanel.cpp
	GUI/Tabs/UITab.cpp
	GUI/Tabs/Displays/CanvasTab.cpp
	GUI/Tabs/Displays/PlotWindow.cpp
	GUI/Tabs/Displays/GraphPlotter.cpp
	GUI/Tabs/Displays/HistogramPlotter.cpp
	GUI/Tabs/Displays/ErrorView.cpp
	GUI/Tabs/Displays/EventDisplay.cpp
	GUI/Tabs/Displays/ResidualsDisplay.cpp
	GUI/Tabs/Displays/EfficiencyDisplay.cpp
	DAQMonitor/DAQState.cpp
	DAQMonitor/DataRunOperations.cpp
	DAQMonitor/LockableStream.cpp
	DAQMonitor/PlotSavingOperations.cpp
	DAQMonitor/PacketDecodingOperations.cpp
	DAQMonitor/DataCaptureOperations.cpp
	DAQMonitor/DataModel/DAQData.cpp
	DAQMonitor/DataModel/Plots.cpp
	DAQMonitor/ProgramControl/SigHandlers.cpp
	DAQMonitor/ProgramControl/Threads.cpp
	DAQMonitor/FileManagement/FileManager.cpp
	analysis/MonitorHooks.cpp
)

ROOT_GENERATE_DICTIONARY(G__UISignals ${monitor_HEADERS} LINKDEF LinkDef.h)

###############################################################################

add_library(MuonReco ${recoObjs_SOURCES} ${recoObjs_HEADERS})
target_link_libraries(MuonReco ${ROOT_LIBRARIES})

add_library(p2MuonReco ${p2Objs_SOURCES} ${p2Objs_HEADERS})
target_link_libraries(p2MuonReco ${ROOT_LIBRARIES})

add_library(libP2Core ${p2Core_SOURCES} ${p2Core_HEADERS})

add_library(libP2ecap ${p2ecap_SOURCES} ${p2ecap_HEADERS})
target_link_libraries(libP2ecap libP2Core ${PCAP_LIBRARY})

add_library(libP2Decode ${p2Decode_SOURCES} ${p2Decode_HEADERS})
target_link_libraries(libP2Decode libP2Core p2MuonReco)

add_library(libMonitor ${monitor_SOURCES} G__UISignals.cxx)
target_link_libraries(libMonitor libP2Decode libP2ecap)

###############################################################################

add_executable(p2ecap EthernetCapture/Phase2EthernetCapture_Standalone.cpp)
target_link_libraries(p2ecap PUBLIC libP2ecap)

add_executable(p2decode Decoder/Phase2Decode_Standalone.cpp)
target_link_libraries(p2decode PUBLIC libP2Decode)

add_executable(DAQManager StartDAQ.cpp)
target_link_libraries(DAQManager PUBLIC libMonitor)

###############################################################################

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow")