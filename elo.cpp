#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <math.h>
#include <algorithm>    // std::sort
#include<cstdlib>
#include<ctime>

#define KFACTOR 64

#define ELO_FILE "eloFile.txt"
#define RESULTS_FILE "results.txt"
#define GAME_FILE "game.txt"

using namespace std;
unordered_map<string,double> mp;

double calculateElo(double teamOne, double teamTwo, double prevElo, int outcome) {
  double transformedOne = pow(10, teamOne/400);
  double transformedTwo = pow(10, teamTwo/400);
  double transSum = transformedOne + transformedTwo;
  double expOne = transformedOne / transSum;
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
  //cout << outcome << endl;
  for(int i = 1; i <= 5; i++) {
    mp[t1[i]] = calculateElo(teamElos[teamNum], teamElos[teamNum == 1 ? 0 : 1], mp[t1[i]], outcome);
  }
}

void make_elo_map(ifstream& file) {
    string line;
    while(getline(file,line)) {
        stringstream stream(line);
        string name;
        string elo;
        getline(stream, name, ' ');
        getline(stream, elo, ' ');
        mp[name] = stod(elo);
    }
}

void updateGame(string winning, string losing) {
    stringstream win_stream(winning);
    stringstream lose_stream(losing);
    vector<string> winLine = parseLine(win_stream);
    vector<string> loseLine = parseLine(lose_stream);

    array<double, 2> teamElos = instantiateElo(winLine, loseLine);
    updateElo(teamElos, winLine, 0);
    updateElo(teamElos, loseLine, 1);
}

unordered_set<string> get_here() {
    ifstream here_file;
    here_file.open("here.txt");
    unordered_set<string> set;
    string name;
    while(getline(here_file, name)) {
        set.insert(name);
    }
    here_file.close();
    return set;
}


int main(int argc, char** argv)
{
  if(argc < 2) {
      cout << "Provide a flag" << endl;
  } else {
      int flag = stoi(argv[1]);
      if(flag == 1){
          ifstream elo_file;
          elo_file.open(ELO_FILE);
          make_elo_map(elo_file);
          elo_file.close();

          ifstream resultsFile;
          string winning;
          string losing;
          resultsFile.open(RESULTS_FILE);
          while(getline(resultsFile, winning)) {
              getline(resultsFile, losing);
              updateGame(winning, losing);
          }

          ofstream output;

          output.open(ELO_FILE);

          for (auto it: mp) {
              string line = it.first + " " + to_string((int)it.second) + "\n";
              output << line;
          }
          output.close();
          resultsFile.close();
      } else if(flag == 2){
          ifstream elo_file;
          elo_file.open(ELO_FILE);

          make_elo_map(elo_file);
          elo_file.close();

          ifstream game_file;
          game_file.open(GAME_FILE);

          string winning, losing;

          getline(game_file, winning);
          getline(game_file, losing);

          updateGame(winning, losing);

          ofstream output;

          output.open(ELO_FILE);

          for (auto it: mp) {
              string line = it.first + " " + to_string((int)it.second) + "\n";
              output << line;
          }
          output.close();

          game_file.close();
      } else {
          srand(time(0));
          ifstream elo_file;
          elo_file.open(ELO_FILE);

          make_elo_map(elo_file);

          unordered_set<string> here = get_here();

          int diff = 200;
          int actualDiff = 10000;
          while(actualDiff > diff) {
              vector<string> v1;
              vector<string> v2;
              double n1 = 0, n2 = 0;
              for(auto it: mp) {
                  if(here.find(it.first) == here.end()) continue;
                  int num = rand();
                  if(v1.size() < 5 && num % 2 == 0) {
                      v1.push_back(it.first);
                      n1 += it.second;
                  } else if (v2.size() < 5){
                      v2.push_back(it.first);
                      n2 += it.second;
                  }
              }

              actualDiff = abs(n2 - n1);
              if(actualDiff < diff) {
                  for(int i = 0; i < v1.size(); ++i) {
                      cout << v1[i] << " ";
                  }

                  printf("\n");

                  for(int i = 0; i < v1.size(); ++i) {
                      cout << v2[i] << " ";
                  }

                  printf("\n");

                  cout << n1 << " " << n2 << endl;
              }
          }

          elo_file.close();
      }
  }
  return 0;

}
