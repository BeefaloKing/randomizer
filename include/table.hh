#pragma once

#include <string>
#include <vector>

class Table
{
public:
	Table();
	Table(const Table &other);
	Table &operator=(const Table &other);
	~Table();

	bool open(std::string);
	void close();

	bool findHeader(const std::string &header, size_t &pos);
	void getColValues(const std::vector<size_t> &rows, size_t col,
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
	std::vector<std::string> headers;
	std::string* body;
	size_t numRows;
	size_t numCols;
};
