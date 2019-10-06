#include "HttpUtility.hpp"
#include "Iterator.hpp"
#include "adler32.hpp"
#include "json.hpp"
#include <algorithm>
#include <deque>
#include <map>

#include "type/pTypes.hpp"

using namespace Contain;
using namespace Iterator;

enum class op { unin = 0, connect, star };

pStack<AString> op;
pStack<AString> id;
constexpr const char *explaint = "PSONGEXPLAINT";
constexpr const char *none = "PSONGNONE";

// map<AString, int> priority = {
//   // {'(',0},
//   // {'|',1},
//   // {'(',2},
//   // {'*',3}
// };

pMap<AString,int> priority = pMap<AString,int>(
  pKeyValue<AString,int>('(',0),
  pKeyValue<AString,int>('|',1),
  pKeyValue<AString,int>('.',2),
  pKeyValue<AString,int>('*',3)
  );


struct state {
  map<AString, state> translist;
  vector<state> statelist =
      vector<state>(); // one or more state view as one state
  bool bAccept;
  bool bStart;
  bool bValueState;
  AString value = "";
  void AddTranslate(AString c, state s) { translist[c] = s; }
  state(bool Start) : bAccept(false), bStart(Start), bValueState(false) {}
  state() {
    bStart = true;
    bAccept = false;
    bValueState = false;
  }
  state(AString Value)
      : value(Value), bValueState(true), bStart(false), bAccept(false) {}
};

struct FA {
  deque<state> S;

  FA() { S.push_back(state()); }

  AString PreBuild(const AString &expr) {
    if (expr._length() == 0)
      return "";
    AString ret = "", realRet = "";
    for (int index = 0; index < expr._length(); index++) {
      auto temp = expr[index];
      ret += temp;
      if (temp != '(' && temp != '|' && index + 1 < expr._length())
        if (expr[index + 1] != '|' && expr[index + 1] != ')' &&
            expr[index + 1] != '*') {
          ret += '.';
        }
    }
    return ret;
  }

  // ab.a*b*|*.c.d.
  AString MiddleBuild(const AString &prev) {

    AString ret = "";
    pStack<AString> op;
    for (int index = 0; index < prev._length(); index++) {
      auto temp = prev[index];
      if (!IsSpecialInput(temp))
        ret += temp;
      else if (op.Empty()) {
        op.Push(temp);
      } else {
        if (temp == '(') {
          op.Push(temp);
        } else if (temp == ')') {
          while (!op.Empty()) {
            plog(op.Peek());
            if (op.Peek() == "(")
              break;
            ret += op.Pop();
          }
          CONDITION_MESSAGE(op.Empty(), "unexpected (");
          op.Pop();
        } else {
          if (op.Peek() != '(') {
            if (priority[temp] <= priority[op.Peek()]) {
              plog(temp," ",priority[temp]," ",op.Peek()," ",priority[op.Peek()]," ");
              ret += op.Pop();
              op.Push(temp);
            } else {
              ret += temp;
            }
          } else {
            op.Push(temp);
          }
        }
      }
    }
    while (!op.Empty())
      ret += op.Pop();
    return ret;
  }

  bool IsSpecialInput(const char c) {
    return (c == '*' || c == '|' || c == '.' || c == '(' || c == ')');
  }

  void build(const AString &expr) {
    auto src = PreBuild(expr);
    int index = 0;

    while (index < src._length()) {
      auto temp = src[index];
      if (temp == '\\') {
        if (index + 1 > src._length() - 1) {
          plog("unexpceted '\\'");
        }
        id.Push(src[index + 1]);
        index += 2;
        continue;
      } else if (op.Empty()) {
        op.Push(temp);
      } else if (IsSpecialInput(temp)) {
        op.Push(src[index]);
      }
    }
  }

#pragma region state op
  void Union(AString s1, AString s2) {
    state state1(false);
    state state2(false);
    state state3(false);
    state state4(false);
    state state5(false);
    state state6(false);

    state1.AddTranslate(explaint, state2);
    state2.AddTranslate(s1, state3);
    state3.AddTranslate(explaint, state6);
    state1.AddTranslate(explaint, state4);
    state4.AddTranslate(s2, state5);
    state5.AddTranslate(explaint, state6);

    state1.statelist.push_back(state1);
    state1.statelist.push_back(state2);
    state1.statelist.push_back(state3);
    state1.statelist.push_back(state4);
    state1.statelist.push_back(state5);
    state1.statelist.push_back(state6);

    S.push_back(state1);
  }

  void Connect(AString s1, AString s2) {
    state state1(false);
    state state2(false);
    state state3(false);
    state state4(false);

    state1.AddTranslate(s1, state2);
    state2.AddTranslate(explaint, state3);
    state3.AddTranslate(s1, state4);

    state1.statelist.push_back(state1);
    state1.statelist.push_back(state2);
    state1.statelist.push_back(state3);
    state1.statelist.push_back(state4);

    S.push_back(state1);
  }

  void Star(AString s1) {
    state state1(false);
    state state2(false);
    state state3(false);
    state state4(false);

    state1.AddTranslate(explaint, state2);
    state1.AddTranslate(explaint, state4);
    state2.AddTranslate(s1, state3);
    state3.AddTranslate(explaint, state2);
    state3.AddTranslate(explaint, state4);

    S.push_back(state1);
  }
#pragma endregion
};

bool IsOperator(char ch) {
  return ((ch == 42) || (ch == 124) || (ch == 40) || (ch == 41) || (ch == 8));
};
//! Checks if the specific character is input character
bool IsInput(char ch) { return (!IsOperator(ch)); };

//! Checks is a character left parantheses
bool IsLeftParanthesis(char ch) { return (ch == 40); };

//! Checks is a character right parantheses
bool IsRightParanthesis(char ch) { return (ch == 41); };

string ConcatExpand(string strRegEx) {
  string strRes;

  for (int i = 0; i < strRegEx.size() - 1; ++i) {
    char cLeft = strRegEx[i];
    char cRight = strRegEx[i + 1];
    strRes += cLeft;
    if ((IsInput(cLeft)) || (IsRightParanthesis(cLeft)) || (cLeft == '*'))
      if ((IsInput(cRight)) || (IsLeftParanthesis(cRight)))
        strRes += '.';
  }
  strRes += strRegEx[strRegEx.size() - 1];

  return strRes;
}

int main(int argc, char const *argv[]) {
  using namespace Utility;

  // Test file sync
  // Utility::JObject obj;
  // AString a = 1;
  // obj.Add("where",1);
  // auto ret = obj.Serial();
  // plog(ret);

  // Linker<AString> params;
  // params.Add("file");
  // params.Add("md5.h");

  // Utility::JObject job;
  // job.Add("admin_id",1001,"sid","Hd3M1a1jII4MZeUx5fIKswfgWoFVH9l","mount_point","xkraid1560365017","begin_line",1);
  // plog(job.Serial());

  // auto ret = LoadDiff<diff>("verify27262990");
  // SaveFile<diff>("diff",CalcFileSlideDiff("test.test"));
  // for_each(ret.begin(),ret.end(),[](diff& item){
  //     plog("index: ",item.index," rvale: ",item.rvalue," md5:
  //     ",item.MD5Value);
  // });

  // vector<range> oplist = performMarge("test.test","verify27263090");

  // auto oplist = LoadRange("");
  // for_each(oplist.begin(),oplist.end(),[](range& rg){
  //     plog(rg.index," ",rg.length," ",rg.sameblock," ",rg.offset);
  // });

  // Test FormPost
  // Linker<AString> params;
  // Utility::JObject obj;
  // obj.Add("admin_id",1001,"sid","npJ8cS1q25M9vQcXXW02H9Te2G2L7O7","directory_path","/var/share/mp/xklvm1562233594/wxy/xk","all_file_name","/var/share/mp/xklvm1562233594/wxy/xk/UtilityTest.cpp");
  // params.Add("json",obj.Serial());
  // // params.Add("file","test.test.op");

  // auto url = "http://192.168.10.23:555/cgi-bin/xk_file_rsync_verify.cgi";
  // auto url = "192.168.10.250:9000/x";
  // FormPostTest(url,params,99999,TransListener(nullptr,[&](AString string){
  //     plog("result: ",string._length()," content:",string);

  // },nullptr,
  // nullptr
  // ),[](Utility::Request& req){
  //             Utility::JObject job;
  //     job.Add("admin_id",1001,"sid","DbsG1J7TqfnJE4B63VRPgD3otHsQe1I","path_name","/var/share/mp/xklvm1562233594/wxy/xk/","file_name","UnitTest.cpp");
  //     req.OtherRecord.Add(Utility::KV("XK_JSON",job.Serial()));
  // });

  // Test Json
  // pdouble d = 10.0;
  // pint a = 10;
  // a += 10.0;
  // d += 100;
  // d = d + a;
  // // d=d+a;
  // AString z = "zz";

  // createfunc s;
  // plong l = 100;

  // JObject obj;
  // JAarry list = JAarry();

  // list.Add(KeyValue("zz", 123), KeyValue("cc", "dd"));
  // list.Add(KeyValue("zz", 123), KeyValue("cc", "dd"));
  // obj.Add("nihao", 123, "zzz", "oo", "test", 230.011201, "list",
  //         JAarry(KeyValue(list)));

  // plog(obj.Serial());
  // auto ret = JObject::Parse(
  //     "{\"begin_line\":0,\"count\":2,\"file_list\":[{\"file\":\"/var/share/mp/"
  //     "xkraid1569382834/12\"},{\"file\":\"/var/share/mp/xkraid1569382834/"
  //     "34\",\"origin\":\"heiheihei\"}],\"error_code\":0}");

  // plog("error_code", ret["error_code"].value);
  // plog(ret["file_list"].list[1]["origin"].value);

  // FY.create<plong>();
  // plog(AString::ToD("1234.03"));
  // plog(1234.046);
  // show_message(typeid(Test).name());

  // plog(ConcatExpand("ab(a*|b*)*cd"));
  FA f;
  auto ret = f.PreBuild("ab(a*|b*)*cd");
  plog(ret);
  plog(f.MiddleBuild(ret));

  // priority['*'] = 0;
  // priority['|'] = 1;
  // priority['.'] = 2;
  // priority['*'] = 3;

  // for(auto it = priority.begin();it!=priority.end();it++){
  //   plog(it->first," ",it->second);
  // }
  return 0;
}
