#include <iostream>
#include <stdio.h>

using namespace std;

enum TokenType
{
    BOOM_ERROR = -1, // 啊哈，出错了
    NUMBER = 1,
    IDENTIFIER = 2,
    IF = 4
};

int DFATable[][37] = {
    // 0 1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u v w x y z !
      {1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,-1}, // s0 -- 起始状态
      {1,1,1,1,1,1,1,1,1,1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, // s1 -- 到这里说明是数字
      {3,3,3,3,3,3,3,3,3,3,2,2,2,2,2,4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,-1}, // s2 -- 变量
      {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,-1},
      {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,-1} // s4 -- 这是IF
};

// 
// Match:
// 给定一个字符串str，判断这个字符串的类型
// 
// 例子:
// if, 返回IF
// 数字,返回NUMBER
// 变量,返回IDENTIFIER
// 
TokenType Match(string str)
{
    int state = 0;

    for (string::iterator iter = str.begin();
        iter != str.end();
        ++iter )
    {
        char c = *iter;
        int index = 0;
        if ( c >= '0' && c <= '9' )
        {
            index = c - '0';
        }
        else if (c >= 'a' && c <= 'z')
        {
            index = c - 'a' + 10; // a列在DFATable中的索引值
        }
        else 
        {
            index = 36; // !列在DFATable中的索引值，到这里说明不匹配了
        }

        state = DFATable[state][index];

        if (state == BOOM_ERROR)
            break;
    }

    return (TokenType)state;
}

int g_line = 0;
void Print(TokenType type)
{
    switch (type)
    {
        case BOOM_ERROR:
            cout << ++g_line << ": BOOM_ERROR/n" <<endl;
            break;

        case IF:
            cout << ++g_line << ": IF/n" <<endl;
            break;

        case NUMBER:
            cout << ++g_line << ": NUMBER/n" <<endl;
            break;

        case IDENTIFIER:
            cout << ++g_line << ": IDENTIFIER/n" <<endl;
            break;

        default:
            cout << ++g_line << ": Error/n" <<endl;
            break;
    }
}

int main()
{
    Print(Match("if"));
    Print(Match("iff"));
    Print(Match("if0"));
    Print(Match("0if"));
    Print(Match("i0f"));
    Print(Match("ia"));
    Print(Match("01")); 
    Print(Match("123")); 
    Print(Match("1f")); 
    Print(Match("abcd")); 
    Print(Match("ab")); 
    Print(Match("a")); 
    Print(Match("0")); 
    Print(Match("i"));
    Print(Match("_"));

    return 0;
}