#pragma once

#include <map>
#include <string>
#include <vector>

namespace FLT
{
	enum FLT
	{
		Contains,
		NotContains
	};
}

class Table
{
public:
	Table();
	Table(const Table &other);
	Table &operator=(const Table &other);
	~Table();

	bool open(std::string);
	void close();

	bool findHeader(const std::string &header, size_t &pos); // Eventually remove
	size_t colAt(const std::string &header);
	void findRows(size_t col, const std::string &lookup, std::vector<size_t> &rows);
	void findMatchRows(size_t col, size_t matchCol, std::vector<size_t> & rows);
	void filterRows(size_t col, const std::string &lookup, std::vector<size_t> &rows,
		FLT::FLT filter);
	void getColValues(const std::vector<size_t> &rows, size_t col,
		std::vector<std::string> &values);
	// Overload expects multiple cols and will return tab delimited entires in values
	void getColValues(const std::vector<size_t> &rows, const std::vector<size_t> &cols,
		std::vector<std::string> &values);
	void print(std::iostream &out);

	std::string &at(size_t row, size_t col);
	inline const size_t &rows()
	{
		return numRows;
	}
	inline const size_t &cols()
	{
		return numCols;
	}
	inline const std::string &name()
	{
		return fileName;
	}
private:
	std::string fileName;
	std::vector<std::string> fileHeaders; // Handles ugly duplicate columns
	std::map<std::string, size_t> headers;
	std::string* body;
	size_t numRows;
	size_t numCols;
};
