#include "application.h"
#include <map>
#include "imgui/imgui.h"
#include "imgui/implot.h"

Application::Application()
{
	Initialize();
}

Application::~Application()
{
	Reset();
}

void Application::ClearTable()
{
	std::lock_guard<std::mutex> lock(mutex_);
	entries_.clear();
}

void Application::Draw()
{
	static int plot_type = SW_Last;
	constexpr float button_height = 20;

	ImGui::Begin("Records");

	const float window_width = ImGui::GetWindowWidth();

	if (ImGui::Button("Flush Cache", { 100, button_height })) {
		server_->Flush();
	}
	ImGui::SameLine();
	if (ImGui::Button("Clear Table", { 100, button_height })) {
		ClearTable();
	}
	ImGui::SameLine();

	ImGui::SetNextItemWidth(100);
	if (ImGui::Combo("Contents", &plot_entry_type_, "Last (ms)\0Min (ms)\0Max (ms)\0Avg (ms)\0Hz\0")) {
		ClearPlots();
	}

	ImGui::Separator();

	constexpr ImGuiTableFlags flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
	if (ImGui::BeginTable("Records", 7, flags, { -1, -1 })) {
		ImGui::TableSetupColumn("Item");
		ImGui::TableSetupColumn("Last (ms)");
		ImGui::TableSetupColumn("Min (ms)");
		ImGui::TableSetupColumn("Max (ms)");
		ImGui::TableSetupColumn("Avg (ms)");
		ImGui::TableSetupColumn("Hz");
		ImGui::TableSetupColumn("Plot");
		ImGui::TableHeadersRow();

		{
			std::lock_guard<std::mutex> lock(mutex_);
			bool all_plots_off = true;
			for (auto& e : entries_) {
				ImGui::PushID(e.stopwatch_id);
				ImGui::PushID(e.name.c_str());
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(e.name.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.2f", e.last_ms);
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("%.2f", e.min_ms);
				ImGui::TableSetColumnIndex(3);
				ImGui::Text("%.2f", e.max_ms);
				ImGui::TableSetColumnIndex(4);
				ImGui::Text("%.2f", e.avg_ms);
				ImGui::TableSetColumnIndex(5);
				ImGui::Text("%.2f", e.hz);
				ImGui::TableSetColumnIndex(6);
				if (e.plot) {
					all_plots_off = false;
				}
				if (ImGui::Checkbox("Plot", &e.plot)) {
					if (e.plot) {
						
						e.plot_data.clear();
					}
				}
				ImGui::PopID();
				ImGui::PopID();
			}

			if (all_plots_off) {
				tick = 0;
			}
		}

		ImGui::EndTable();


	}

	ImGui::End();

	ImGui::Begin("Plots");
	int limit_tick = max(1024, (int)tick);
	ImPlot::SetNextPlotLimitsX(limit_tick - PLOT_DATA_SIZE, limit_tick, ImGuiCond_Always);
	if (ImPlot::BeginPlot("Plots", "Ticks", 0, { -1, -1 })) {
		{
			std::lock_guard<std::mutex> lock(mutex_);
			for (auto& e : entries_) {
				if (e.plot) {
					ImPlot::PlotLine(e.name.c_str(), e.plot_data.t.data(), e.plot_data.x.data(), e.plot_data.size(), e.plot_data.t.offset());
				}
			}
		}
		ImPlot::EndPlot();
	}
	ImGui::End();
}

void Application::Initialize()
{
	if (Server::Create(45454, &server_) != 0) {
		initialized_ = false;
		return;
	};
	initialized_ = true;
	networking_thread_ = std::thread(&Application::ProcessPackets, this);
}

void Application::Reset()
{
	if (server_ != nullptr) {
		delete server_;
	}

	if (initialized_) {
		shutdown_requested_ = true;
		networking_thread_.join();
		initialized_ = false;
	}
}

void Application::ProcessPackets()
{
	while (!shutdown_requested_) {
		bool updated = server_->Update();
		if (updated) {
			std::lock_guard<std::mutex> lock(mutex_);
			tick++;
			SynchronizeData();
		}
	}
}

void Application::ClearPlots()
{
	std::lock_guard<std::mutex> lock(mutex_);
	tick = 0;
	for (auto& e : entries_) {
		e.plot_data.clear();
	}
}



void Application::SynchronizeData()
{
	const auto& cache = server_->cache();
	for (auto it = cache.begin(); it != cache.end(); it++) {

		const auto stopwatch_id = it->first;
		const auto& stopwatch = it->second;

		for (auto it2 = stopwatch.begin(); it2 != stopwatch.end(); it2++)
		{
			const auto& stopwatch_entry = *it2;
			const auto& entry_name = stopwatch_entry.first;
			const auto& entry_data = stopwatch_entry.second;
			const auto last_ms = entry_data[0];
			const auto min_ms = entry_data.getMinimum();
			const auto max_ms = entry_data.getMaximum();
			const auto avg_ms = entry_data.getAverage();
			const auto hz = entry_data.getReciprocal() * 1000.0;

			auto result = std::find_if(entries_.begin(), entries_.end(), [stopwatch_id, entry_name](const TableEntry& entry) { return entry.stopwatch_id == stopwatch_id && entry.name == entry_name; });
			TableEntry* entry;
			if (result == std::end(entries_)) {
				entries_.emplace_back(TableEntry{});
				entry = &entries_.back();
				entry->stopwatch_id = stopwatch_id;
				entry->name = entry_name;
			}
			else {
				entry = &(*result);
			}
			entry->last_ms = last_ms;
			entry->min_ms = min_ms;
			entry->max_ms = max_ms;
			entry->avg_ms = avg_ms;
			entry->hz = hz;

			if (entry->plot) {
				switch (plot_entry_type_) {
				case SW_Last:
				{
					entry->plot_data.push_back(tick, entry->last_ms);
					break;
				}
				case SW_Min:
				{
					entry->plot_data.push_back(tick, entry->min_ms);
					break;
				}
				case SW_Max:
				{
					entry->plot_data.push_back(tick, entry->max_ms);
					break;
				}
				case SW_Avg:
				{
					entry->plot_data.push_back(tick, entry->avg_ms);
					break;
				}
				case SW_Hz:
				{
					entry->plot_data.push_back(tick, entry->hz);
					break;
				}
				}

			}
		}
		}
	}
