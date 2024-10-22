#include "HttpUtility.hpp"
#include "Iterator.hpp"
#include "adler32.hpp"
#include "json.hpp"
#include "type/pTypes.hpp"
#include <algorithm>
#include <deque>
#include <set>
#include <unordered_map>

using namespace Contain;
using namespace Iterator;

enum class op { unin = 0, connect, star };

pStack<AString> op;
pStack<AString> id;
constexpr const char *explaint = "PSONGEXPLAINT";
constexpr const char *none = "PSONGNONE";

pMap<AString, int> priority = pMap<AString, int>(
    pKeyValue<AString, int>('(', 0), pKeyValue<AString, int>('|', 1),
    pKeyValue<AString, int>('.', 2), pKeyValue<AString, int>('*', 3));

struct state {
  // vecotr vaild
  typedef KeyValuePair<AString,state> item;
  vector<item> translist;
  // multimap<AString, state> translist;
  vector<state> statelist;
  bool bAccept;
  bool bStart;
  bool bValueState;
  AString value = "";
  void AddTranslate(AString c, state s) { 
    // translist.emplace(c, s); 
    translist.push_back(item(c,s));
  }
  // state(bool Start) : bAccept(false), bStart(Start), bValueState(false) {
  //   plog(value);
  //   statelist.push_back(*this);
  // }
  state() {
    bStart = true;
    bAccept = false;
    bValueState = false;
    statelist.push_back(*this);
  }
  state(const AString &Value)
      : value(Value), bValueState(true), bStart(false), bAccept(false) {
    statelist.push_back(*this);
  }
};

struct FA {
  // deque<state> S;
  pStack<state> S;

  FA() { S.Push(state()); }

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

  // ab.a*b*|*.c.d. conver to suffix
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
            // plog(op.Peek());
            if (op.Peek() == "(")
              break;
            ret += op.Pop();
          }
          CONDITION_MESSAGE(op.Empty(), "unexpected (");
          op.Pop();
        } else {
          if (op.Peek() != '(') {
            if (priority[temp] <= priority[op.Peek()]) {
              // plog(temp," ",priority[temp]," ",op.Peek(),"
              // ",priority[op.Peek()]," ");
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

  auto FinalBuild(const AString &middle) {
    pStack<state> ret;
    for (int index = 0; index < middle._length(); index++) {
      auto temp = middle[index];
      // plog(temp);
      if (!IsSpecialInput(temp)) {
        ret.Push(state(AString(temp)));
      } else {
        if (temp == '.') {
          state s2 = ret.Pop();
          state s1 = ret.Pop();
          Connect(s1, s2);
          // plog(s1.statelist[0].translist.size());
          // plog(cret.translist.size());
          // plog(s1.statelist[0].translist.size());
          // ret.Push(Connect(s1, s2));
          ret.Push(s1);
        } else if (temp == '*') {
          state s1 = ret.Pop();
          Star(s1);
          // ret.Push(Star(s1));

          ret.Push(s1);
        } else if (temp == '|') {
          state s2 = ret.Pop();
          state s1 = ret.Pop();
          Union(s1, s2);
          // ret.Push(Union(s1, s2));
          ret.Push(s1);
        } else {
          plog("not support char");
        }
      }
    }
    CONDITION_MESSAGE(ret.size() < 1,
                      "stack exception: try to access a empty stack")
    plog(ret.size());
    auto top = ret.Pop();
    top.statelist[top.statelist.size() - 1].bAccept = true;
    plog(top.statelist.size());
    return top;
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
  state Union(state &s1, state &s2) {
    state start(explaint);
    state end(explaint);

    start.AddTranslate(explaint, s1.statelist[0]);
    start.AddTranslate(explaint, s2.statelist[0]);
    s1.statelist[s1.statelist.size() - 1].AddTranslate(explaint, end);
    s2.statelist[s2.statelist.size() - 1].AddTranslate(explaint, end);

    s2.statelist.push_back(end);
    s1.statelist.insert(s1.statelist.begin(), start);
    s1.statelist.insert(s1.statelist.end(), s2.statelist.begin(),
                        s2.statelist.end());

    S.Push(s1);
    return s1;
  }

  state Connect(state &s1, state &s2) {
    state start(explaint);
    state end(explaint);

    start.AddTranslate(explaint, s1.statelist[0]);
    s1.statelist[s1.statelist.size() - 1].AddTranslate(explaint,
                                                       s2.statelist[0]);
    s2.statelist[s2.statelist.size() - 1].AddTranslate(explaint, end);

    s2.statelist.push_back(end);
    s1.statelist.insert(s1.statelist.begin(), start);
    s1.statelist.insert(s1.statelist.end(), s2.statelist.begin(),
                        s2.statelist.end());

    S.Push(s1);
    return s1;
  }

  state Star(state &s1) {
    state start(explaint);
    state end(explaint);

    start.AddTranslate(explaint, s1.statelist[0]);
    s1.statelist[s1.statelist.size() - 1].AddTranslate(explaint, end);
    start.AddTranslate(explaint, end);
    s1.statelist[s1.statelist.size() - 1].AddTranslate(explaint,
                                                       s1.statelist[0]);

    s1.statelist.push_back(end);
    s1.statelist.insert(s1.statelist.begin(), start);

    S.Push(s1);
    return s1;
  }
#pragma endregion

  // bool isSame(const vector<state> &vPar, const vector<state> &vSub) {
  //   for (auto itSub = vSub.begin(); itSub != vSub.end(); itSub++) {
  //     for (auto itPar = vPar.begin(); itPar != vPar.end(); itPar++) {
  //       if(*itSub==*itPar) continue;
  //     }
  //     return false; 
  //   }
  //   return true;
  // }

  vector<state> CloserItem(state item) {
    vector<state> ret;
    // auto first = item.statelist[0];
    auto first = item;

    // plog(first.statelist[0].translist.size());
    // for(int index=0;index<first.statelist[0].translist.size();index++){
    //   plog(first.statelist[0].translist[index]._key," : ",first.statelist[0].translist[index]._value.value);
    // }
    
    ret.push_back(first);
    for(int index=0;index<first.translist.size();index++){
        // plog(first.translist[index]._key," : ",first.translist[index]._value.value);
        auto pair = first.translist[index];
        if(pair._key.Equal(explaint)){
        // ret.push_back(pair._value); //not  need !
        auto temp = CloserItem(pair._value);
        ret.insert(ret.end(),temp.begin(),temp.end());
      }
    }
    // for(auto pair=first.translist.begin();pair!=first.translist.end();pair++){
    //   if(pair->_key.Equal(explaint)){
    //     ret.push_back(pair->_value);
    //     auto temp = CloserItem(pair->_value);
    //     ret.insert(ret.end(),temp.begin(),temp.end());
    //   }
    // }
    return ret;
  }

  vector<state> move(vector<state> obj,const AString & input){
    
  }

  vector<state> CloserItems(vector<state> items) {}

  // set<state> explaintcloser(set<state> s){
  //   set<state> ret;
  //   for(auto it=s.begin();it!=s.end();it++){
  //     set<state> temp;
  //     auto m =it->translist.find(explaint);
  //     if(m==it->translist.end() ) continue;

  //     // auto m = it->translist.find(explaint);
  //     //
  //     for(m=it->translist.lower_bound(explaint);m!=it->translist.upper_bound(explaint);m++){
  //     //   // auto rss = ret.find(*m);
  //     //   // if(ret.find(*m)!=ret.end()) continue;
  //     //   temp.emplace(*m);
  //     // }
  //     // if(!temp.empty()){
  //     //   ret.emplace(temp);
  //     // }
  //   }
  //   return ret;
  // }
};



int main(int argc, char const *argv[]) {
  using namespace Utility;

  FA f;
  // auto test = "ab(a*|b*)*cd";
  auto test = "abc";
  auto ret = f.PreBuild(test);
  plog(ret);
  plog((ret = f.MiddleBuild(ret)));
  auto start = f.FinalBuild(ret);

  // for(int i=0;i<start.statelist.size();i++){
  //   plog(start.statelist[i].value);
  //   plog(start.statelist[i].translist.size())
  // }
  // plog(f.S.Peek().translist.size());
  auto temp = f.CloserItem(start.statelist[0]);
  // plog(temp.size());
  for(int i=0;i<temp.size();i++){
    plog(temp[i].value);
  }

  // plog(temp[0].value);
  // plog(temp[1].value);
  // plog(temp[0].translist);

  // for (auto it = temp.begin(); it != temp.end(); it++) {
  //   plog(it->value);
  // }
  // AString test = "abacd";
  // for (int index = 0; index < test._size(); index++) {
  //   // temp
  // }
  return 0;

}


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
