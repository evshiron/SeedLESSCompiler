//
// Created by fancysherry on 15-10-23.
//
#ifndef SEEDLESSCOMPILER_CALCULATE_H

#define SEEDLESSCOMPILER_CALCULATE_H

#include <string.h>
#include <sstream>

using namespace std;
//const int MAX = 1000;

string Calculate(string origin);

double function_cal(char *Str_input);
char switchTo16(int number);
bool replace(std::string &str, const std::string &from, const std::string &to);
string rgbToString(int r, int g, int b);

template<class Type>
class STACK {                 //定义栈类
private:
    Type base[1000];
    int Size;
public:
    STACK() { Size = 0; };

    void push(Type a)     //入栈
    {
        base[Size] = a;
        Size++;
    }

    Type pop()            //出栈
    {
        return base[--Size];
    }

    int size() {
        return Size;
    }
};

class CheckStr {
public:
    static int check(char *str) {
        int i;
        STACK<char> Temp;
        for (i = 0; i < strlen(str); i++) {
            char t = str[i];
            if (!((int(str[i]) <= 57 && int(str[i]) >= 48) || str[i] == '(' || str[i] == ')' || str[i] == '*'
                  || str[i] == '+' || str[i] == '-' || str[i] == '/' || str[i] == '.' ||
                  str[i] == 'e'))       //检测是否含有非法字符 添加科学计数法e
                return 2;
            else if (str[i] == '(')
                Temp.push('0');
            else if (str[i] == ')') {
                if (Temp.size() <= 0)                                      //检测括号是否匹配，右括号是否过多
                    return 1;
                else
                    Temp.pop();
            }
        }
        if (Temp.size() != 0)                                                //检测括号是否匹配，左括号是否过多
            return 1;
        return 0;
    }
};

class Calculator {
public:
    bool IsData(char);

    bool IsSym(char);

    int IsPar(char);

    bool Check(char *);

    int setPri(char);                 //判断符号的优先极别
    double ToData(char *);               //把字符串转化为数值
    double Call(double, double, char);    //具体按符号计算
    int GetMatch(char *buffer, int pos); //利用栈找到匹配的括号
    void Opr(STACK<char> &, STACK<double> &, int &); //利用栈计算
    double Calculate(char *, double &);

    //字符串的读入及调配

};


class Input {
public:
    Input() {
        for (int i = 0; i < 1000; i++)
            Str_input[i] = '\0';
    }

    char Str_input[1000];
} ;


class Output {
public:
    Output() {
        result = 0;
    }

    void getRes(double res) {
        result = res;
    }

    double printRes() {
//         cout<<"这个表达式的结果为："<<result<<endl;
        return result;
    }

private:
    double result;
};


#endif //SEEDLESSCOMPILER_CALCULATE_H