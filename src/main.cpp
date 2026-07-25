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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

string trim(const string& value) {
	const string whitespace = " \t\r\n";
	const string::size_type begin = value.find_first_not_of(whitespace);
	if (begin == string::npos) {
		return "";
	}
	const string::size_type end = value.find_last_not_of(whitespace);
	return value.substr(begin, end - begin + 1);
}

vector<int> loadCsvIds(const string& filePath) {
	ifstream input(filePath.c_str());
	if (!input.is_open()) {
		ostringstream message;
		message << "unable to open " << filePath;
		throw runtime_error(message.str());
	}

	vector<int> ids;
	string line;
	while (getline(input, line)) {
		if (line.empty()) {
			continue;
		}

		stringstream lineStream(line);
		string firstField;
		if (!getline(lineStream, firstField, ',')) {
			continue;
		}

		firstField = trim(firstField);
		if (firstField.empty()) {
			continue;
		}

		istringstream valueStream(firstField);
		int id = 0;
		char extra = '\0';
		if (valueStream >> id && !(valueStream >> extra)) {
			ids.push_back(id);
		}
	}

	if (ids.empty()) {
		throw runtime_error("data.csv did not contain any numeric ids");
	}

	return ids;
}

double benchmarkAvl(const vector<int>& ids) {
	avlTree<int> tree;
	tree.freedPreallocate(ids.size());

	const chrono::steady_clock::time_point start = chrono::steady_clock::now();
	for (vector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (!tree.insert(*it)) {
			ostringstream message;
			message << "duplicate key in data.csv: " << *it;
			throw runtime_error(message.str());
		}
	}
	const chrono::steady_clock::time_point finish = chrono::steady_clock::now();
	return chrono::duration<double>(finish - start).count();
}

double benchmarkRedBlack(const vector<int>& ids) {
	llrbTree<int> tree;
	tree.freedPreallocate(ids.size());

	const chrono::steady_clock::time_point start = chrono::steady_clock::now();
	for (vector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (!tree.insert(*it)) {
			ostringstream message;
			message << "duplicate key in data.csv: " << *it;
			throw runtime_error(message.str());
		}
	}
	const chrono::steady_clock::time_point finish = chrono::steady_clock::now();
	return chrono::duration<double>(finish - start).count();
}

int main() {
	const vector<int> ids = loadCsvIds("data.csv");

	const double avlSeconds = benchmarkAvl(ids);
	const double redBlackSeconds = benchmarkRedBlack(ids);

	cout << fixed << setprecision(6);
	cout << "rows: " << ids.size() << '\n';
	cout << "avl insert time: " << avlSeconds << " s" << '\n';
	cout << "red-black insert time: " << redBlackSeconds << " s" << '\n';

	return 0;
}

