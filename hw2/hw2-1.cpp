#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main () {
  string line;
  ifstream myfile ("Q1.txt");
  int n;
  int bt[20], wt[20], tat[20];
  float avwt = 0, avtat = 0;
  if (myfile.is_open())
  {
    myfile >>  n;
    for (int i = 0; i < n; i++){
        myfile >> bt[i];
    }
    myfile.close();
  }
  else cout << "Unable to open file";
  wt[0] = 0;    //waiting time for first process is 0

  //calculating waiting time
  for (int i = 1; i < n; i++){
    wt[i] = 0;
    for (int j = 0; j < i; j++)
        wt[i] += bt[j];
  }

  cout << "Process\t\tWaiting Time\tTurnaround Time";

  //calculating turnaround time
  for (int i = 0;i < n; i++){
    tat[i] = bt[i] + wt[i];
    avwt += wt[i];
    avtat += tat[i];
    cout << "\n[" << i+1 << "]" << "\t\t" << wt[i] << "\t\t" << tat[i];
  }
  avwt /= n;
  avtat /= n;
  cout << "\n\nAverage Waiting Time:" << avwt;
  cout << "\nAverage Turnaround Time:" << avtat << endl;
  return 0;
}
