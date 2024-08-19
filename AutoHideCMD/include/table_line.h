#pragma once

#include <string>
#include <atomic>

class TableLine {
	std::string m_creation_time;
	std::string m_log_text;
	uint64_t m_source_id;
	uint64_t m_counter;
	static std::atomic_uint64_t m_counter_ref;
public:
	TableLine(TableLine&&) noexcept;
	TableLine(const uint64_t, const std::string&);

	uint64_t get_index() const;
	std::string to_JSON() const;
};