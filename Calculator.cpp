//
// Created by fancysherry on 15-10-23.
//

#include "Calculator.h"
#include <iostream>

#include<stack>
#include<iostream>
#include<deque>
#include<string>
#include <algorithm>

#include<cmath>
#include <string.h>
#include <regex>

#define REGEX_MATCH_COUNT(x) distance(x, sregex_iterator())

using namespace std;


//string getValue(string origin);
//double function_cal(char *Str_input);




bool Calculator::IsData(char ch) {
    return ((ch >= '0' && ch <= '9') || ch == '.') ? true : false;
}

bool Calculator::IsSym(char ch)      //判断是否输入非法运算符
{
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/') ? true : false;
}

int Calculator::IsPar(char ch)  //判断是否'()'
{
    if (ch == '(')
        return 1;
    if (ch == ')')
        return -1;
    return 0;
}

bool Calculator::Check(char *ch)//检验小数点个数，>1报错 l
{
    int a = 0;
    for (int i = 0; i < strlen(ch); i++)
        if (ch[i] == '.')
            a++;
    if (a > 1)
        return false;
    return true;
}

int Calculator::setPri(char ch)          //符号的优先极别
{
    switch (ch) {
        case '+':
            return 0;
        case '-':
            return 0;
        case '*':
            return 1;
        case '/':
            return 1;
        default:
            return -1;
    }
}

double Calculator::ToData(char *ch)   //将数字转化为数值
{
    int i, j, k, sumn = 0;
    double sum = 0.0, p = 0.0, q = 0.0;
    if (!Check(ch)) return 0.0;
    for (i = 0; i < strlen(ch); i++)             //读入整数部分
    {
        if (ch[i] != '.')
            sumn = sumn * 10 + (ch[i] - '0');
        else break;
    }

    if (i < strlen(ch))
        for (j = i + 1; j < strlen(ch); j++)        //小数部分
            sum = sum * 10 + (ch[j] - '0');
    sum /= pow(10.0, (double) (strlen(ch) - 1 - i));
    return (sum + sumn);                      //返回值
}

double Calculator::Call(double sum, double data, char ch) {
    double ans = 0.0;
    switch (ch) {
        case '+':
            ans = sum + data;
            break;
        case '-':
            ans = data- sum;//TODO
            break;
        case '*':
            ans = sum * data;
            break;
        case '/':
            if (data != 0.0)
                ans = sum / data;
            else {
                cout << "程序出现除0错误，终止！\n";
                system("pause");
                exit(1);
            }
            break;
        default:
            ans = 0.0;
            break;
    }
    return ans;
}

int Calculator::GetMatch(char *buffer, int pos)     //利用栈找到匹配的括号
{
    STACK<char> Temp;
    int i;
    for (i = pos; i < strlen(buffer); i++) {
        if (IsPar(buffer[i]) == 1)//左括号l
            Temp.push('0');
        if (IsPar(buffer[i]) == -1)//右括号l
        {
            Temp.pop();//出栈l
            if (Temp.size() == 0) return i;
        }
    }
    return -1;
}

void Calculator::Opr(STACK<char> &symbol, STACK<double> &data, int &mark)//运算符操作l
{
    double sum;
    while (symbol.size() != 0)//运算符栈大小不为0，l
    {
        char tem = symbol.pop();//出栈l
        int temp = setPri(tem);//优先级l
        symbol.push(tem);//入栈l
        if (temp < mark)
            break;
        else {
            sum = Call(data.pop(), data.pop(), symbol.pop());
            data.push(sum);
        }
    }
}

double Calculator::Calculate(char *buffer, double &sum)   //字符串读入和各个函数调配
{
    STACK<double> data;
    STACK<char> symbol;
    double ans;
    char temp[1000];
    int ct = 0, mark = 0, tp = 0;
    data.push(sum);//入栈l
    while (ct <= strlen(buffer)) {
        if (IsData(buffer[ct]))            //如果是数字或小数点
        {
            while (ct < strlen(buffer) && IsData(buffer[ct])) {
                temp[tp++] = buffer[ct++];
            }
            temp[tp] = '\0';
            tp = 0;                         //读到非数字也非小数为止
            ans = ToData(temp);             //把读到的字符串转化为数
            data.push(ans);      //将数字存入栈中l

            if (ct == strlen(buffer))        //已经独到字符串末尾
            {
                mark = 0;
                Opr(symbol, data, mark);    //计算
                sum = data.pop();           //此时data栈中还剩一个数据，即是结果
                return sum;               //返回结果
            }
            else {
                int mark = setPri(buffer[ct]);
                Opr(symbol, data, mark);     //计算
            }
        }
        else if (IsSym(buffer[ct]) && buffer[ct - 1] != 'e')         //如果是运算符且不为指数部分
            symbol.push(buffer[ct++]);     //运算符入symbol栈
        else {
            char BF[100];
            int k = 0;          //如果都不是，则只能是括号
            while (IsPar(buffer[ct]) != 1 && ct <= strlen(buffer))
                BF[k++] = buffer[ct++];
            BF[k] = '\0';
            if (IsPar(buffer[ct]) == 1)       //一旦读到左括号，寻找它匹配的右括号
            {
                int i, j;
                char Temp[100];
                for (i = ct + 1, j = 0; i < GetMatch(buffer, ct); i++, j++)
                    Temp[j] = buffer[i];     //把这对括号中的字符串存入Temp
                Temp[j] = '\0';
                data.push(Calculate(Temp, sum)); //递归调用Calculate直到没有括号
                //然后开始计算，值层层返回最后将最终结果放入data栈
                ct += (strlen(Temp) + 1);       //跳过已经处理完的字符
                if (ct + 1 == strlen(buffer))    //这里考虑字符串以括号结尾的情况
                {
                    mark = 0;
                    Opr(symbol, data, mark);
                    sum = data.pop();
                    return sum;
                }
                else {
                    mark = setPri(buffer[ct + 1]); //不是的话继续计算
                    Opr(symbol, data, mark);
                }
                ct++;                           //读入下一个字符
            }
        }
    }
    return 0.;
}


double function_cal(char *Str_input)//Str_input代入equation[][]
{
    double sum = 0.0;
    cout.precision(12);

    Calculator cl;
    Output out;

    while (1) {
        Str_input;//输入模块
        int res = CheckStr::check(Str_input); //判断模块 in.Str_input
        if (res == 0)
            break;
        else if (res == 1)
            cout << "输入字符串括号不匹配，请重新输入：\n";
        else if (res == 2)
            cout << "输入字符串有非法字符，请重新输入：\n";
        else { }
    }
    out.getRes(cl.Calculate(Str_input, sum)); //计算模块
//   out.printRes();                               //输出模块
//    system("pause");
//     return 0;
    return out.printRes();
}


char switchTo16(int number) {
    if (number < 10) {
        return number + '0';
    }
    else {
        switch (number) {
            case 10:
                return 'A';
                break;
            case 11:
                return 'B';
                break;
            case 12:
                return 'C';
                break;
            case 13:
                return 'D';
                break;
            case 14:
                return 'E';
                break;
            case 15:
                return 'F';
                break;
        }
    }
}


int switch16To10(char number) {
    if (number <= '9') {

        return number - '0';
    }
    else {
        switch (number) {
            case 'A':
            case 'a':
                return 10;
                break;
            case 'B':
            case 'b':
                return 11;
                break;
            case 'C':
            case 'c':
                return 12;
                break;
            case 'D':
            case 'd':
                return 13;
                break;
            case 'E':
            case 'e':
                return 14;
                break;
            case 'F':
            case 'f':
                return 15;
                break;
            default:
                break;
        }
    }
}

string rgbToString(int r, int g, int b) {
    string rgb_str = "#";
    rgb_str.resize(7);
    rgb_str[0] = '#';
//    cout <<"1  "<< switchTo16(r / 16) << endl;
//    cout <<"2  "<< switchTo16(r % 16) << endl;
//    cout <<"3  "<< switchTo16(g / 16) << endl;
//    cout <<"4  "<< switchTo16(g % 16) << endl;
//    cout <<"5  "<< switchTo16(b / 16) << endl;
//    cout <<"6  "<< switchTo16(b % 16) << endl;
    rgb_str[1] = switchTo16(r / 16);
    rgb_str[2] = switchTo16(r % 16);
    rgb_str[3] = switchTo16(g / 16);
    rgb_str[4] = switchTo16(g % 16);
    rgb_str[5] = switchTo16(b / 16);
    rgb_str[6] = switchTo16(b % 16);
    return rgb_str;

}

char *stringToCharArray(string str) {
    char exercise[1024];
    strncpy(exercise, str.c_str(), sizeof(exercise));
    exercise[sizeof(exercise) - 1] = 0;
    return exercise;
}

bool replace(std::string &str, const std::string &from, const std::string &to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}


string Calculate(string origin) {

    string pre_expression = origin;
    string str_result;

//    while (true) {

    //  (  4-   2)*3
//    cout << "请输入表达式，按enter结束：" << endl;
//    cin >> str;
//        for (int i = 0; i != str.size(); ++i) {
//            //逐一加入每个字符，这里使用deque因为deque在两端删除添加的速度最快
//            coll1.push_back(str[i]);
//        }


//    string pre_expression = string("  (  4%-   2%)*3%");
//    string pre_expression = string("  (  4.5px-   2.2px)*(3px-1px)");
//    string pre_expression = string("  (  4.5dip-   2.2dip)*(3dip-1dip)");
//    string pre_expression = string("  (  #323211-   #323212)*(#323213-#323214)");

    pre_expression.erase(remove_if(pre_expression.begin(), pre_expression.end(), ::isspace), pre_expression.end());
    cout << pre_expression << endl;
    if (pre_expression.find('#') != -1) {
        int r_val;
        int g_val;
        int b_val;

        regex regex_color("#[a-fA-F0-9]{6}");

        sregex_iterator begin(pre_expression.begin(), pre_expression.end(), regex_color);
        sregex_iterator end = sregex_iterator();

//        cout << "Found " << REGEX_MATCH_COUNT(begin) << " variable uses." << endl;

        string expression_one = pre_expression;
        string expression_two = pre_expression;
        string expression_three = pre_expression;
        for (sregex_iterator it = begin; it != end; ++it) {

            string key = (*it).str();

            int one = switch16To10(key[1]);
            int two = switch16To10(key[2]);
            int temp = one * 16 + two;
            string temp_s = to_string(temp);
            replace(expression_one, key, temp_s);
            cout << "expression_one" << expression_one << endl;
//            cout << "temp"+temp_s << endl;
//            cout << "key"+key << endl;
//            cout << "expression_one"+expression_one << endl;
        }
//        cout<<"array"<<stringToCharArray(expression_one)<<endl;
        r_val = (int) function_cal(stringToCharArray(expression_one));
        if (r_val > 255) {
            r_val = 255;
        }
        cout << "r_val" << r_val << endl;
        if (r_val < 0) {
            r_val = 0;
        }
        for (sregex_iterator it = begin; it != end; ++it) {

            string key = (*it).str();

            int three = switch16To10(key[3]);
            int four = switch16To10(key[4]);
            int temp = three * 16 + four;
            string temp_s = to_string(temp);
            replace(expression_two, key, temp_s);
//            cout << key << endl;
        }
        g_val = (int) function_cal(stringToCharArray(expression_two));
        if (g_val > 255) {
            g_val = 255;
        }
        if (g_val < 0) {
            g_val = 0;
        }


        for (sregex_iterator it = begin; it != end; ++it) {

            string key = (*it).str();

            int five = switch16To10(key[5]);
            int six = switch16To10(key[6]);
            int temp = five * 16 + six;
            string temp_s = to_string(temp);
            replace(expression_three, key, temp_s);
//            cout << key << endl;
        }
        b_val = (int) function_cal(stringToCharArray(expression_three));
        if (b_val > 255) {
            b_val = 255;
        }
        if (b_val < 0) {
            b_val = 0;
        }
//        cout<<"r_double "<<(int)function_cal(stringToCharArray(expression_one))<<endl;
//        cout<<"g_double "<<function_cal(stringToCharArray(expression_two))<<endl;
//        cout<<"b_double "<<function_cal(stringToCharArray(expression_three))<<endl;
//        cout << "r"<<r_val << endl;
//        cout << "g"<<g_val << endl;
//        cout << "b"<<b_val << endl;
        return rgbToString(r_val, g_val, b_val);
    }

    else if (pre_expression.find('%') != -1) {
//        cout << pre_expression+"two" << endl;
        pre_expression.erase(remove(pre_expression.begin(), pre_expression.end(), '%'), pre_expression.end());
        char exercise[1024];
        strncpy(exercise, pre_expression.c_str(), sizeof(exercise));
        exercise[sizeof(exercise) - 1] = 0;

        double result = function_cal(exercise);
        if (result >= 100) {
            result = 100;
        }
        cout << result << '%' << endl;
        ostringstream ss;   //用这种方法可以处理.后0的数量
        ss << result;
        string s(ss.str());

        str_result = s + '%';
    }
    else if (pre_expression.find("px") != -1) {
        pre_expression.erase(remove(pre_expression.begin(), pre_expression.end(), 'p'), pre_expression.end());
        pre_expression.erase(remove(pre_expression.begin(), pre_expression.end(), 'x'), pre_expression.end());
        char exercise[1024];
        strncpy(exercise, pre_expression.c_str(), sizeof(exercise));
        exercise[sizeof(exercise) - 1] = 0;

        double result = function_cal(exercise);

        cout << result << "px" << endl;
        ostringstream ss;
        ss << result;
        string s(ss.str());

        str_result = s + "px";

    }

    else if (pre_expression.find("dp") != -1) {
        pre_expression.erase(remove(pre_expression.begin(), pre_expression.end(), 'p'), pre_expression.end());
        pre_expression.erase(remove(pre_expression.begin(), pre_expression.end(), 'd'), pre_expression.end());
        char exercise[1024];
        strncpy(exercise, pre_expression.c_str(), sizeof(exercise));
        exercise[sizeof(exercise) - 1] = 0;

        double result = function_cal(exercise);
        cout << result << "dp" << endl;
        ostringstream ss;
        ss << result;
        string s(ss.str());

        str_result = s + "dp";
    }

    else if (pre_expression.find("dip") != -1) {
        pre_expression.erase(remove(pre_expression.begin(), pre_expression.end(), 'p'), pre_expression.end());
        pre_expression.erase(remove(pre_expression.begin(), pre_expression.end(), 'd'), pre_expression.end());
        pre_expression.erase(remove(pre_expression.begin(), pre_expression.end(), 'i'), pre_expression.end());

        char exercise[1024];
        strncpy(exercise, pre_expression.c_str(), sizeof(exercise));
        exercise[sizeof(exercise) - 1] = 0;

        double result = function_cal(exercise);
        cout << result << "dip" << endl;
        ostringstream ss;
        ss << result;
        string s(ss.str());

        str_result = s + "dip";

    }

    else {
    }
    return str_result;

}