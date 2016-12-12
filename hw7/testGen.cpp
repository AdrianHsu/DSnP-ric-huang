#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <climits>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <map>


using namespace std;

#define TASKINIT    (((rand() % 1000) + 1) * 10000)
#define NAME_LEN    6
#define LOAD_LIMIT  20000

#define BASIC_NUM   50
#define NUM_OUTPUT  2    

#define WEIGHT      2000
#define CYCLE_CHECK 5000


int main(int argc, char** argv)
{
   if(argc != 3){
      cerr << "Usage : ./testGen <file_name> <# of task -n>" << endl;
      cerr << "e.g.    ./testGen do5 20000 " << endl;
      return -1;
   }

   string numstr = argv[2];
   for(int i = 0; i < numstr.length(); i++){
      if(!isdigit(numstr[i])){
         cerr << "ERROR : \"" << numstr << "\" is NOT a non-negative integer. " << endl;
         return -1;
      }
   }

    srand((unsigned)time(NULL));
    fstream outf(argv[1], ios::out);

    outf << "taski " << TASKINIT << endl;

    int n = atoi(argv[2]);
    for(int i = 0; i < n; i++){
        outf << "taskn -n ";
        for(int j = 0; j < NAME_LEN; j++)
           outf << char(int('a') + (rand()%26));
        outf << " " << ((rand()%LOAD_LIMIT)+1) << endl;
    }


    /*** ADD ANY COMMAND U LIKE ***/

    int cnt = 0;
    while(cnt++ < n/NUM_OUTPUT){
       int mod = rand() % 3;
       if(!(cnt % WEIGHT)) mod = rand() % 6;
       if(!(cnt % CYCLE_CHECK)) mod = 6;
       switch(mod)
       {
          case 0: outf << "taska " << rand() % LOAD_LIMIT << " -r " << rand() % BASIC_NUM << endl;
                  break;
          case 1: outf << "taskr -r " << rand() % BASIC_NUM << endl;
                  break;
          case 2: outf << "taskn -r " << rand() % BASIC_NUM << endl;
                  break;
          case 3: for(int i = 0; i < BASIC_NUM; i++){
                     outf << "taskr -n ";
                     for(int j = 0; j < NAME_LEN; j++)
                        outf << char(int('a') + (rand()%26));
                     outf << endl;
                  }
                  break;

          case 4: for(int i = 0; i < BASIC_NUM/10; i++){
                     outf << "taskn -n ";
                     for(int j = 0; j < NAME_LEN; j++)
                        outf << char(int('a') + (rand()%26));
                     outf << " " << ((rand()%LOAD_LIMIT)) << endl;
                  }
                  break;

          case 5: for(int i = 0; i < BASIC_NUM/10; i++){
                     outf << "taskq ";
                     for(int j = 0; j < NAME_LEN; j++)
                        outf << char(int('a') + (rand()%26));
                     outf << endl;
                  }
                  break;

          case 6: outf << "taskq -min" << endl; 
                  outf << "taskq -hash" << endl;
                  outf << "taskq -heap" << endl; 
                  if(rand() % 5){
                     outf << "taski " << TASKINIT << endl;
                     outf << "taskn -r " << n << endl;
                  }
                  break;

          default: cerr << "ERROR : DEFAULT HAPPEN ..." << endl;
                   outf.close();
                   return -1;

       }
    }

    outf << "taskq -hash" << endl;
    outf << "taskq -heap" << endl; 
    outf << "taskq -min" << endl;
    outf << "USAGE" << endl;
    outf << "q -f" << endl;

    outf.close();
    return 0;

}

