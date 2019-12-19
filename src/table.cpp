#include "table.hh"
#include "utils.hh"
#include <climits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

Table::Table()
{
	fileName = "";
	body = nullptr;
	numRows = 0;
	numCols = 0;
}

Table::Table(const Table &other)
{
	fileName = other.fileName;
	headers = other.headers;
	numRows = other.numRows;
	numCols = other.numCols;
	body = nullptr;

	if (numRows * numCols > 0)
	{
		body = new std::string[numRows * numCols];
	}
	for (size_t i = 0; i < numRows * numCols; i++)
	{
		body[i] = other.body[i];
	}
}

Table &Table::operator=(const Table &other)
{
	if (this != &other)
	{
		close();
		fileName = other.fileName;
		headers = other.headers;
		numRows = other.numRows;
		numCols = other.numCols;

		if (numRows * numCols > 0)
		{
			body = new std::string[numRows * numCols];
		}
		for (size_t i = 0; i < numRows * numCols; i++)
		{
			body[i] = other.body[i];
		}
	}
	return *this;
}

Table::~Table()
{
	if (body)
	{
		delete[] body;
	}
}

bool Table::open(std::string file)
{
	bool success = true;

	fileName = file;
	std::fstream csv;
	csv.open(file, std::fstream::in);
	if (csv.good())
	{
		std::string line;
		std::stringstream ssLine;
		std::string element;

		std::getline(csv, line, '\n');
		if (line.length() > 0)
		{
			ssLine.str(line);
			while(ssLine.good())
			{
				std::getline(ssLine, element, '\t');
				headers.push_back(element);
			}
		}
		else
		{
			printError(EC::badFormat, file);
			success = false;
		}
		numCols = headers.size();

		while (csv.ignore(INT_MAX, '\n') && !csv.eof())
		{
			numRows++;
		}
		if (numRows * numCols > 0)
		{
			body = new std::string[numRows * numCols];
		}

		// Revert position and populate body
		csv.seekg(0, csv.beg);
		csv.ignore(INT_MAX, '\n');
		size_t row = 0;
		size_t col = 0;
		while (csv.good())
		{
			ssLine.clear();
			std::getline(csv, line, '\n');
			// Skip line if completely empty
			if (line.length() == 0)
			{
				continue;
			}
			ssLine.str(line);

			col = 0;
			while (ssLine.good())
			{
				std::getline(ssLine, element, '\t');
				at(row, col) = element;
				col++;
			}
			row++;
		}
	}
	else
	{
		printError(EC::badAccess, file);
		success = false;
	}
	csv.close();
	return success;
}

void Table::close()
{
	fileName = "";
	headers.clear();
	if (body)
	{
		delete[] body;
		body = nullptr;
	}
	numRows = 0;
	numCols = 0;
}

bool Table::findHeader(const std::string &header, size_t &pos)
{
	for (size_t i = 0; i < numCols; i++)
	{
		if (headers[i] == header)
		{
			pos = i;
			return true;
		}
	}
	return false;
}

void Table::findRows(size_t col, const std::string &lookup, std::vector<size_t> &rows)
{
	for (size_t i = 0; i < numRows; i++)
	{
		if (at(i, col) == lookup)
		{
			rows.push_back(i);
		}
	}
}

void Table::getColValues(const std::vector<size_t> &rows, size_t col,
	std::vector<std::string> &values)
{
	for (size_t i = 0; i < rows.size(); i++)
	{
		values.push_back(at(rows.at(i), col));
	}
}

void Table::getColValues(const std::vector<size_t> &rows, const std::vector<size_t> &cols,
	std::vector<std::string> &values)
{
	for (size_t i = 0; i < rows.size(); i++)
	{
		std::string entries;
		for (size_t j = 0; j < cols.size(); j++)
		{
			entries.append(at(rows.at(i), cols.at(j)));
			if (j < cols.size() - 1)
			{
				entries.append("\t");
			}
		}
		values.push_back(entries);
	}
}

void Table::print(std::iostream &out)
{
	for (size_t h = 0; h < numCols; h++)
	{
		out << headers.at(h);
		if (h < numCols - 1)
		{
			out << "\t";
		}
	}
	out << "\r\n";
	for (size_t r = 0; r < numRows; r++)
	{
		for (size_t c = 0; c < numCols; c++)
		{
			out << at(r, c);
			if (c < numCols - 1)
			{
				out << "\t";
			}
		}
		out << "\r\n";
	}
}

std::string &Table::at(size_t row, size_t col)
{
	if (row < numRows && col < numCols)
	{
		// Consider indexing by col * numRows + row
		size_t offset = row * numCols + col;
		return body[offset];
	}
	else
	{
		throw std::out_of_range("table out of range");
	}
}
