#include <iostream>
#include <string>
#include <map>
#include <cstring>
using namespace std;
map <string,int> test = {
    {"A",1},
    {"B",2},
    {"C",3}
};
int main(){
    char A[20] = "hello,world";
    char B[10] = "world";
    A[5] = '\0';
    string VT_name = A;
    map<string,int>temp;
    temp.insert(pair<string,int>(VT_name,1));
    VT_name = B;
    int test = 2;
    test = temp.at(A);
    cout << test <<endl << strlen(A);
    return 0;
}