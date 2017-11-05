#include <iostream>
#include <fstream>
#include <string>
#include <climits>
using namespace std;

int main () {
  string line;
  ifstream myfile ("Q4.txt");
  int n, t1, t2;
  int bt[20], tmp[20], at[20], p[20], wt[20], tat[20], a[20], b[20];
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
        a[i] = b[i] = 0;
    }
    myfile >> t1;
    myfile >> t2;
    myfile.close();
  }
  else cout << "Unable to open file";
  int pos,temp;
  for(int i=0;i<n -1;i++)
    {
        for(int j=0;j<n-1-i;j++)
        {
            if(at[j] > at[j+1]){
                pos=j+1;

                temp=bt[j];
                bt[j]=bt[pos];
                bt[pos]=temp;

                temp=p[j];
                p[j]=p[pos];
                p[pos]=temp;

                temp=at[j];
                at[j]=at[pos];
                at[pos]=temp;

                temp=tmp[j];
                tmp[j]=tmp[pos];
                tmp[pos]=temp;
            }
        }
    }
  int time = 0;
  int i,j,k,x;
  i = j = k = 0;
  x = n;
  int a_is_empty, b_is_empty, run_c;
  int flag;
  while(x != 0){
        a_is_empty = b_is_empty = 1;
        run_c = 0;
        for (int t = 0; t < n; t++){
            if (a[t] == 0 && at[t] <= time)
                a_is_empty = 0;
            if (b[t] == 0 && at[t] <= time)
                b_is_empty = 0;
            if (a[t] && b[t] && tmp[t] > 0)
                run_c = 1;
        }
        if (!a_is_empty){
            if(tmp[i] <= t1 && tmp[i] > 0 && at[i] <= time)
            {
                  time += tmp[i];
                  tmp[i] = 0;
                  flag = 1;
                  a[i] = 1;
                  b[i] = 1;
                  i++;
            }
            else if(tmp[i] > 0 && at[i] <= time)
            {
                  tmp[i] -= t1;
                  time += t1;
                  a[i] = 1;
                  i++;
            }
            if(tmp[i-1] == 0 && flag == 1)
            {
                  x--;
                  wt[i-1] = time - at[i-1] - bt[i-1];
                  tat[i-1] = time - at[i-1];
                  avwt += wt[i-1];
                  avtat += tat[i-1];
                  flag = 0;
            }
        }else if(a_is_empty && !b_is_empty){
            if (b[j]) j++;
            if(tmp[j] <= t2 && tmp[j] > 0 && at[j] <= time)
            {
                  time += tmp[j];
                  tmp[j] = 0;
                  flag = 1;
                  b[j] = 1;
                  j++;
            }
            else if(tmp[j] > 0 && at[j] <= time)
            {
                  tmp[j] -= t2;
                  time += t2;
                  b[j] = 1;
                  j++;
            }
            if(tmp[j-1] == 0 && flag == 1)
            {
                  x--;
                  wt[j-1] = time - at[j-1] - bt[j-1];
                  tat[j-1] = time - at[j-1];
                  avwt += wt[j-1];
                  avtat += tat[j-1];
                  flag = 0;
            }
        }else if (a_is_empty && b_is_empty && run_c){
            int min = INT_MAX;
            for (int t = 0; t < n; t++){
                if (a[t] == 1 && b[t] == 1 && tmp[t] > 0 && tmp[t] < min && at[t] <= time){
                    min = tmp[t];
                    k = t;
                }
            }
            time += tmp[k];
            tmp[k] = 0;
            x--;
            wt[k] = time - at[k] - bt[k];
            tat[k] = time - at[k];
            avwt += wt[k];
            avtat += tat[k];
        }else{
            time++;
        }
  }
  for(int i=0;i<n;i++)
    {
        pos=i;
        for(int j=i+1;j<n;j++)
        {
            if(p[j]<p[pos])
                pos=j;
        }

        temp=bt[i];
        bt[i]=bt[pos];
        bt[pos]=temp;

        temp=p[i];
        p[i]=p[pos];
        p[pos]=temp;

        temp=at[i];
        at[i]=at[pos];
        at[pos]=temp;

        temp=wt[i];
        wt[i]=wt[pos];
        wt[pos]=temp;

        temp=tat[i];
        tat[i]=tat[pos];
        tat[pos]=temp;
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

