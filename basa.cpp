#include "basa.hpp"
#include <algorithm>
#include <stack>

using namespace std;

//3амена нетерминалов чтобы не было кривого распознавания таких как I и I1
//E1=Q S1=Y I1=A D1=X D2-Z

void Basa::load(ifstream& file1, ifstream& file2,ifstream& file3)//table 1 is words // table2 is razdeliteli
{
    int i=1;
    string temp;
    while (file1>>temp){
        table1[temp]=i;
        i++;
    }
    i=1;
    while(file2>>temp){
        table2[temp]=i;
        i++;
    }

    string term_line, non_term_line;
    getline(file3,term_line);
    getline(file3,non_term_line);
    temp.clear();
    for (int i=0; i<term_line.size(); i++){
        if (term_line[i]!=' '){
            temp.push_back(term_line[i]);
        }
        else {
            TNT.terms.push_back(temp);
            temp.clear();
        }
    }   
    temp.clear();
    for (int i=0; i<non_term_line.size(); i++){
        if (non_term_line[i]!=' '){
            temp.push_back(non_term_line[i]);
        }
        else {
            TNT.non_terms.push_back(temp);
            temp.clear();
        }
    }   
    while(getline(file3,temp)){//'=space
        string f,s;
        f= temp.substr(0, temp.find(' '));
        s= temp.substr(temp.find(' ')+1,temp.size()-f.size()-2);
        Grammar.Rules.push_back({f,s});
    }
}

void Basa::work(){
    cout << "insert the line\n";
    string line;
    getline(cin,line);
    if (line.size()==0){
        cout << "void line is not avaible";
        return;
    }

    lexer(line);//парсим входную строчку
    
    parser(line);
}

void Basa::lexer(string original_line){
    string temp_line{};
    string temp_char;
    string temp_chars;
    //temp_line.push_back(temp_char[0]);

    for (unsigned i=0; i<original_line.size(); i++){//строка должна каждый раз в конце содержать ; так что все должно работать
        temp_line.push_back(original_line[i]);
        temp_char=original_line[i];
        
        if (temp_line.size()>1){
            temp_chars.push_back(temp_line[temp_line.size()-2]);
            temp_chars.push_back(temp_line[temp_line.size()-1]);
            //temp_char=temp_line[temp_line.size()-1];
        }
        
        bool f=false;
        if (table2.find(temp_chars)!=table2.end()){
            if (temp_line.size()>1){
                temp_line.erase(temp_line.begin()+temp_line.size()-1);
                temp_line.erase(temp_line.begin()+temp_line.size()-1);
            }
            l_parser(temp_line,temp_chars);

            f=true;
        }
        else {
            if (table2.find(temp_char)!=table2.end()){
                if (temp_line.size()>1)
                    temp_line.erase(temp_line.begin()+temp_line.size()-1);
                if (temp_line.size()>0)    
                    temp_line.erase(temp_line.begin()+temp_line.size()-1);
                if (temp_chars.size()>0)
                    temp_line.push_back(temp_chars[temp_chars.size()-2]);
                l_parser(temp_line, temp_char);

                f=true;
            }
        }
        if (f){
            temp_line.clear();
            temp_char.clear();
            temp_chars.clear();
        }
    }
}

void Basa:: l_parser(string &temp_line, string &temp_chars){
    //присваиваем айдишник слову до разделителя из таблицы 1 или добавляем в таблицу 3
    int table_number;
    int id_number;
    if (table1.find(temp_line)!=table1.end()){
        id_number=table1[temp_line];
        table_number=1;
    }
    else {
        table_number=3;
        if (table3.find(temp_line)!=table3.end()){
            id_number=table3[temp_line];
        }
        else {
            if (temp_line.size()>0){
                table3[temp_line]=table3.size();
                id_number=table3.size();
            }
        }
    }
    if (temp_line.size()>0){
        id_number++;
        table4.push_back({temp_line,table_number,id_number});
    }

    //присваиваем разделителю айдишник
    table_number=2;
    id_number=table2[temp_chars];
    id_number++;

    table4.push_back({temp_chars,table_number,id_number});
}

void Basa::parser(string main_line){
    int ans=0; //0-ok  1-not ok

    /*for (auto y: table3){//заносим все новые переменные+ буквы\цифры со строчки в таблицу терминалов
        TNT.terms.push_back(y.first);
    }
    for (int i=0;i<10; i++){
        string a{char('0'+i)};
        TNT.terms.push_back(a);
        Grammar.Rules.push_back({"R",a});
    }
    for (int i=0; i<26;i++){
        string a{char('a'+i)};
        TNT.terms.push_back(a);
        Grammar.Rules.push_back({"C",a});
    }*/
    
    //работа с табл 3. поиск чисел и переменных
    for (auto u:table3){//N-number I-id name
        string a=u.first;
        bool nums_there=false;
        bool char_there=false;
        bool nums_first=false;
        for (int i=0; i<a.size();i++){
            char d=a[i];
            if ('a'<=d && d<='z'){
                char_there=true;
            }
            else if ('0'<=d && d<='9'){
                nums_there=true;
                if (i==0) nums_first=true;
            }
            else {
                cout << "the line is not correct" << endl;
                cout << "unexpected character in identificator name";
                return;
            }
        }
        if (!char_there && nums_there){
            Grammar.Rules.push_back({"N",a});
            TNT.terms.push_back(a);
        }
        else if(char_there && !nums_first){
            Grammar.Rules.push_back({"I",a});
            TNT.terms.push_back(a);
        }
        else {
            cout << "the line is not correct" << endl;
            cout << "incorrect identificator name";
            return;
        }
    }

    //построение множества First
    bool there_nt=false;
    set <pair <string,string>> Set_First;
    map <pair <string,string>, int> rule_Number;
    for (int ii=0; ii<Grammar.Rules.size(); ii++){
        string l=Grammar.Rules[ii].L;
        string r=Grammar.Rules[ii].R;
        
        string ttemp{};
        for (int i=0; i<r.size(); i++){
            ttemp.push_back(r[i]);
            if (TERM(ttemp) ^ NTERM(ttemp)){//функция как снизу и то же самое но на нетерминалы
                r=ttemp;
                if (l!=r){
                    if (NTERM(ttemp)) there_nt=true;
                    if (Set_First.find({l,r})==Set_First.end()){
                        Set_First.insert({l,r});
                        First.push_back({l,r});
                        rule_Number[{l,r}]=ii;
                    }
                }
                break;
            }
        }
    }
    while (there_nt){
        there_nt=false;
        for (int i=0;i<First.size(); i++){
            if (NTERM(First[i].second)){
                vector <string> to_push{};
                for(int j=0; j<First.size(); j++){
                    if (First[j].first==First[i].second){
                        to_push.push_back(First[j].second);
                        if (NTERM(First[j].second)) there_nt=true;
                        /*pair <string ,string> d={First[i].first,First[j].second};
                        pair <string,string> dg={First[j].first,First[j].second};
                        if (d!=dg)
                        rule_Number[d].insert(rule_Number[d].begin(),rule_Number[dg].begin(),rule_Number[dg].end());*/
                        rule_Number[{First[i].first,First[j].second}]=rule_Number[{First[i].first,First[i].second}];
                    }//i.f i.s=j.f j.s
                }
                if (to_push.size()>0 ) {
                    First[i].second=to_push[0];
                    //if (TERM(First[i].second)) rule_Number[{First[i].first,First[i].second}]=;
                }
                for (int j=1; j<to_push.size(); j++){
                    if (Set_First.find({First[i].first,to_push[j]})==Set_First.end()){
                        Set_First.insert({First[i].first,to_push[j]});
                        First.push_back({First[i].first,to_push[j]});
                        //if (TERM(to_push[i])) rule_Number[{First[i].first,to_push[i]}]=;
                    }
                    //First.push_back({First[i].first,to_push[j]});
                }
            }
        }
    }
    sort(First.begin(),First.end());//для удобства проверки
    
    //построение Follow
    string eps="~";//это такой эпсилон типа пустая строчка
    set <pair <string,string>> Set_Follow;

    there_nt=false;
    for (int i=0; i<TNT.non_terms.size(); i++){
        Follow.push_back({TNT.non_terms[i],eps});
        Set_Follow.insert({TNT.non_terms[i],eps});
    }
    for (int i=0; i<Grammar.Rules.size(); i++){
        for (int j=0; j<Grammar.Rules[i].R.size()-1; j++){
            bool a=CNTERM(Grammar.Rules[i].R[j]);
            //bool aa=CTERM(Grammar.Rules[i].R[j]);
            if (a){
            //if (CNTERM(Grammar.Rules[i].R[j]) || CTERM(Grammar.Rules[i].R[j])){//эта штука не выполняется
                string ttemp;
                for (int t=j+1; t<Grammar.Rules[i].R.size(); t++){
                    if (Grammar.Rules[i].R[t]!=' ') ttemp.push_back(Grammar.Rules[i].R[t]);
                    if (TERM(ttemp) ^ NTERM(ttemp)){
                        string tt=ttemp;
                        if (t<Grammar.Rules.size()-1){
                            tt.push_back(Grammar.Rules[i].R[t+1]);
                        }
                        if (TERM(tt) ^ NTERM(tt)){
                            ttemp=tt;
                        }
                        if (NTERM(ttemp)) there_nt=true;

                        string l;//to_string работает плохо. символ ---> код символа как строка
                        l.push_back(Grammar.Rules[i].R[j]);
                        string r=ttemp;
                        if (Set_Follow.find({l,r})==Set_Follow.end()){
                            Set_Follow.insert({l,r});
                            Follow.push_back({l,r});
                        }
                        //break;
                    }
                }
            }
        }
    }//добавить элементи 3 таблицы в терминалы после реализации follow
    while (there_nt){
        there_nt=false;

        vector <int> id_to_delete;
        vector <pair <string,string>> to_add;
        for (int i=0; i<Follow.size();i++){
            if (NTERM(Follow[i].second)){
                id_to_delete.push_back(i);
                for (int j=0; j<Follow.size(); j++){
                    if (Follow[j].first==Follow[i].second && Follow[i].first!=Follow[j].second){
                        to_add.push_back({Follow[i].first, Follow[j].second});
                        if (NTERM(Follow[j].second)){
                            there_nt=true;
                        }
                    }
                }
            }
        }
        sort(id_to_delete.begin(),id_to_delete.end());
        reverse(id_to_delete.begin(),id_to_delete.end());
        for (int i=0; i<id_to_delete.size(); i++){
            Follow.erase(Follow.begin()+id_to_delete[i]);
        }
        for (int i=0; i<to_add.size(); i++){
            if (Set_Follow.find(to_add[i])==Set_Follow.end()){
                Follow.push_back(to_add[i]);
                Set_Follow.insert(to_add[i]);
            }
        }
    }
    sort(Follow.begin(),Follow.end());//для удобства проверки

    //распознавание цепочки и работа со стеком
    stack <string> Stek;
    Stek.push("P");
    vector <triple> buf=table4;
    if (main_line=="{}"){
        cout <<"the line is correct";
        return;
    }

    while (ans==0 && Stek.size()!=0 && buf.size()!=0){
        string A=Stek.top();
        string B=buf[0].word;
        pair <string,string> q={A,B};
        if (NTERM(A) && TERM(B) && find(First.begin(), First.end(),q)!=First.end()){
            int rulz=rule_Number[q];//
            string x=Stek.top();
            Stek.pop();
            /*for (int i=0; i<rulz.size(); i++){
                string last=x;
                last.erase(0,1);
                x=Grammar.Rules[rulz[i]].R;
                x=x+last;
            }
            for (int i=x.size()-1; i>-1; i--){//не надо разбивать переменную из нескольких букв- закидывать сразу целиком
                string r; r.push_back(x[i]);
                Stek.push(r);
            }*/
            string last=x;
            last.erase(0,1);//!!!!!!!!!!!!!!!!!!!!(переменный из неск буквт)
            x=Grammar.Rules[rulz].R;
            x=x+last;
            string temp;
            for (int i=x.size()-1;i>-1;i--){
                if (x[i]!=' ') temp.push_back(x[i]);
                string rev=temp;
                reverse(rev.begin(),rev.end());
                if (TERM(rev) || NTERM(rev)){
                    Stek.push(rev);
                    temp.clear();
                }
            }
            if (temp.size()!=0){
                reverse(temp.begin(),temp.end());
                Stek.push(temp);
                cout<< "something wrong" << endl;
            }
            //Stek.push(x);
        }
        /*if (NTERM(A) && TERM(B) && find(First.begin(), First.end(),q)!=First.end()){
            Stek.pop();
            bool don=false;
            string x;
            for (int i=0; i<First.size(); i++){
                if (First[i].first==A){
                    for (int j=0; j<First.size(); j++){//------------
                        if (First[i].second==First[j].first){
                            x=First[j].second;
                            don=true;
                            break;
                        }
                    }
                }
                if (don) break;
            }
            //if (!don) ans=1;//hz??
            Stek.push(x);
        }*/
        else if (NTERM(A) && TERM(B) && find(Follow.begin(), Follow.end(),q)!=Follow.end()){
            Stek.pop();
            //Stek.push(eps);
        }
        else if(TERM(B) && B==A){
            buf.erase(buf.begin());
            Stek.pop();
        }
        else {
            ans=1;
        }
    }

    //обработка ответа
    if (ans==0){
        cout << "the line is correct";
    }
    else {
        cout << "the line is not correct";
    }
    return;
}

/* for (int i=0; i<rowCount; i++){
    for (int j=0; j<columnCount; j++){
        cout << matrix[i][j] << ' ';
    }
    cout << rightVector[i] << endl;
}
*/
bool Basa::CTERM(char line){
    string lline;
    lline.push_back(line);
    return TERM(lline);
}

bool Basa::TERM(string line){
    for (int i=0; i<TNT.terms.size();i++){
        if (line==TNT.terms[i]){
            return true;
        }
    }
    return false;
}

bool Basa::CNTERM(char line){
    string lline;
    lline.push_back(line);
    return NTERM(lline);
}

bool Basa::NTERM(string line){
    for (int i=0; i<TNT.non_terms.size();i++){
        if (line==TNT.non_terms[i]){
            return true;
        }
    }
    return false;
}

void Basa::unload(ofstream& file3,ofstream& file4)
{
    for (auto& p:table3){
        file3 << p.first << endl;
    }
    for (int i=0; i< table4.size(); i++){
        ///file4 << вывод в формате строка №табл №эл 
        file4 << table4[i].word << ' ' << table4[i].table << ' '<< table4[i].id-1 << endl;
    }
}
