#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <algorithm>
#include <string>
#include <climits>
#include <limits>
using namespace std;

class TSPPlanner {
public:
    vector<string> locations;
    unordered_map<string, unordered_map<string, double> > distanceMatrix;
    unordered_map<string, unordered_map<string, double> > timeMatrix;

    TSPPlanner(vector<string> locs,
               unordered_map<string, unordered_map<string, double> > distMat,
               unordered_map<string, unordered_map<string, double> > timeMat) {
        locations = locs;
        distanceMatrix = distMat;
        timeMatrix = timeMat;
    }

    double calculatePathDistance(const vector<int> &pathIdx) {
        double total = 0;
        for (size_t i = 0; i + 1 < pathIdx.size(); ++i) {
            total += distanceMatrix[locations[pathIdx[i]]][locations[pathIdx[i + 1]]];
        }
        return total;
    }

    double calculatePathTime(const vector<int> &pathIdx) {
        double total = 0;
        for (size_t i = 0; i + 1 < pathIdx.size(); ++i) {
            total += timeMatrix[locations[pathIdx[i]]][locations[pathIdx[i + 1]]];
        }
        return total;
    }

    void findEfficientRoute() {
        int n = locations.size();
        vector<int> perm(n);
        for (int i = 0; i < n; ++i) perm[i] = i;

        double minDist = numeric_limits<double>::max();
        double minTime = 0;
        vector<int> bestPath;

        do {
            double dist = calculatePathDistance(perm);
            if (dist < minDist) {
                minDist = dist;
                minTime = calculatePathTime(perm);
                bestPath = perm;
            }
        } while (next_permutation(perm.begin(), perm.end()));

        cout << "\nOptimized Visit Order:" << endl;
        for (int i = 0; i < (int)bestPath.size(); ++i) {
            cout << i + 1 << ". " << locations[bestPath[i]] << endl;
        }
        cout << "Total Distance: " << minDist << " km" << endl;
        cout << "Estimated Time: " << minTime << " mins" << endl;
    }
};

unordered_set<string> listAvailableCities(const string &filename) {
    ifstream infile(filename.c_str());
    unordered_set<string> cities;
    string line, city, place;
    while (getline(infile, line)) {
        stringstream ss(line);
        getline(ss, city, ',');
        cities.insert(city);
    }
    return cities;
}

vector<string> loadPlacesForCity(const string &cityName) {
    ifstream infile("places_all.txt");
    vector<string> places;
    string line, city, place;
    while (getline(infile, line)) {
        stringstream ss(line);
        getline(ss, city, ',');
        getline(ss, place, ',');
        if (city == cityName) {
            places.push_back(place);
        }
    }
    return places;
}

void loadDistanceAndTimeMatrixForCity(
    const string &cityName,
    unordered_map<string, unordered_map<string, double> > &distMat,
    unordered_map<string, unordered_map<string, double> > &timeMat) {

    ifstream infile("distances_all.csv");
    string line, city, src, dst, distStr, timeStr;

    getline(infile, line); // skip header
    while (getline(infile, line)) {
        stringstream ss(line);
        getline(ss, city, ',');
        getline(ss, src, ',');
        getline(ss, dst, ',');
        getline(ss, distStr, ',');
        getline(ss, timeStr, ',');

        if (city == cityName) {
            double dist = atof(distStr.c_str());
            double time = atof(timeStr.c_str());
            distMat[src][dst] = dist;
            timeMat[src][dst] = time;
        }
    }
}

int main() {
    cout << "Welcome to the Indian City Travel Planner\n" << endl;

    unordered_set<string> cities = listAvailableCities("places_all.txt");
    cout << "Available Cities: " << endl;
    int count = 1;
    // for (const auto &c : cities) {
    //     cout << count++ << ". " << c << endl;
    // }
    unordered_set<string>::iterator it;
    for (it = cities.begin(); it != cities.end(); ++it) {
    cout << count++ << ". " << *it << endl;
     }

    cout << "\nEnter a city name exactly as shown: ";
    string city;
    getline(cin, city);

    vector<string> cityPlaces = loadPlacesForCity(city);
    if (cityPlaces.empty()) {
        cout << "\nCity not found in dataset." << endl;
        return 0;
    }

    cout << "\nFamous Places in " << city << ":" << endl;
    for (int i = 0; i < (int)cityPlaces.size(); ++i) {
        cout << i + 1 << ". " << cityPlaces[i] << endl;
    }

    cout << "\nEnter the serial numbers of places you want to visit (space-separated): ";
    string input;
    getline(cin, input);
    stringstream ss(input);
    set<int> indices;
    int idx;
    while (ss >> idx) {
        if (idx >= 1 && idx <= (int)cityPlaces.size()) {
            indices.insert(idx);
        }
    }

    if (indices.size() < 2 || indices.size() > 8) {
        cout << "\nPlease enter between 2 and 8 valid, non-repeating indices." << endl;
        return 0;
    }

    vector<string> selected;
    set<int>::iterator it2;
  for (it2 = indices.begin(); it2 != indices.end(); ++it2) {
    selected.push_back(cityPlaces[*it2 - 1]);
  }


    unordered_map<string, unordered_map<string, double> > distanceMatrix;
    unordered_map<string, unordered_map<string, double> > timeMatrix;
    loadDistanceAndTimeMatrixForCity(city, distanceMatrix, timeMatrix);

    TSPPlanner planner(selected, distanceMatrix, timeMatrix);
    planner.findEfficientRoute();

    return 0;
}
