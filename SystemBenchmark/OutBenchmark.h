#pragma once

#include "Benchmark.h"

static const unsigned long NumWrittenChars = 500000;

class CoutBySingleCharBenchmark : public Benchmark
{
public:
	CoutBySingleCharBenchmark(const std::string& name) : Benchmark(name) {}

	virtual void Run()
	{
		BeginMeasure();
		for (int i = 0; i < NumWrittenChars; i++)
		{
			std::cout << ' ';
		}
		EndMeasure();
		std::cout << '\n';
	}
private:
};

class CoutByWholeStringBenchmark : public Benchmark
{
public:
	CoutByWholeStringBenchmark(const std::string& name) : Benchmark(name) {}

	virtual void Run()
	{
		std::string str;
		str.resize(NumWrittenChars);
		for (char& c : str)
			c = ' ';

		BeginMeasure();
		std::cout << str;
		EndMeasure();
		std::cout <<  '\n';
	}
private:
};

class PrintBySingleCharBenchmark : public Benchmark
{
public:
	PrintBySingleCharBenchmark(const std::string& name) : Benchmark(name) {}

	virtual void Run()
	{
		BeginMeasure();
		for (int i = 0; i < NumWrittenChars; i++)
		{
			printf_s("%c", ' ');
		}
		EndMeasure();
		std::cout << '\n';
	}
private:
};

class PrintByWholeStringBenchmark : public Benchmark
{
public:
	PrintByWholeStringBenchmark(const std::string& name) : Benchmark(name) {}

	virtual void Run()
	{
		std::string str;
		str.resize(NumWrittenChars);
		for (char& c : str)
			c = ' ';

		BeginMeasure();
		printf_s("%s", str);
		fflush(stdout);
		EndMeasure();
		std::cout << '\n';
	}
private:
};

#pragma optimize("", off)

class ConcatSingleCharsAndPrintBenchmark : public Benchmark
{
public:
	ConcatSingleCharsAndPrintBenchmark(const std::string& name) : Benchmark(name) {}

	virtual void Run()
	{
		std::string str;
		//str.resize(NumWrittenChars);
		BeginMeasure();
		for (unsigned int i = 0; i < NumWrittenChars; i++)
			str.push_back(' ');

		printf_s("%s", str);
		EndMeasure();
		std::cout << '\n';
	}
private:
};

#pragma optimize("", on)