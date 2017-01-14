# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <fstream>
# include <iomanip>
# include <cmath>
# include <ctime>
#include <vector>
#include <string>
#include <algorithm>
#include <bits/stdc++.h>
# include <cstring>

std::vector<char> possibleGenes;
std::string str;
using namespace std;
bool found = true;
int counter = 0;

# define POPSIZE 50
# define MAXGENS 10000
# define NVARS 3
# define PXOVER 0.8
double PMUTATION = 0.15;
double prev_best_val = 0;
struct genotype
{
  std::vector< std::vector<char> > genes;
  double fitness;
  bool operator < (genotype rhs)
  {
    return fitness < rhs.fitness;
  }
};

struct genotype population[POPSIZE+1];
struct genotype newpopulation[POPSIZE+1]; 
int ctr = 0;

int main ( );
void crossover ();
void elitist ( );
void evaluate ( );
int i4_uniform_ab ();
void initialize ();
void keep_the_best ( );
void mutate ();
double r8_uniform_ab ();
void report ( int generation );
void selector ();
void timestamp ( );
void Xover ( int one, int two);

int main ( )
{
  srand (time(NULL));
  str = "h";
  possibleGenes.push_back('+');
  possibleGenes.push_back('-');
  possibleGenes.push_back('[');
  possibleGenes.push_back(']');
  possibleGenes.push_back('.');
  possibleGenes.push_back('+');
  possibleGenes.push_back('<');
  possibleGenes.push_back('>');

  int generation;
  int i;

  timestamp();

  initialize ();

  for(int ii = 0; ii < POPSIZE; ++ii)
  {
    population[ii].fitness = 0;
    for(int j = 0; j < population[ii].genes.size(); ++j)
    {
      string s = "";
      std::vector<char> gene = population[ii].genes[j];
      for(int k = 0; j < gene.size(); ++j)
        s += gene[j];
      ofstream ofs("output.bf", std::ostream::out);
      ofs << s;
      ofs.close();
      string cmd = "timeout -k 0.5 0.5 ./bfinterpreter output.bf > result.txt";
      char result[256];
      std::system(cmd.c_str());
      ifstream ifs("result.txt", std::ifstream::in);
      ifs.getline(result, 256);
      //cout<<"result is : "<<result<<endl;
      string res = result;
      bool success = false;

      if(res.find("Runtime") == std::string::npos)
      {
        int maximum = 0;
        for(int k = 0; k < res.size(); ++k)
          maximum = max(256-abs(res[k] - str[j]) - (k-j)*(k-j) + (64-4*(k-j))*(res[k] == str[j]), maximum);
        maximum += 64*(res.size() == 1);
        population[ii].fitness += maximum;
      }
      else
      {
        res = res.substr(0, res.find("Runtime"));
        int maximum = 0;
        for(int k = 0; k < res.size(); ++k)
          maximum = max(256-abs(res[k] - str[j]) - (k-j)*(k-j) + (64-4*(k-j))*(res[k] == str[j]), maximum);
        maximum += 64*(res.size() == 1);
        population[ii].fitness += maximum;
      }
    }
  }
  cout<<"evaluate good\n";
  for ( generation = 0; generation < MAXGENS; generation++ )
  {
    ctr = 0;
    crossover();
    //cout<<"crossover good\n";
    mutate();
    //cout<<"mutate good\n";
    evaluate();
    //if(found)
      //break;
    //cout<<"evaluate good\n";
    cout<<"PARENTS : \n";
    /*for(int k = 0; k < POPSIZE; ++k)
    {
      cout<<k<<" : ";
      for(int i = 0; i < population[k].genes.size(); ++i)
        cout<<population[k].genes[i];
      cout<<" : "<<population[k].fitness<<endl;
    }
    cout<<"OFFSPRINGS : \n";
    for(int k = 0; k < POPSIZE; ++k)
    {
      cout<<k+POPSIZE<<" : ";
      for(int i = 0; i < newpopulation[k].genes.size(); ++i)
        cout<<newpopulation[k].genes[i];
      cout<<" : "<<newpopulation[k].fitness<<endl;
    }*/
    if(population[POPSIZE-1].fitness >= 3264)
      break;
    char ch;
    //cin>>ch;
    selector();
    //cout<<"selector good\n";
    report (generation);
  }

  cout << "\n";
  cout << "  Best member after " << generation << " generations:\n";
  cout << "\n";
  for(int i = 0; i < population[POPSIZE-1].genes.size(); ++i)
  {
    for(int j = 0; j < population[POPSIZE-1].genes[i].size(); ++j)
      cout<<population[POPSIZE-1].genes[i][j];
    cout<<" ";
  }
  cout << "\n";
  cout << "  Best fitness = " << population[POPSIZE-1].fitness << "\n";

  cout << "\n";
  timestamp ( );

  return 0;
}

void crossover ()
{
  cout<<"crossover strated\n";
  std::deque<int> myList;
  for(int i = 0; i < POPSIZE; ++i)
  {
    myList.push_back(i);
  }
  while(!myList.empty())
  {
    //cout<<"ctr = "<<ctr<<" size = "<<myList.size()<<endl;
    int one = myList.back();
    myList.pop_back();
    int two = myList[rand()%(myList.size())];
    myList.erase(find(myList.begin(), myList.end(), two));
    Xover(one, two);
  }
  //cout<<"ctr = "<<ctr<<endl;
  cout<<"crossover finished\n";
  return;
}

void evaluate ( )
{
  for(int i = 0; i < POPSIZE; ++i)
  {
    newpopulation[i].fitness = 0;
    //cout<<"newpopulation[i].genes.size() = "<<newpopulation[i].genes.size()<<" evaluate started\n";
    for(int j = 0; j < newpopulation[i].genes.size(); ++j)
    {
      string s = "";
      std::vector<char> gene = newpopulation[i].genes[j];
      for(int k = 0; j < gene.size(); ++j)
        s += gene[j];
      ofstream ofs("output.bf", std::ostream::out);
      ofs << s;
      ofs.close();
      string cmd = "timeout -k 0.05 0.05 ./bfinterpreter output.bf > result.txt";
      char result[256];
      std::system(cmd.c_str());
      ifstream ifs("result.txt", std::ifstream::in);
      ifs.getline(result, 256);
      //cout<<"result is : "<<result<<endl;
      string res = result;
      bool success = false;

      if(res.find("Runtime") == std::string::npos)
      {
        int maximum = 0;
        for(int k = 0; k < res.size(); ++k)
          maximum = max(256-abs(res[k] - str[j]) - (k-j)*(k-j) + (64-4*(k-j))*(res[k] == str[j]), maximum);
        maximum -= 4*abs(res.size() - 1);
        newpopulation[i].fitness += maximum;
      }
      else
      {
        res = res.substr(0, res.find("Runtime"));
        int maximum = 0;
        for(int k = 0; k < res.size(); ++k)
          maximum = max(256-abs(res[k] - str[j]) - (k-j)*(k-j) + (64-4*(k-j))*(res[k] == str[j]), maximum);
        //maximum += 64*(res.size() == 1);
        newpopulation[i].fitness += maximum;
      }
    }
    //cout<<"evaluate finished\n";
  }
  return;
}

void initialize ()
{
    for (int j = 0; j < POPSIZE; j++ )
    {
      for(int k = 0; k < str.size(); ++k)
      {
        std::vector<char> v;
        v.push_back(possibleGenes[rand()%8]);
        population[j].genes.push_back(v);
      }
      population[j].fitness = 0;
    }

  return;
}

void mutate ()
{
  int i;
  double x;

  for ( i = 0; i < POPSIZE; i++ )
  {
    if(rand()%1000 < 1000*PMUTATION)
    {
      //cout<<"mutated\n";
      int id = rand()%newpopulation[i].genes.size();
      int newLength = rand()%((int)(1+newpopulation[i].genes[id].size()))+1;
      for(int j = 0; j < newLength; ++j)
      {
        newpopulation[i].genes[id].push_back(possibleGenes[rand()%8]); 
      }
    }
  }
  //cout<<"mutation over\n";

  return;
}

void report ( int generation )
{
  double avg;
  double best_val;
  int i;
  double square_sum;
  double stddev;
  double sum;
  double sum_square;

  if ( generation == 0 )
  {
    cout << "\n";
    cout << "  Generation       Best            Average       Standard \n";
    cout << "  number           value           fitness       deviation \n";
    cout << "\n";
  }

  sum = 0.0;
  sum_square = 0.0;

  for ( i = 0; i < POPSIZE; i++ )
  {
    sum = sum + population[i].fitness;
    sum_square = sum_square + population[i].fitness * population[i].fitness;
  }

  avg = sum / ( double ) POPSIZE;
  square_sum = avg * avg * POPSIZE;
  stddev = sqrt ( ( sum_square - square_sum ) / ( POPSIZE - 1 ) );
  if(prev_best_val == population[POPSIZE-1].fitness)
    ++counter;
  else
    counter = 0;
  //PMUTATION = 0.15 + counter*0.01;
  best_val = population[POPSIZE-1].fitness;
  prev_best_val = best_val;

  cout << "  " << setw(8) << generation 
       << "  " << setw(14) << best_val 
       << "  " << setw(14) << avg 
       << "  " << setw(14) << stddev << "\n";
  cout<<" PMUTATION = "<<PMUTATION<<endl;

  return;
}

void selector ()
{
  std::vector<genotype> curPop;

  for(int i = 0; i < POPSIZE; ++i)
    curPop.push_back(population[i]);
  for(int i = 0; i < POPSIZE; ++i)
    curPop.push_back(newpopulation[i]);

  sort(curPop.begin(), curPop.end());

  for(int i = 2*POPSIZE - 1; i >= POPSIZE; --i)
    population[i-POPSIZE] = curPop[i];

  string s = "";
  for(int i = 0; i < population[POPSIZE-1].genes.size(); ++i)
  {
    std::vector<char> gene = population[POPSIZE-1].genes[i];
    for(int j = 0; j < gene.size(); ++j)
      s += gene[j];
  }
  ofstream ofs("output.bf", std::ostream::out);
  ofs << s;
  ofs.close();
  string cmd = "timeout -k 1 1 ./bfinterpreter output.bf > result.txt";
  char result[256];
  std::system(cmd.c_str());
  ifstream ifs("result.txt", std::ifstream::in);
  ifs.getline(result, 256);
  cout<<"best output : ";
  for(int i = 0; i < population[POPSIZE-1].genes.size(); ++i)
  {
    for(int j = 0; j < population[POPSIZE-1].genes[i].size(); ++j)
      cout<<population[POPSIZE-1].genes[i][j];
    cout<<" ";
  }
  //for(int i = 0; i < population[POPSIZE-1].genes.size(); ++i)
    //cout<<population[POPSIZE-1].genes[i];
  cout<<" : "<<result<<" : "<<population[POPSIZE-1].fitness<<endl;
}

void timestamp ( )
{
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct tm *tm;
  size_t len;
  time_t now;

  now = time ( NULL );
  tm = localtime ( &now );

  len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );

  cout << time_buffer << "\n";

  return;
# undef TIME_SIZE
}

void Xover ( int one, int two)
{
  int point = rand()%(str.size()+1);
  newpopulation[ctr].fitness = 0;
  newpopulation[ctr+1].fitness = 0;
  newpopulation[ctr].genes.clear();
  newpopulation[ctr+1].genes.clear();

  std::vector<char> v1, v2, dummy;

  for(int i = 0; i < point; ++i)
  {
    //cout<<"okay\n";
    v1 = population[one].genes[i];
    v2 = population[two].genes[i];
    int point1 = rand()%v1.size() + 1, point2 = rand()%v2.size() + 1;
    
    dummy.clear();
    for(int j = 0; j < point1; ++j)
    {
      dummy.push_back(v1[j]);
    }
    for(int j = point2; j < v2.size(); ++j)
      dummy.push_back(v2[j]);
    newpopulation[ctr].genes.push_back(dummy);

    dummy.clear();
    for(int j = 0; j < point2; ++j)
      dummy.push_back(v2[j]);
    for(int j = point1; j < v1.size(); ++j)
      dummy.push_back(v1[j]);
    newpopulation[ctr+1].genes.push_back(dummy);
  }
  for(int i = point; i < str.size(); ++i)
  {
    //cout<<"okay1\n";
    v2 = population[one].genes[i];
    v1 = population[two].genes[i];
    int point1 = rand()%v1.size() + 1, point2 = rand()%v2.size() + 1;

    dummy.clear();
    for(int j = 0; j < point1; ++j)
    {
      dummy.push_back(v1[j]);
    }
    for(int j = point2; j < v2.size(); ++j)
      dummy.push_back(v2[j]);
    newpopulation[ctr].genes.push_back(dummy);

    dummy.clear();
    for(int j = 0; j < point2; ++j)
      dummy.push_back(v2[j]);
    for(int j = point1; j < v1.size(); ++j)
      dummy.push_back(v1[j]);
    newpopulation[ctr+1].genes.push_back(dummy);
  }
  ctr += 2;
}