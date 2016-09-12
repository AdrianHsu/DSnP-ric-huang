#include <iostream>
#include <climits>
#include <cstdio>
#include <cstring>
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
    if(strcmp(str, "SUM") == 0) {
      scanf("%u", &col_num);
      cout << "The summation of data in column #" << col_num << " is " << table.sum(col_num)  << "." << endl;
    } else if(strcmp(str, "AVE") == 0) {
      scanf("%u", &col_num);
      table.ave(col_num);
    } else if(strcmp(str, "MAX") == 0){
      scanf("%u", &col_num);
      table.max(col_num);
    } else if(strcmp(str, "MIN") == 0){
      scanf("%u", &col_num);
      table.min(col_num);
    } else if(strcmp(str, "COUNT") == 0){
      scanf("%u", &col_num);
      table.count(col_num);
    } else if(strcmp(str, "ADD") == 0){
      
      int col_total = table.getColTotal();
      int arr[col_total];
      for(int i = 0; i < col_total; i++) {
        char _str[5]; //-99 to 100
        scanf("%s", _str);
        // it's possible to get empty row. i.e. ADD - -
        if (strcmp(_str, "-") == 0) arr[i] = INT_MAX;
        else arr[i] = atoi(_str);
      }
      table.add(arr);
    }

    
  }
}
