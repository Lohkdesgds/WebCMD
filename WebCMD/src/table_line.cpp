#include "table_line.h"

#include <chrono>
#include <iomanip>
#include <sstream>

std::atomic_uint64_t TableLine::m_counter_ref;

static std::string _make_time_string()
{
	const auto now = std::chrono::system_clock::now();

	const std::time_t tt = std::chrono::system_clock::to_time_t(now);
	std::tm tm;

#ifdef _WIN32
	_localtime64_s(&tm, &tt);
#else
	localtime_r(&tt, &tm);
#endif

	std::stringstream ss;
	ss << std::put_time(&tm, "%Y-%m-%d %X");
	return ss.str();
}

TableLine::TableLine(TableLine&& o) noexcept 
	: m_creation_time(std::move(o.m_creation_time)), m_log_text(std::move(o.m_log_text)), m_source_id(o.m_source_id), m_counter(o.m_counter)
{
}

TableLine::TableLine(const uint64_t id, const std::string& text)
	: m_creation_time(_make_time_string()), m_log_text(text), m_source_id(id), m_counter(m_counter_ref++)
{
}

uint64_t TableLine::get_index() const
{
	return m_counter;
}

std::string TableLine::to_JSON() const
{
	return "{\"date\":\"" + m_creation_time + "\",\"thread\":" + std::to_string(m_source_id) + ",\"text\":\"" + m_log_text + "\",\"real_idx\":" + std::to_string(m_counter) + "}";
}
