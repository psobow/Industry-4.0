#include <iostream>
#include "Matrix.h"
#include <fstream>
#include  <string>
#include <algorithm>
#include <random>
#include <ctime>
#include <math.h>
#define M_E 2.71828182845904523536
#include <list>
using namespace std;

int MainBase;

struct Node
{
	string Code;
	string City;
	double Lat;
	double Lng;
};

struct Package
{
	Matrix<double> Dist;
	vector<Node> Cities;
};

Package LoadFile(string path)
{
	Package Gift;


	fstream File(path.c_str(), ios::in);
	string Line;
	getline(File, Line);
	int NumberOfCities = stoi(Line);
	Gift.Dist.resize(NumberOfCities, NumberOfCities);
	getline(File, Line);
	string Unit = Line.substr(0, Line.find_first_of(";"));

	int i = 0;
	while (i < NumberOfCities && getline(File, Line))
	{
		replace(Line.begin(), Line.end(), ',', '.');
		int StringOffset = 0;
		for (int j = 0; j < NumberOfCities; j++)
		{
			string Number = Line.substr(StringOffset, Line.find_first_of(";", StringOffset) - StringOffset);
			StringOffset += Number.length() + 1;
			Gift.Dist.SetValue(i, j, stod(Number));
		}

		i++;
	}

	i = 0;
	while (getline(File, Line) && i < NumberOfCities)
	{
		Node city;
		int StringOffset = 0;

		string Code = Line.substr(StringOffset, Line.find_first_of(";", StringOffset) - StringOffset);
		StringOffset += Code.length() + 1;
		city.Code = Code;

		string City = Line.substr(StringOffset, Line.find_first_of(";", StringOffset) - StringOffset);
		StringOffset += City.length() + 1;
		city.City = City;

		string Lat = Line.substr(StringOffset, Line.find_first_of(";", StringOffset) - StringOffset);
		replace(Lat.begin(), Lat.end(), ',', '.');
		StringOffset += Lat.length() + 1;
		city.Lat = stod(Lat);

		string Lng = Line.substr(StringOffset, Line.find_first_of(";", StringOffset) - StringOffset);
		replace(Lng.begin(), Lng.end(), ',', '.');
		StringOffset += Lng.length() + 1;
		city.Lng = stod(Lng);

		Gift.Cities.push_back(city);
		i++;
	}

	File.close();
	return Gift;
}


// Annealing algorithm

double caculateDistance(vector<int> path, Package & M)
{
	double part_dis = 0;
	vector<double> distance;
	double All_distance = 0;

	for (int i = 0; i < path.size(); i++)
	{
		if (i > 0) part_dis = part_dis + M.Dist.GetValue(path[i], path[i - 1]);

		if (path[i] == MainBase) { //mainbase index

			if (i > 0) {
				distance.push_back(part_dis);
				part_dis = 0;
			}
		}		
	}
	for (int j = 0; j < distance.size(); j++) {
		All_distance = All_distance + distance[j];
	}

	return All_distance;
}

double getRandomDouble(double floor, double ceil)
{
	
	std::uniform_real_distribution<double> unif(floor, ceil);
	std::default_random_engine re;
	return unif(re);
}

void initializeRandomGenerator()
{
	srand((unsigned)time(0));
}

int getRandomInt(int floor, int ceil)
{
	int randomNumber = (rand() % (ceil - floor)) + floor;

	return randomNumber;
}

void decreseTemperature(int & temperature)
{
	temperature--;
}

bool examinIfPathValid(vector<int> path)
{
	for (int i = 0; i < path.size() - 1; i++)
	{
		// zabroni� wyst�powania main base 
		if (path[i] == MainBase && path[i + 1] == MainBase)
		{
			return false;
		}
	}
	return true;
}

vector<int> annealing(vector<int> pathInit)
{
	int T = 1000;
	int T0 = 0;
	int mainbase = MainBase;
	auto M = LoadFile("PL.csv");
	int size = pathInit.size();

	//path.remove_if([mainbase](int val) { return MainBase == val; });

	vector<int> currentBestPath(pathInit);
	std::vector<int> pathOld(pathInit);

	while (T > T0)
	{
		for (int k = 0; k < 10; k++)
		{
			// zapamietaj now� kolejno��
			std::vector<int> pathNew(pathOld);

			// Pierwsze ograniczenie 
			// Losuj i oraz J tak d�ugo a� zawarto�� wylosowanego indexu r�ny od MainBase
			// swap
			int i;
			int j;
			do
			{
				i = getRandomInt(1, size - 2); // index pierwszego miasta z wy��czeniem zer na pocz�tku i ko�cu
				j = getRandomInt(1, size - 2); // index drugiego miasta z wy��czeniem zer na pocz�tku i ko�cu
			} while (pathNew[i] == MainBase || pathNew[j] == MainBase);
			

			// What in case if i == j
			std::iter_swap(pathNew.begin() + i, pathNew.begin() +j);

			
			// Przyjmij path z wi�ksz� warto�� Cmax z pewnym prawdopodobie�stwem
			if (caculateDistance(pathNew, M) > caculateDistance(pathOld, M))
			{
				double r = getRandomDouble(0.0, 1.0);
				double s = pow( (M_E) , (caculateDistance(pathOld, M) - caculateDistance(pathNew, M)) / T);
					if (r >= s) 
					{
						//T = T0;
						//break;
						pathNew = vector<int>(pathOld);
					}
			}


			pathOld = vector<int>(pathNew);


			if (caculateDistance(pathOld, M) < caculateDistance(currentBestPath, M))
			{
				// Drugie Ograniczenie
				// Je�eli dwa zera s�si�duj� nie dokonuj podmiany
				if (examinIfPathValid(pathOld))
				{
					currentBestPath = vector<int>(pathOld);
				}
			}
		}
		decreseTemperature(T);
	}
	
	return currentBestPath;
}



vector<int> GreedyVRP(Matrix<double> Dist, vector<Node> & Cities)
{
	int mainbase = MainBase;
	const double DistProp = 2.0 / 3.0;
	int TruckCount = 3;

	const int MaxOrders = ceil((Cities.size() - 1) / (float)TruckCount);
	//const double DistLimit = DistProp * Dist.GetRowMax(MainBase);
	vector<int> Pi;
	//---------------------------------------------------------------------------------

	list<int> A_indexes; // lista index�w miast
	for (int i = 0; i < Cities.size(); i++) A_indexes.push_back(i);

	A_indexes.remove_if([mainbase](int val) { return MainBase == val; });

	int k = 1; // numer ci�ar�wki

	while (!A_indexes.empty())
	{
		Pi.push_back(MainBase);
		int J = Dist.GetRowMaxIndex(MainBase, A_indexes); // najdalszy s�siad index
		// get row index zwraca index najdalszego s�siada
		// znajdujemy najdalszego s�siada.
		// dla niego szukamy jego najbli�szego s�siada i nastepnie znajduje najbli�szego (tyle razy ile jest limit ustawiony).

		Pi.push_back(J);
		A_indexes.remove_if([J](int val) { return J == val; });
		int x_k = 1; // numer kursu ci�ar�wki

		// Szukanie kolejnych najbli�szych s�si�d�w tego znalezionego najdalszego.
		int L = J;
		while (x_k < MaxOrders && !A_indexes.empty())
		{

			L = Dist.GetRowMinIndex(L, A_indexes);

			if (x_k + 1 < MaxOrders)
			{
				Pi.push_back(L);
				A_indexes.remove_if([L](int val) { return L == val; });
			}
			x_k++;
		}
		k++;
	}
	Pi.push_back(MainBase);

	return Pi;
}

vector<int> UserPermutation(vector<Node> & Cities) {
	int index;
	int mainbase;
	cout << "Number of cities : " << Cities.size() << endl;
	for (int i = 0; i < Cities.size(); i++) cout << i + 1 << "\t" << Cities[i].Code << "\t" << Cities[i].City << "\t" << Cities[i].Lat << "\t" << Cities[i].Lng << endl;
	vector<int> Pi;
	cout << "Chose number of base city : ";
	cin >> mainbase;
	MainBase = mainbase - 1;
	// Na pocz�tku wprowadzanej permutacji musi by� 
	cout << "Dont change base index to 0, if you want finish choose '-1'\n" 
		<< "Example: (MainBase = 16) Valid permutation: 16 1 2 3 16 10 11 12 16\n" 
		<< "Enter permutation: ";
	do {
		cin >> index;
		if (index != -1) Pi.push_back(index - 1);
	} while (index != -1);
	return Pi;
}


void printDistance(vector<int> path, Package & M)
{
	vector<double> distance;
	double part_dis = 0;
	double All_distance = 0;
	cout << "Path: ";
	for (int i = 0; i < path.size(); i++)
	{
		if (i > 0) part_dis = part_dis + M.Dist.GetValue(path[i], path[i - 1]);
		if (path[i] == MainBase) { //mainbase index
			cout << "0 ";
			if (i > 0) {
				distance.push_back(part_dis);
				part_dis = 0;
			}
		}
		else {
			cout << path[i] + 1 << " ";
		}
	}
	for (int j = 0; j < distance.size(); j++) {
		cout << endl << "Distance_" << j + 1 << " : " << distance[j] << " ";
		All_distance = All_distance + distance[j];
	}
	cout << endl << "All distnace : " << All_distance;
}



int main()
{
	initializeRandomGenerator();
	// definicja zmiennej globalnej z MainBase na potrzeby GreedyVRP oraz Annealing 
	MainBase = 17;
	vector<int> customPath = { 17, 2, 1, 3,
							17, 10, 11, 12,
							17, 20, 21, 22, 17 };


	auto M = LoadFile("PL.csv");

	auto pathGreedyVRP = GreedyVRP(M.Dist, M.Cities);
	printDistance(pathGreedyVRP, M);

	cout << "\n\n\n\n";
	//auto path = UserPermutation(M.Cities);


	auto pathAnnealing = annealing(pathGreedyVRP);
	printDistance(pathAnnealing, M);

	

	

}



