#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <string>
#include <math.h>
#define KFACTOR 64
#define ELO_FILE "eloFile.txt"
#define RESULTS_FILE "results.txt"
using namespace std;
unordered_map<string,double> mp;
double calculateElo(double teamOne, double teamTwo, double prevElo, int outcome) {
  double transformedOne = pow(10, teamOne/400);
  double transformedTwo = pow(10, teamTwo/400);
  double transSum = transformedOne + transformedTwo;
  double expOne = transformedOne / transSum;
  double expTwo = transformedTwo / transSum;
  double finalElo = prevElo + KFACTOR * (outcome - expOne);
  return finalElo;
}
vector<string> parseLine(stringstream &s) {
  vector<string> vec;
  string token;
  while(getline(s, token, ' ')) {
    vec.push_back(token);
  }
  int i = 0;

  return vec;
}

array<double, 2> instantiateElo(vector<string> t1, vector<string> t2) {
  array<double,2> fin;
  for(int i = 1; i <= 5; i++) {
    if(mp.find(t1[i]) == mp.end()) {
        mp[t1[i]] = 1000;
    }
    if(mp.find(t2[i]) == mp.end()) {
        mp[t2[i]] = 1000;
    }
    fin[0] += mp[t1[i]];
    fin[1] += mp[t2[i]];
  }
  fin[0] /= 5;
  fin[1] /= 5;
}

void updateElo(array<double, 2> teamElos, vector<string> t1, int teamNum) {
  int outcome = t1[0] == "Loss:" ? 0 : t1[0] == "Win:" ? 1 : .5;
  cout << outcome << endl;
  for(int i = 1; i <= 5; i++) {
    mp[t1[i]] = calculateElo(teamElos[teamNum], teamElos[teamNum == 1 ? 0 : 1], mp[t1[i]], outcome);
  }
}

int main(int argc, char** argv)
{
  ifstream resultsFile;
  string winning;
  string losing;
  resultsFile.open(RESULTS_FILE);
  while(getline(resultsFile, winning)) {
    getline(resultsFile, losing);
    cout << winning << endl;
    cout << losing << endl;
    stringstream win_stream(winning);
    stringstream lose_stream(losing);
    vector<string> winLine = parseLine(win_stream);
  /*  for(int i = 0; i < winLine.size(); i++) {
      cout << winLine[i] << endl;
    } */
    vector<string> loseLine = parseLine(lose_stream);
    array<double, 2> teamElos = instantiateElo(winLine, loseLine);
    updateElo(teamElos, winLine, 0);
    updateElo(teamElos, loseLine, 1);


  }
  for (auto it: mp) {
    cout << it.first << " " << it.second << endl;
  }
  cout << "here" << endl;
  resultsFile.close();
  return 0;

}
