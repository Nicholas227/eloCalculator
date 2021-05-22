#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <math.h>
#include <algorithm>
#include<cstdlib>
#include<ctime>

#define KFACTOR 16

#define ELO_FILE "eloFile.txt"
#define RESULTS_FILE "results.txt"
#define GAME_FILE "game.txt"

using namespace std;
unordered_map<string,double> mp;
/*
 * Worth = Kills + Assists * 0.333
 *
 * newelo = prevElo +
 *
 * */

bool tie_game = false;

double calculateElo(double teamOne, double teamTwo, double prevElo, int outcome) {
  double transformedOne = pow(10, teamOne/400);
  double transformedTwo = pow(10, teamTwo/400);
  double transSum = transformedOne + transformedTwo;
  double expOne = transformedOne / transSum;
  double finalElo = prevElo + KFACTOR * (outcome - expOne);
  return finalElo;
}

void parseLine(stringstream &s, unordered_map<string, pair<double,int>>& kills, int team) {

  string first;
  getline(s, first, ' ');

  tie_game = first != "Win:" && first != "Loss:" ? true : false;

  string name;
  string w;

  while(getline(s, name, ' ')) {
    getline(s, w, ' ');
    double worth = stod(w);
    kills[name] = {worth, team};
  }
}

array<double, 2> instantiateElo(unordered_map<string, pair<double,int>>& kills) {
  array<double,2> fin;
  for(auto it: kills) {
    string name = it.first;
    if(mp.find(name) == mp.end()) {
        mp[name] = 1000;
    }

    fin[it.second.second] += mp[name];
  }
  fin[0] /= 5;
  fin[1] /= 5;
}

void updateElo(unordered_map<string, pair<double, int>> kills, array<double, 2> fin) {
  unordered_map<string, double> new_elos;
  for(auto p1: kills) {
      double prev_elo = mp[p1.first];
      double p1_worth = p1.second.first;
      double elo_one = pow(10, mp[p1.first]/400);
      for(auto p2: kills) {
          if(p1.first != p2.first) {
              double p2_worth = p2.second.first;
              double elo_two = pow(10, mp[p2.first]/400);
              double transSum = elo_one + elo_two;
              double expOne = elo_one / transSum;
              double s_ab = p1_worth > p2_worth ? 1 : p1_worth == p2_worth ? .5 : 0;
              double elow = s_ab >= .5 ? prev_elo : mp[p2.first];
              double elol = s_ab >= .5 ? mp[p2.first] : prev_elo;
              double personq = 2.2 / ((elow - elol) * 0.001 + 2.2);
              double teamq = 2.2 / ((fin[0] - fin[1]) * 0.001 + 2.2);
              double teammovm = log(abs(p1_worth - p2_worth) + 1) * teamq;
              double movm = log(abs(p1_worth - p2_worth) + 1) * personq;
              /* this line weightes only be individual elo for margin of victory*/
              //prev_elo += KFACTOR * (s_ab - expOne) * movm;
              //this line weights only team elo_one
              prev_elo += KFACTOR * (s_ab - expOne) * teammovm;
              //this line balances the team and individual elo when considering margin of victory
              //prev_elo += KFACTOR * (s_ab - expOne) * ((teammovm + movm) / 2);

//              double exponent = (mp[p1.first] - mp[p2.first]) / 400;
//              double e_ab = 1/(1 + pow(10,exponent));
//              //prev_elo += KFACTOR * (s_ab - e_ab);
          }
      }
      new_elos[p1.first] = prev_elo;
  }

  for(auto it: new_elos) {
      mp[it.first] = it.second;
  }
}

/*
 * double transformedOne = pow(10, teamOne/400);
  double transformedTwo = pow(10, teamTwo/400);
  double transSum = transformedOne + transformedTwo;
  double expOne = transformedOne / transSum;
  double finalElo = prevElo + KFACTOR * (outcome - expOne);
 * */

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
    unordered_map<string, pair<double, int>> kills;
    parseLine(win_stream, kills, 1);
    parseLine(lose_stream, kills, 0);

//    for(auto it: kills) {
//        cout << it.first << " " << it.second.first << " " << it.second.second << endl;
//    }

    array<double, 2> fin = instantiateElo(kills);
    updateElo(kills, fin);
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
//              for(auto it: mp) {
//                  cout << it.first << " " << it.second << endl;
//              }
          }


          ofstream output;

          output.open(ELO_FILE);

          cout << "here" << endl;

          for (auto it: mp) {
              cout << it.first << " " << it.second << endl;
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
