#include <iostream>
#include <cstdio>
#include <string>
#include "p2Table.h"
#include <stdlib.h>
using namespace std;

int main()
{
  Table table;

  // TODO: read in the csv file
  string csvFile;
  cout << "Please enter the file name: ";
  cin >> csvFile;
  if (table.read(csvFile))
    cout << "File \"" << csvFile << "\" was read in successfully." << endl;
  else exit(-1); // csvFile does not exist.


  char str[5];
  // TODO read and execute commands
  while (true) {
    
    scanf("%s", str);
    if(strcmp(str, "PRINT") == 0) {
      table.print();
      continue;
    } else if(strcmp(str, "EXIT") == 0) {
      break;
    }

    int col_num;
    scanf("%u", &col_num);
    if(strcmp(str, "SUM") == 0) {
      cout << "The summation of data in column #" << col_num << " is " << table.sum(col_num)  << "." << endl;
    } else if(strcmp(str, "AVE") == 0) {
      table.ave(col_num);
    } else if(strcmp(str, "MAX") == 0){
      table.max(col_num);
    } else if(strcmp(str, "MIN") == 0){
      table.min(col_num);
    } else if(strcmp(str, "COUNT") == 0){
      table.count(col_num);
    } else if(strcmp(str, "ADD") == 0){
      
      int col_total = table.getColTotal();
      int arr[col_total];
      arr[0] = col_num;
      for(int i = 1; i < col_total; i++) {
        scanf("%u", &arr[i]);
      }
      //table.add(arr);
    }

    
  }
}
