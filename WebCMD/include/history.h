#pragma once

#include "table_line.h"

#include <deque>
#include <mutex>

constexpr uint64_t max_history_size = 100;

class History {
	std::deque<TableLine> m_history;
	mutable std::mutex m_mtx;
	uint64_t m_lowest_val = 0;
public:
	void append_log(TableLine&& newline);

	uint64_t get_lowest_value() const;

	std::string get_index_autofix(uint64_t) const;
};