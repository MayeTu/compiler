#include <vector>
#include <iostream>
#include <fstream>
#include "basa.hpp"
//#include "program.hpp"

using namespace std;

int main(){
    ifstream d[3]; 
    d[0].open("1.txt");
    d[1].open("2.txt");
    d[2].open("grammar.txt");
    int flag_for_file=false;
    for (int i=1; i<4; i++){
        if (!d[i-1].is_open()){
            flag_for_file=true;
            cout << "      !!!  DATABASE "<< i << " CORRUPTED  !!!\n";
        }
    }
    if (flag_for_file) exit(0);

    Basa basa;

    basa.load(d[0],d[1],d[2]);

    d[0].close();
    d[1].close();

    basa.work();//////function for main work

    ofstream dd[2];
    dd[0].open("3.txt");
    dd[1].open("4.txt");
    flag_for_file=false;
    for (int i=1; i<3; i++){
        if (!dd[i-1].is_open()){
            flag_for_file=true;
            cout << "      !!!  DATABASE "<< i << " CORRUPTED  !!!\n";
        }
    }
    if (flag_for_file) exit(0);

    basa.unload(dd[0],dd[1]);

    dd[0].close();
    dd[1].close();

    return 0;
}
