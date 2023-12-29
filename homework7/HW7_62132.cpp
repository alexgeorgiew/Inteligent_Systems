#include <iostream>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <set>
#include<numeric>

using namespace std;

struct Point
{
	long double x;
	long double y;

	//real point only
	int centroidIndex;

	Point(double x, double y)
	{
		this->x = x;
		this->y = y;
		this->centroidIndex = -1;
	}
};

vector<Point*> points;
vector<Point*> centroids;
set<int> indexesOfPointsCentroids;

vector<double> pointDistances;

const int magicNum = 1000;
int scaledMaxX, scaledMaxY, scaledMinX, scaledMinY;
int maxx, maxy, minx, miny;

int width = 1820, height = 980;

void initX()
{
	auto comparison = [](const Point* a, const Point* b)
	{
		return a->x < b->x;
	};

	auto max = max_element(points.begin(), points.end(), comparison);
	auto min = min_element(points.begin(), points.end(), comparison);
	Point* maxPoint = points[distance (points.begin(), max)];
	Point* minPoint = points[distance(points.begin(), min)];

	maxx = maxPoint->x;
	scaledMaxX = maxPoint->x * magicNum;
	minx = minPoint->x;
	scaledMinX = minPoint->x * magicNum;
}

void initY()
{
	auto comparison = [](const Point* a, const Point* b)
	{
		return a->y < b->y;
	};

	auto max = max_element(points.begin(), points.end(), comparison);
	auto min = min_element(points.begin(), points.end(), comparison);
	Point* maxPoint = points[distance(points.begin(), max)];
	Point* minPoint = points[distance(points.begin(), min)];

	maxy = maxPoint->y;
	scaledMaxY = maxPoint->y * magicNum;
	miny = minPoint->y;
	scaledMinY = minPoint->y * magicNum;
}

void initMinMax()
{
	initX();
	initY();
}

double computeSquaredDistance(Point* a, Point* b)
{
	return (a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y);
}

void initRandomCentroids(int K)
{
	double x, y;
		 
	srand(time(NULL));

	for (int i = 0; i < K; i++)
	{

		x = ((double) (rand() % magicNum) / magicNum) * (scaledMaxX - scaledMinX) + scaledMinX;
		y = ((double)(rand() % magicNum) / magicNum) * (scaledMaxY - scaledMinY) + scaledMinY;

		Point* centroid = new Point(x / magicNum, y / magicNum);
		centroids.push_back(centroid);
	}	
}

double findMinimalDistanceFromPointToCentroid(int pointIndex)
{
	double minDistance = computeSquaredDistance(points[pointIndex], points[0]);
	double curDistance;

	for (int centroidIndex : indexesOfPointsCentroids)
	{
		curDistance = computeSquaredDistance(points[pointIndex], points[centroidIndex]);

		if (curDistance < minDistance)
		{
			minDistance = curDistance;
		}
	}

	return minDistance;
}

// with weighted probability distribution - pointDistances vector
int chooseNewCentroid()
{
	double sumAllPointDistances = accumulate(pointDistances.begin(), pointDistances.end(), 0.0);

	double val = (double)rand() / RAND_MAX;

	double curSum = 0;

	for (int i = 0; i < pointDistances.size(); i++)
	{
		curSum += pointDistances[i];
		if (val < curSum / sumAllPointDistances)
		{
			return i;
		}
	}
}


int findNearestCentroid(Point* p)
{
	double minDistance = computeSquaredDistance(p, centroids[0]);;
	double nearestCentroidIndex = 0;
	double curDistance;

	for (int i = 0; i < centroids.size(); i++)
	{
		curDistance = computeSquaredDistance(p, centroids[i]);
		
		if (curDistance < minDistance)
		{
			minDistance = curDistance;
			nearestCentroidIndex = i;
		}
	}

	return nearestCentroidIndex;
}

bool changeCentroid(int centroidIndex)
{
	long double sumx = 0, sumy = 0;
	int counter = 0;

	for (int i = 0; i < points.size(); i++)
	{
		if (points[i]->centroidIndex == centroidIndex)
		{
			sumx += points[i]->x;
			sumy += points[i]->y;
			counter++;
		}
	}

	//no points assigned to this centroid
	if (counter == 0)
	{
		return false;
	}

	Point* newCentroid = new Point(sumx / counter, sumy / counter);

	Point* oldCentroid = centroids[centroidIndex];

	if (oldCentroid->x == newCentroid->x && oldCentroid->y == newCentroid->y)
	{
		return false;
	}

	centroids[centroidIndex] = newCentroid;

	delete oldCentroid;

	return true;
}

void clusterize()
{
	bool toContinue = true;
	bool isChanged;

	while (toContinue)
	{
		toContinue = false;

		for (int i = 0 ; i < points.size(); i++)
		{
			points[i]->centroidIndex = findNearestCentroid(points[i]);
		}

		for (int j = 0; j < centroids.size(); j++)
		{
			isChanged = changeCentroid(j);
			toContinue = isChanged || toContinue;
		}
	}
}

double fromCartesianToComp(double coord, bool isX) {
	return isX ? (coord - minx) * width / (maxx - minx) : (maxy - coord) * height / (maxy - miny) ;
}

sf::Color getColor(int index)
{
	switch (index)
	{
		case 0: 
			return sf::Color::Red;
		case 1:
			return sf::Color::Blue;
		case 2:
			return sf::Color::Green;
		case 3:
			return sf::Color::Magenta;
		case 4:
			return sf::Color::Yellow;
		case 5:
			return sf::Color::Black;
		case 6:
			return sf::Color::Cyan;
		case 7:
			return  sf::Color(111, 111, 111); // gray
		case 8:
			return  sf::Color(64, 224, 208); // turqoise
		case 9:
			return  sf::Color(148, 0, 211); // purple
		default:
			return  sf::Color(255, 99, 71); // orange
	}
}

void drawKMeans(string title)
{
	sf::RenderWindow window(sf::VideoMode(width, height), title);

	vector<sf::CircleShape> circles;
	double x, y, index;
	for (int i = 0 ; i < points.size(); i++)
	{
		x = fromCartesianToComp(points[i]->x, true);
		y = fromCartesianToComp(points[i]->y, false);
		index = points[i]->centroidIndex;
		sf::CircleShape circle;
		circle.setRadius(5);
		circle.setFillColor(getColor(index));
		circle.setPosition(x, y);
		circles.push_back(circle);
	}

	vector<sf::CircleShape> triangles;
	for (int i = 0; i < centroids.size(); i++)
	{
		x = fromCartesianToComp(centroids[i]->x, true);
		y = fromCartesianToComp(centroids[i]->y, false);
		sf::CircleShape circle;
		sf::CircleShape triangle(10, 3);
		triangle.setFillColor(getColor(i));
		triangle.setPosition(x, y);
		triangles.push_back(triangle);
	}

	sf::Event event;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

		}

		window.clear(sf::Color::White);

		for (int i = 0; i < circles.size(); i++)
		{
			window.draw(circles[i]);
		}

		for (int i = 0; i < triangles.size(); i++)
		{
			window.draw(triangles[i]);
		}

		window.display();
	}
}

void kMeans(int K)
{
	initRandomCentroids(K);
	clusterize();
	drawKMeans("kMeans");
}

int main() 
{
	int K;
	string fname;

	cout << "Enter K:";
	cin >> K;

	cout << endl << "Enter file name:";
	cin >> fname;

	ifstream file(fname);

	//ifstream file("normal.txt");
	//ifstream file("unbalance.txt");

	string line;
	double x, y;
	while (getline(file, line))
	{
		stringstream ss(line);

		ss << line;
		ss >> x >> y;

		Point* p = new Point(x, y);
		
		points.push_back(p);
	}

	file.close();

	initMinMax();

	kMeans(K);
	centroids.clear();
	kMeansPlusPlus(K);
}