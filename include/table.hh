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
private:
	std::vector<std::string> headers;
	std::string* body;
	size_t numRows;
	size_t numCols;
};
