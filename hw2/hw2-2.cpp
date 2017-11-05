#include <iostream>
#include <fstream>
#include <string>
#include <climits>
using namespace std;

int main () {
  string line;
  ifstream myfile ("Q2.txt");
  int n;
  int bt[20], tmp[21], at[20], p[20], wt[20], tat[20];
  float avwt = 0, avtat = 0;
  if (myfile.is_open())
  {
    myfile >>  n;
    for (int i = 0; i < n; i++){
        myfile >> at[i];
        p[i] = i+1;
    }
    for (int i = 0; i < n; i++){
        myfile >> bt[i];
        tmp[i] = bt[i];
    }
    myfile.close();
  }
  else cout << "Unable to open file";

  int cnt = 0;
  int end = 0;
  tmp[20] = INT_MAX;
  for (int time = 0; cnt != n;)
  {
        int min = 20;
        for (int i = 0; i < n; i++){
            if (at[i] <= time && tmp[i] < tmp[min] && tmp[i] > 0)
                min = i;
        }
        if (min != 20){
            cnt++;
            end = time + tmp[min];
            time += tmp[min];
            tmp[min] = 0;
            wt[min] = end - at[min] - bt[min];
            tat[min] = end - at[min];
            avwt += wt[min];
            avtat += tat[min];
        }else {
            time++;
        }
  }

  cout << "Process\t\tWaiting Time\tTurnaround Time";

  for (int i = 0; i < n; i++){
     cout << "\n[" << p[i] << "]" << "\t\t" << wt[i] << "\t\t" << tat[i];
  }
  avwt /= n;
  avtat /= n;
  cout << "\n\nAverage Waiting Time:" << avwt;
  cout << "\nAverage Turnaround Time:" << avtat << endl;
  return 0;
}

