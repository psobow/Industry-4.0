#pragma once
#include <assert.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
#include <list>

using namespace std;
template <typename Type>
class Matrix
{
public:
	Matrix(int Columns = 1, int Rows = 1)
		:
		Columns(Columns), Rows(Rows)
	{
		MatPtr.resize(Columns * Rows);
	}

	void resize(int Columns_ = 1, int Rows_ = 1)
	{
		Columns = Columns_;
		Rows = Rows_;

		MatPtr.resize(Columns * Rows);

	}
	// kopiowanie Matrixy po prawej stronie
	void operator =(const Matrix rhs)
	{
		Columns = rhs.Columns;
		Rows = rhs.Rows;

		MatPtr.resize(Columns * Rows);
		for (int y = 0; y < Rows; y++)
		{
			for (int x = 0; x < Columns; x++)
			{
				MatPtr[y * Columns + x] = rhs.MatPtr[y * Columns + x];
			}
		}
	}

	Type GetRowMax(int i)
	{

		Type Max=0;

		for (int j = 0; j < Columns; j++)
		{
			if (Max < MatPtr[i * Columns + j])
			{
				Max = MatPtr[i * Columns + j];
			}
		}
		return Max;
	}

	int GetRowMaxIndex(int i,  const list<int> CheckList)
	{

		Type Max = std::numeric_limits<Type>::min();;
		int index = 0;
		for (int j = 0; j < Columns; j++)
		{
			if (Max < MatPtr[i * Columns + j])
			{
				if (find(CheckList.cbegin(), CheckList.cend(), j) != CheckList.cend())
				{
					Max = MatPtr[i * Columns + j];
					index = j;
				}
			}
		}
		return index;
	}

	int GetRowMinIndex(int i, const list<int> CheckList)
	{

		Type Min = std::numeric_limits<Type>::max();
		int index = 0;
		for (int j = 0; j < Columns; j++)
		{
			if (Min > MatPtr[i * Columns + j] && MatPtr[i * Columns + j] !=0)
			{
				if (find(CheckList.cbegin(), CheckList.cend(), j) != CheckList.cend())
				{
					Min = MatPtr[i * Columns + j];
					index = j;
				}
			}
		}
		return index;
	}

	double& operator[](int index)
	{
		return MatPtr[index];
	}
	// dodawanie Matrixy i zwracanie wyniku w postaci Matrixy
	Matrix operator+(const Matrix& rhs)
	{
		assert(Columns == rhs.Columns && Rows == rhs.Rows);
		
			Matrix out(Columns, Rows);
			for (int i = 0; i < Rows * Columns; i++)
			{
				out[i] = MatPtr[i] + rhs.MatPtr[i];
			}
			return out;
	}
	// dodawanie Matrixy I zapisywanie dzialania w obiekcie wywolujacym
	void operator+=(const Matrix& rhs)
	{
		assert(Columns == rhs.Columns && Rows == rhs.Rows);
		
			for (int i = 0; i < Rows * Columns; i++)
			{
				MatPtr[i] += rhs.MatPtr[i];
			}

	}
	// odejmowanie Matrixy i zwracanie wyniku w postaci Matrixy
	Matrix operator-(const Matrix& rhs)
	{
		assert(Columns == rhs.Columns && Rows == rhs.Rows);
		
			Matrix out(Columns, Rows);
			for (int i = 0; i < Rows * Columns; i++)
			{
				out[i] = MatPtr[i] - rhs.MatPtr[i];
			}
			return out;
	;
	}
	// odejmowanie Matrixy I zapisywanie dzialania w obiekcie wywolujacym
	void operator-=(const Matrix& rhs)
	{
		assert(Columns == rhs.Columns && Rows == rhs.Rows);
		
			for (int i = 0; i < Rows * Columns; i++)
			{
				MatPtr[i] -= rhs.MatPtr[i];
			}
		
	}
	// zmniejszenie wszystkich wartosci Matrixy o 1
	Matrix operator--(int)
	{
		for (int i = 0; i < Rows * Columns; i++)
		{
			MatPtr[i]--;
		}
		return *this;
	}
	Matrix operator++(int)
	{
		for (int i = 0; i < Rows * Columns; i++)
		{
			MatPtr[i]++;
		}
		return *this;
	}
	Matrix operator*(const Matrix& rhs)
	{
		assert(Columns != rhs.Rows);
	
			Matrix out(Rows, rhs.Columns);
			for (int line1 = 0; line1 < Rows; line1++)
			{
				for (int line2 = 0; line2 < rhs.Columns; line2++)
				{
					for (int i = 0; i < rhs.Rows; i++)
					{
						out[line1 * rhs.Columns + line2] += MatPtr[line1 * Columns + i] * rhs.MatPtr[i * rhs.Columns + line2];
					}
				}
			}
			return out;
		
	}
	bool operator==(const Matrix& rhs)
	{
		assert(Columns == rhs.Columns && Rows == rhs.Rows);
	
			for (int i = 0; i < Rows * Columns; i++)
			{
				if (MatPtr[i] != rhs.MatPtr[i]) return false;
			}
			return true;
		
	}
	void ShowValues()
	{
		cout << "--------------------------------------------------------- \n";
		for (int i = 0; i < Rows; i++)
		{
			cout << "|";
			for (int g = 0; g < Columns; g++)
			{
				int index = i * Columns + g;
				double wartosc = MatPtr[index];
				cout << "   " << MatPtr[index];
			}
			cout << "|\n";
		}
	}
	void SetValue(int i, int j, Type Val)
	{
		MatPtr[i * Columns + j] = Val;
	}
	double GetValue(int i, int j)
	{
		int index = i * Columns + j;
		double wartosc = MatPtr[index];
		return wartosc;
	}
private:
	vector<Type> MatPtr;
	int Columns;
	int Rows;
};

