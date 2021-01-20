#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include "server.h"
#include "ringbuffer.h"



using StopwatchID = Server::StopwatchIdentifier;
using TrackerID = std::string;
constexpr int PLOT_DATA_SIZE = 1024;


struct TableEntry {
	StopwatchID stopwatch_id;
	std::string name;
	double last_ms;
	double min_ms;
	double max_ms;
	double avg_ms;
	double hz = 0;
	bool plot = false;
	RingBuffer2<double, PLOT_DATA_SIZE> plot_data;
};

class Application {
public:
	Application();
	~Application();
	void ClearTable();
	void Draw();

private:
	void Initialize();
	void Reset();
	void ProcessPackets();
	void SynchronizeData();
	void ClearPlots();

private:
	std::mutex mutex_;
	Server* server_;
	std::thread networking_thread_;
	std::vector<TableEntry> entries_;
	std::atomic_bool shutdown_requested_ = false;

	enum PlotType { SW_Last, SW_Min, SW_Max, SW_Avg, SW_Hz };
	int plot_entry_type_ = SW_Last;
	double tick = 0;
	bool initialized_ = false;


};