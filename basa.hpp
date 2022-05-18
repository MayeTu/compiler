#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

struct triple{
    string word;
    int table,id;
};

struct rule{
    string L;
    string R;
};

struct rules{
    vector <rule> Rules;
};

struct t_i_nt{
    vector <string> non_terms;
    vector <string> terms;
};

class Basa{
//private:

public:

    map <string,int> table1;//таблица 1 и 2 это слово-айдишник
    map <string,int> table2;
    map <string,int> table3;//а тут уже все нормально, номер -слово
    rules Grammar;
    t_i_nt TNT;
    int table3_size=0;
    vector <pair <string,string>> First;
    vector <pair <string,string>> Follow;
    map <pair <string,string>, int> Table;

    vector <triple> table4;
    void load(ifstream& file1, ifstream& file2,ifstream& file3);

    void work();

    void lexer(string line);
    void l_parser(string &temp_line, string &temp_chars);

    void parser(string line);
    bool CTERM(char line);
    bool TERM(string line);
    bool CNTERM(char line);
    bool NTERM(string line);

    void unload(ofstream& file3,ofstream& file4);
};
