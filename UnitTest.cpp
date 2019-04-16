#include "Contain.h"
#include "pString.hpp"
#include <vector>
#include <cstdio>

using namespace Contain;

enum class TokenFlag{
    FLAG_COMMENT,
    FLAG_BLANK,
    FLAG_PUNCUATION,
    FLAG_ACTION,
    FLAG_NAME,
    FLAG_TABLE_FILED_NAME,
    FLAG_TABLE_FILED_TYPE_NAME,
    FLAG_TABLE_FILED_SIZE,
    FLAG_TABLE_FILED_ATTRIBUTE,
    FLAG_TABLE_PRIMARY_KEY,
    FLAG_TABLE_FILED_COMMENT,
    FLAG_TABLE_OTHER_ATTRIBUTE,
    FLAG_OTHER
};

struct Token{
    TokenFlag flag;
    WString   content;
    Token(TokenFlag tf):flag(tf){}
    Token(){}
    virtual bool is(WString ct) = 0;
    virtual ~Token(){};
};

struct ActionToken:Token{
    AString action;
    ActionToken(AString ac):Token(TokenFlag::FLAG_ACTION),action(ac){};
};

struct NameToken:Token{
    AString action;
    NameToken(AString ac):Token(TokenFlag::FLAG_NAME),action(ac){};
};

struct Filed{
    AString FiledName;
    AString FiledComment;
    AString OtherDesc;
    AString FiledType;
    AString FiledSize;
};

struct ForeignKey{
    AString ForeignKeyName;
    AString ForeignReferenceTable;
};

struct CreateExpression{
    AString TableName;
    AString PrimaryKey;
    AString OtherTableDesc;
    vector<Filed> FiledList;
    vector<ForeignKey> ForeignList;
};

constexpr const char* FINDCONDITION = "%FINDCOND%";

class Parse{
    public:
    static vector<AString> mList;
    static vector<CreateExpression> mCreateList;

    template<typename U>
    static size_t SkipBlank(U* src){
        U* temp = src;
        while(*src && (*src == ' ' || *src=='\r' || *src=='\n')) src++;
        return src-temp-1;
    }

    static void init(const char * filename){
        auto file = fopen(filename,"r");
        char buf[2048];
        int size = 0;
        AString temp = "";
        while(1){
            size=fread(buf,sizeof(char),sizeof(buf),file);
            
            for(int index =0;index<size;index++){

                if(buf[index] == ' ' || buf[index] == '\r' || buf[index] == '\n') 
                {
                    if(!temp.Empty()) mList.push_back(temp);
                    temp = "";
                    index += SkipBlank(&buf[index]);
                }else if(buf[index] == ';' || buf[index] == '\'' || buf[index] == '(' || buf[index] == ')' || buf[index] == '=' || buf[index] == ','){
                    if(temp._length() > 1){
                        mList.push_back(temp.substr(0,temp._length()));
                        mList.push_back(buf[index]);
                    }else{
                        mList.push_back(buf[index]);
                    }
                    temp = "";
                }else{
                    temp+=buf[index];
                }
            }
            if(feof(file)) break;
        }
        fclose(file);
    } 

    static void parse(){
        decltype(mList)::iterator it;

        for(it = mList.begin();it!=mList.end();it ++){
            if((it->ToUpper()) == "CREATE" && it+1 != mList.end() && (it+1)->ToUpper() == "TABLE"){
                it+=2;
                CreateExpression temp;

                temp.TableName = *it++;
                it++;
                while(1){
                    if(it->Equal(")") && (it+1)!=mList.end() && (it+1)->Equal(")")) break;
                    Filed filed;
                    bool  PushFlag = true;

                    if(it->Equal(",")) it++;
                    if(it->ToLowwer().Equal("primary") && (it+1)->ToLowwer().Equal("key")){
                        it += 2;
                        temp.PrimaryKey = *it++;
                        PushFlag = false;
                        goto next;
                    }
                    if(it->ToLowwer().Equal("foreign") && (it+1)->ToLowwer().Equal("key")){
                        it += 2;
                        ForeignKey fKey;
                        fKey.ForeignKeyName = *it++;
                        it += 2;
                        fKey.ForeignReferenceTable = *it;
                        temp.ForeignList.push_back(fKey);
                        PushFlag = false;
                        goto next;
                    }

                    filed.FiledName = *it++;
                    // SHOW_MESSAGE(filed.FiledName.c_str(),1);
                    filed.FiledType = *it++;
                    // SHOW_MESSAGE(filed.FiledType.c_str(),1);

                    if(!filed.FiledType.Equal("longtext")){
                        it++;
                        filed.FiledSize = *it++;
                        // SHOW_MESSAGE(filed.FiledSize.c_str(),1);
                        it++;
                    }

next:
                    while(it!=mList.end()){
                        if(it->Equal(")") && (it+1)!=mList.end() && (it+1)->Equal(")")) break;

                        if((it->ToUpperN()).Equal("COMMENT")){
                            it+=2;
                            filed.FiledComment = *it++;
                            SHOW_MESSAGE(filed.FiledComment,1);
                        }
                        filed.OtherDesc+=*it;

                        if(it->Equal(",")) break;
                        it++;
                    }
                    
                    if(PushFlag) 
                        temp.FiledList.push_back(filed); 
                }
                mCreateList.push_back(temp);
            }
        }
    }

    static void Test(){
        AString ret = "<?php\n    include_once(\"../common.php\");\n    include_once(\"../const.php\");\n";
        ret+=AString("    //This File is Generate Auto By ")+__FILE__+".\n\n";
        size_t TableSize = Parse::mCreateList.size();

        auto ToType = [](AString & subject)->AString{
            subject.ToLowwer();
            if(subject.Equal("varchar")||subject.Equal("longtext"))
                return 's';
            else if(subject.Equal("int") || subject.Equal("long") || subject.Equal("small"))
                return 'i';
            else if(subject.Equal("float") || subject.Equal("double"))
                return 'd';
            return 's';
        };
        
        for(int index=0;index<TableSize;index++){
            AString TableName = Parse::mCreateList.at(index).TableName;
            auto   ThisFiled  = Parse::mCreateList.at(index);
            auto   FiledList  = Parse::mCreateList.at(index).FiledList;
            size_t FiledSize  = FiledList.size();
            ret+="    //TableName: "+TableName+"\n";
            ret+="    Class "+TableName+"{\n";
            for(int indexY = 0;indexY<FiledSize;indexY++){
                ret+="        public $"+Parse::mCreateList.at(index).FiledList.at(indexY).FiledName+";\n";
            }
            ret+="\n";
            //construct
            ret+="        public function __construct(";
            for(int indexY = 0;indexY<FiledSize;indexY++){
                AString temp;
                if(indexY == FiledSize-1) 
                    temp = "){\n";
                else temp =",";
                ret+= "$"+Parse::mCreateList.at(index).FiledList.at(indexY).FiledName+temp;
            }
            for(int indexY = 0;indexY<FiledSize;indexY++){
                ret+= "            $this->"+Parse::mCreateList.at(index).FiledList.at(indexY).FiledName+" = $"+Parse::mCreateList.at(index).FiledList.at(indexY).FiledName+";\n";
            }
            ret+="        }\n";

            //Check
            ret+="       public function Check(){\n            return new ReturnObject(0,\"\");\n        }\n";

            //Add
            ret+="       public function Add(){\n            $ret = $this->Check();\n            if($ret->resultcode!=0) return $ret;\n            MysqlHelper::$UseUTF8 =true;\n\n            ";
            ret+="$insertSql = \"INSERT INTO "+TableName+"(\n                ";
            for(int indexY = 0;indexY<FiledSize;indexY++){
                AString temp;
                if(indexY == FiledSize-1) {
                    temp = ")VALUES(";
                    for(int indexZ = 0;indexZ <FiledSize;indexZ++){
                        temp+=(indexZ==FiledSize-1?"?)\";\r":"?,");
                    }
                }
                else temp =",\n                ";
                ret+=FiledList.at(indexY).FiledName+temp;
            }
            ret+="\n\n            return MysqlHelper::SafeQuery($insertSql,\"";
            for(int indexY = 0;indexY<FiledSize;indexY++){
                AString aFiledType = FiledList.at(indexY).FiledType.ToLowwer();
                ret+=ToType(aFiledType);
                if(indexY == FiledList.size()-1){
                    ret+="\",\n                ";
                }
            }

            for(int indexY = 0;indexY<FiledSize;indexY++){
                if(indexY == FiledList.size()-1){
                    ret+="$this->"+FiledList.at(indexY).FiledName+");\n";
                }else
                {
                    ret+="$this->"+FiledList.at(indexY).FiledName+",\n                ";     
                }
            }
            ret+="        }\n";

            //TotalCount
            ret+="\n        public static function GetTotalCount(){\n            return MysqlHelper::SafeQueryResult(\"SELECT count(*) AS 'count' FROM "
            + TableName + " WHERE 1 = ?\",'i',1);\n        }\n";

            //GetList temp
            ret+="\n        public static function GetList($page,$num=10000){\n            $start = $num*($page-1);\n            MysqlHelper::$UseUTF8 = true;\n            ";
                    ret+="return MysqlHelper::SafeQueryResult(\"SELECT * FROM "+TableName+" LIMIT ?,?\",\"dd\",$start,$num);\n        }\n\n";
            
            //Find
            ret+="        //FIND FUNCTIONS START----------------------------------------------------\n";
            for(int indexY = 0;indexY<FiledSize ;indexY++){
                // SHOW_MESSAGE(FiledList.at(indexY).FiledComment.c_str(),1);
                // if(FiledList.at(indexY).FiledComment.Contain(FINDCONDITION)){
                    ret+="        public static function FindBy"+FiledList.at(indexY).FiledName.ToUpperN()+"($value,$page=1,$num=10000){\n            $start = $num*($page-1);\n            MysqlHelper::$UseUTF8 = true;\n            ";
                    ret+="$count = MysqlHepler::SafeQueryResult(\"SELECT count(*) AS 'count' FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ?\",$value);\n            ";
                    ret+="$ret   = MysqlHelper::SafeQueryResult(\"SELECT * FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ? LIMIT ?,? \",\'"+ToType(FiledList.at(indexY).FiledType)+"dd\'"+",$value,$start,$num);\n            ";
                    ret+="array_push($ret,$count);\n            return $ret;\n        }\n";
                // }
            }
            ret+="        //FIND FUNCTIONS END----------------------------------------------------\n";
            ret+="    }\n";

            auto file = fopen("test.php","wr");
            fwrite(ret.c_str(),sizeof(char),ret._length(),file);
            fclose(file);
            
            break;
        }
    }
};

vector<AString> Parse::mList;
vector<CreateExpression> Parse::mCreateList;

int main(int argc, char const *argv[])
{

    {
        // Parse::init("xk_show.sql");
        // Parse::parse();
        // Parse::Test();

        AString test = "1234567812131230002300";
        // test.replace("23","");
        auto test2 = test.replace("23","test---");
        SHOW_MESSAGE(test2._length(),1);
        SHOW_MESSAGE(test2.c_str(),1);
        // test.replace("23","xc");
        // SHOW_MESSAGE(test.c_str(),1);
        // SHOW_MESSAGE(test._length(),1);

        // SHOW_MESSAGE(Parse::mCreateList.size(),1);
        
        // for(int index=0;index<Parse::mCreateList.size();index++){
        //      SHOW_MESSAGE(Parse::mCreateList.at(index).TableName,1);
        //  }
        // SHOW_MESSAGE(Parse::mList.size(),1);
        // for(int index=0;index<Parse::mList.size();index++){
        //     SHOW_MESSAGE(Parse::mList.at(index).c_str(),1);
        // }
    }

    return 0;
}

