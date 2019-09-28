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
};

AString PreBuild(const AString &expr) {
  if (expr._length() == 0)
    return "";
  AString ret = "";
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

struct Test : public Reflect<Test> {
  Test(){};
  void test() { show_message("ok"); }
};

int main(int argc, char const *argv[]) {
  using namespace Utility;
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

  //----
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

  // plog(PreBuild("ab(a*|b*)*cd"));

  pdouble d = 10.0;
  pint a = 10;
  a += 10.0;
  d += 100;
  d = d + a;
  // d=d+a;
  AString z = "zz";

  createfunc s;
  plong l = 100;

  JObject obj;
  JAarry list = JAarry();

  list.Add(KeyValue("zz", 123), KeyValue("cc", "dd"));
  list.Add(KeyValue("zz", 123), KeyValue("cc", "dd"));
  obj.Add("nihao", 123, "zzz", "oo", "test", 230.0101, "list",
          JAarry(KeyValue(list)));

  plog(obj.Serial());
  auto ret = JObject::Parse(
      "{\"begin_line\":0,\"count\":2,\"file_list\":[{\"file\":\"/var/share/mp/"
      "xkraid1569382834/12\"},{\"file\":\"/var/share/mp/xkraid1569382834/"
      "34\",\"origin\":\"heiheihei\"}],\"error_code\":0}");

  plog("error_code", ret["error_code"].value);
  plog(ret["file_list"].list[1]["origin"].value);

  FY.create<plong>();
  plog(AString::ToD("1234.03"));
  // show_message(typeid(Test).name());
  return 0;
}
