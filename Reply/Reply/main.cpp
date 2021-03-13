#include<iostream>
#include<algorithm>
#include<vector>
#include<queue>
#include<string>

using namespace std;

class Antena
{
public:
	Antena(int r, int s, int id) : range(r), speed(s), id(id) {}
	int id;
	int range;
	int speed;
	pair<int, int> pos;
};

class Building
{
public:
	Building(int x, int y, int l, int c) : latency(l), connSpeed(c) {
		pos.first = x;
		pos.second = y;
	}
	int latency;
	int connSpeed;
	pair<int, int> pos;
};

int W, H;
int N, M, R;
//input input
vector<Antena> ants;
vector<Antena> resultAnts;
vector<Building> builds;

void input(const char* fileName) {
	FILE* in;
	in = fopen(fileName, "r");
	fscanf(in, "%d%d", &W, &H);
	fscanf(in, "%d%d%d", &N, &M, &R);


	int x, y, l, c;
	int r, ac;
	for (int i = 0; i < N; ++i) {
		fscanf(in, "%d%d%d%d", &x, &y, &l, &c);
		Building bPrim(x, y, l, c);
		builds.push_back(bPrim);
	}
	for (int i = 0; i < M; ++i) {
		fscanf(in, "%d%d", &r, &ac);
		Antena aPrim(r, ac, i);
		ants.push_back(aPrim);
	}

	fclose(in);
}

void output(const char* fileName) {
	FILE* out;
	out = fopen(fileName, "w");
	fprintf(out, "%d", resultAnts.size());

	for (int i = 0; i < resultAnts.size(); i++) {
		fprintf(out, "\n%d %d %d", resultAnts[i].id, resultAnts[i].pos.first, resultAnts[i].pos.second);
	}
}

struct Compare {
	bool operator()(Building& a, Building& b)
	{
		return a.pos.first + a.pos.second >= b.pos.first + b.pos.second;
	}
};

struct GridCompare {
	bool operator()(pair<priority_queue<Building, vector<Building>, Compare>, pair<int, int>>& a, pair<priority_queue<Building, vector<Building>, Compare>, pair<int, int>>& b)
	{
		return true;// a.first.size() >= b.first.size();
	}
};

struct CompareAnts {
	bool operator()(Antena& a, Antena& b)
	{
		return a.speed >= b.speed;
	}
};

void SortAntena(vector<Antena>& ants)
{
	sort(ants.begin(), ants.end(),
		[](const Antena& a, const Antena& b)
	{
		return a.range > b.range;
	});
}

void SortAntenaSpeed(vector<Antena>& ants)
{
	sort(ants.begin(), ants.end(),
		[](const Antena& a, const Antena& b)
	{
		return a.speed >= b.speed;
	});
}

int FindGridWidth(vector<Antena> ants)
{
	return ants[0].range;
	//or make median
}

void MakeConnectivityQueue(vector<Building> & builds)
{
	sort(builds.begin(), builds.end(),
		[](const Building& a, const Building& b)
	{
		return a.connSpeed > b.connSpeed;
	});
}

void main()
{
	input("data_scenarios_a_example.in");

	SortAntena(ants);
	int gridSize = FindGridWidth(ants);
	MakeConnectivityQueue(builds);
	int maxX, maxY;
	maxX = H / gridSize + 1;
	maxY = W / gridSize + 1;

	//gdrid and pq grid creation
	vector<vector<priority_queue<Building, vector<Building>, Compare>>> grid(maxX, vector<priority_queue<Building, vector<Building>, Compare>>(maxY) );

	priority_queue<pair<priority_queue<Building, vector<Building>, Compare>, pair<int,int>>, vector<pair<priority_queue<Building, vector<Building>, Compare>, pair<int, int>>>, GridCompare> pqGrid ;



	for (size_t i = 0; i < builds.size(); i++)
	{ 
		int x = (builds[i].pos.first-1) / gridSize;
		int y = (builds[i].pos.second-1) / gridSize;
		grid[(builds[i].pos.first-1) / gridSize][(builds[i].pos.second-1) / gridSize].push(builds[i]) ;
	}

	for (size_t i = 0; i < maxX; i++)
		for (size_t j = 0; j < maxY; j++)
			{
			if (!grid[i][j].empty())
			{
				pair<priority_queue<Building, vector<Building>, Compare>, pair<int, int>> x;
				x.first = grid[i][j];
				x.second.first = i;
				x.second.second = j;
				pqGrid.push(x);
			}
			}
	while (!pqGrid.empty())
	{
		pair<priority_queue<Building, vector<Building>, Compare>, pair<int, int>> field = pqGrid.top();
		pqGrid.pop();
		Antena currAnt = ants.back();
		ants.pop_back();
		int antRange = currAnt.range * 2;
		pair<int, int> firstCords, lastCords;
		firstCords.first = -1;
		firstCords.second = -1;
		Building currBuild(1,1,1,1);
		while (!field.first.empty())
		{
			int distance;
			pair<int, int> cords;
			currBuild = field.first.top();
			field.first.pop();
			if (firstCords.first == -1)
			{
				firstCords = currBuild.pos;
				continue;
			}
			cords = currBuild.pos;
			distance = cords.first + cords.second - firstCords.first - firstCords.second;
			if (distance > antRange)
			{
				pair<int, int> position;
				position.first = (firstCords.first + lastCords.first)/2;
				position.second = (firstCords.second + lastCords.second+1)/2;
				currAnt.pos = position;
				resultAnts.push_back(currAnt);
				firstCords = cords;
				lastCords.first = -1;
				lastCords.second = -1;

				currAnt = ants.back();
				ants.pop_back();
				int antRange = currAnt.range * 2;

				continue;
			}
			lastCords = cords;
		}
		if (lastCords.first != -1)
		{
			pair<int, int> position;
			position.first = (firstCords.first + lastCords.first) / 2;
			position.second = (firstCords.second + lastCords.second + 1) / 2;
			currAnt.pos = position;
			resultAnts.push_back(currAnt);
			lastCords.first = -1;
			lastCords.second = -1;
		}
	}

	SortAntenaSpeed(ants);
	for (int i = 0; i < min(ants.size(), builds.size()) ;i++) 
	{
		ants[i].pos.first = builds[i].pos.first;
		ants[i].pos.second = builds[i].pos.second;
	}
	for (int i = 0; i < ants.size(); i++)
	{
		resultAnts.push_back(ants[i]);
	}
	

	//output
	char* outFileName = new char[1000];
	strcpy(outFileName, "solution_");
	strcat(outFileName, "data_scenarios_a_example.in");

	output(outFileName);
}






















