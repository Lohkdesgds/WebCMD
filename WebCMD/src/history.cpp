#include "history.h"

void History::append_log(TableLine&& newline)
{
	std::lock_guard<std::mutex> l(m_mtx);
	m_history.push_back(std::move(newline));
	if (m_history.size() > max_history_size) m_history.pop_front();
	m_lowest_val = m_history[0].get_index();
}

uint64_t History::get_lowest_value() const
{
	return m_lowest_val;
}

std::string History::get_index_autofix(uint64_t i) const
{
	std::lock_guard<std::mutex> l(m_mtx);

	const auto lowest = get_lowest_value();
	if (i < lowest) i = lowest;
	if (i >= m_history.size()) return "{}";

	return m_history[i - lowest].to_JSON();
}

void History::clear()
{
	std::lock_guard<std::mutex> l(m_mtx);
	m_history.clear();
	TableLine::reset_counter();
	m_lowest_val = 0;
}