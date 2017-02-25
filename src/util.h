#pragma once

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>


namespace horst {

/**
 * contains utility functions.
 */
namespace util {

/**
 * Demangles a symbol name.
 *
 * On failure, the mangled symbol name is returned.
 */
std::string demangle(const char *symbol);

/**
 * Return the demangled symbol name for a given code address.
 */
std::string symbol_name(const void *addr, bool require_exact_addr=true, bool no_pure_addrs=false);


/**
 * Extend a vector with elements, without destroying source one.
 */
template<typename T>
void vector_extend(std::vector<T> &vec, const std::vector<T> &ext) {
	vec.reserve(vec.size() + ext.size());
	vec.insert(std::end(vec), std::begin(ext), std::end(ext));
}

/**
 * Extend a vector with elements with move semantics.
 */
template<typename T>
void vector_extend(std::vector<T> &vec, std::vector<T> &&ext) {
	if (vec.empty()) {
		vec = std::move(ext);
	}
	else {
		vec.reserve(vec.size() + ext.size());
		std::move(std::begin(ext), std::end(ext), std::back_inserter(vec));
		ext.clear();
	}
}


/** convert poll flags to libuv event flags */
int poll_to_libuv_events(int pollflags);

/** Execute shell command and get outputs */
int exec(const std::string& cmd);


}} // horst::util
