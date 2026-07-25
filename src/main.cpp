/*
 * Benchmark AVL and left-leaning red-black tree insert times from a CSV file.
 *
 * The program expects a file named data.csv in the current working directory.
 * It reads the first numeric value from each row, inserts those values into an
 * AVL tree and into a left-leaning red-black tree, and prints the elapsed time
 * for each tree.
 */

#include "avlTree.h"
#include "llrbTree.h"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::string trim(std::string value) {
	const std::string whitespace = " \t\r\n";
	const std::string::size_type begin = value.find_first_not_of(whitespace);
	if (begin == std::string::npos) {
		return "";
	}
	const std::string::size_type end = value.find_last_not_of(whitespace);
	return value.substr(begin, end - begin + 1);
}

std::vector<std::uint32_t> loadCsvValues(const std::string& filePath) {
	std::ifstream input(filePath.c_str());
	if (!input.is_open()) {
		std::ostringstream message;
		message << "unable to open " << filePath;
		throw std::runtime_error(message.str());
	}

	std::vector<std::uint32_t> values;
	std::string line;
	while (std::getline(input, line)) {
		if (line.empty()) {
			continue;
		}

		std::stringstream lineStream(line);
		std::string field;
		while (std::getline(lineStream, field, ',')) {
			field = trim(field);
			if (field.empty()) {
				continue;
			}

			char* end = nullptr;
			const unsigned long parsed = std::strtoul(field.c_str(), &end, 10);
			if (end != field.c_str() && *trim(std::string(end)).c_str() == '\0') {
				values.push_back(static_cast<std::uint32_t>(parsed));
				break;
			}
		}
	}

	if (values.empty()) {
		throw std::runtime_error("data.csv did not contain any numeric values");
	}

	return values;
}

template <typename Tree>
double timeInsert(Tree& tree, const std::vector<std::uint32_t>& values) {
	const auto start = std::chrono::steady_clock::now();
	for (std::vector<std::uint32_t>::const_iterator it = values.begin(); it != values.end(); ++it) {
		if (!tree.insert(*it)) {
			std::ostringstream message;
			message << "duplicate key in data.csv: " << *it;
			throw std::runtime_error(message.str());
		}
	}
	const auto finish = std::chrono::steady_clock::now();
	const std::chrono::duration<double> elapsed = finish - start;
	return elapsed.count();
}

} // namespace

int main() {
	const std::vector<std::uint32_t> values = loadCsvValues("data.csv");

	avlTree<std::uint32_t> avl;
	llrbTree<std::uint32_t> redBlack;

	avl.freedPreallocate(values.size());
	redBlack.freedPreallocate(values.size());

	const double avlSeconds = timeInsert(avl, values);
	const double redBlackSeconds = timeInsert(redBlack, values);

	std::cout << std::fixed << std::setprecision(6);
	std::cout << "rows: " << values.size() << '\n';
	std::cout << "avl insert time: " << avlSeconds << " s" << '\n';
	std::cout << "red-black insert time: " << redBlackSeconds << " s" << '\n';

	return 0;
}

