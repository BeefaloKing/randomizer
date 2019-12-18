#include "table.hh"
#include "utils.hh"
#include <climits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

Table::Table()
{
	body = nullptr;
	numRows = 0;
	numCols = 0;
}

Table::Table(const Table &other)
{
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
			while (line.back() == '\r')
			{
				line.pop_back();
				line.erase(line.size() - 1);
			}
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
			while (isspace(line.back()))
			{
				line.pop_back();
				line.erase(line.size() - 1);
			}
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
		size_t offset = row * numCols + col;
		return body[offset];
	}
	else
	{
		throw std::out_of_range("table out of range");
	}
}
