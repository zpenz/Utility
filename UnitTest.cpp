#include "Contain.h"
#include "pString.hpp"
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <stdarg.h>
#include <sys/stat.h>

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
    Filed   PrimaryKey;
    AString OtherTableDesc;
    vector<Filed> FiledList;
    vector<ForeignKey> ForeignList;
};

constexpr const char* REPEATCOND = "%NOREPEAT%";

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
                        it += 3;
                        temp.PrimaryKey.FiledName = *it++;
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
                            while(!it->Equal("'"))
                                filed.FiledComment += *it++;
                            SHOW_MESSAGE(filed.FiledComment,1);
                        }
                        filed.OtherDesc+=*it;

                        if(it->Equal(",")) break;
                        it++;
                    }
                    
                    if(PushFlag) 
                        temp.FiledList.push_back(filed); 
                }

                //Set Primary Key
                for(int index=0;index<temp.FiledList.size();index++){
                    if(temp.FiledList.at(index).FiledName.Equal(temp.PrimaryKey.FiledName)) { 
                        temp.PrimaryKey = temp.FiledList.at(index); 
                        break;
                    }
                }

                mCreateList.push_back(temp);
            }
        }


    }

    static void Test(){
        AString ret = "<?php\n    include_once(\"../common.php\");\n    include_once(\"../const.php\");\n";
        ret+=AString("    //This File is Generate Auto By Test")+".\n\n";
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

        auto Normalize = [](AString subject)->AString{
            subject.ToLowwer();
            if(subject._length()>0){
                subject = ((AString)subject[0]).ToUpper()+subject.substr(1,subject._length()-1);
            }
            int pos = -1;
            while(-1!=(pos=subject.find("_"))){
                subject = subject.substr(0,pos)+((AString)subject[pos+1]).ToUpper()+subject.substr(pos+2,subject._length()-pos-2);
            }
            return subject;
        };
        
        for(int index=0;index<TableSize;index++){
            AString TableName = Parse::mCreateList.at(index).TableName;
            AString ClassName = TableName;
            AString FolerName = ClassName;
            AString GFileName = FolerName+"/test.php";
            mkdir(FolerName.c_str(),0755);
            Filed PrimaryKey  = Parse::mCreateList.at(index).PrimaryKey;
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
            ret+="        }\n\n";

            //Check
            ret+="        public function Check(){\n            ";
            for(int indexY =0;indexY<FiledSize;indexY++){
                AString TempFiledName = FiledList.at(indexY).FiledName;
                if(FiledList.at(indexY).FiledComment.Contain(REPEATCOND)){
                    ret+= "if(!empty($this->"+TempFiledName+")){\n                $ret=MysqlHelper::S_IsEmptySet(\"SELECT * FROM "+TableName+" WHERE "+TempFiledName+" = ?\",'"+ToType(TempFiledName)+"',$this->"+TempFiledName+");\n";
                    ret+= "                if($ret->resultcode<0) return $ret;\n                if($ret->data !=0) return new ReturnObject(-233,\""+TempFiledName+" can not repeat\");\n            }\n\n            ";
                }
            }
            ret+= "return new ReturnObject(0,\"\");\n        }\n\n";

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
            //Add Action
            AString AddCode = "<?php \n    include_once(\'"+GFileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return (new "+ClassName+"(\n            ";
            for(int indexY=0;indexY<FiledSize;indexY++){
                if(indexY == FiledSize - 1){
                    AddCode+= "GetRealValueSafe(\""+FiledList.at(indexY).FiledName+"\")\n            ))->Add();\n    })());";
                }
                else
                    AddCode+="GetRealValueSafe(\""+FiledList.at(indexY).FiledName+"\"),\n            ";
            }
            auto AddFile = fopen(FolerName+"/Add.php","wr");
            fwrite(AddCode.c_str(),sizeof(char),AddCode._length(),AddFile);
            fclose(AddFile);

            //TotalCount
            ret+="\n        public static function GetTotalCount(){\n            return MysqlHelper::SafeQueryResult(\"SELECT count(*) AS 'count' FROM "
            + TableName + " WHERE 1 = ?\",'i',1);\n        }\n";
            //Count Action
            AString CountCode = "<?php \n    include_once(\'"+GFileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return "+ClassName+"::GetTotalCount();\n    })());";
            auto CountFile = fopen(FolerName+"/Count.php","wr");
            fwrite(CountCode.c_str(),sizeof(char),CountCode._length(),CountFile);
            SAFE_CLOSE(CountFile);

            //GetList temp
            ret+="\n        public static function GetList($page,$num=10000){\n            $start = $num*($page-1);\n            MysqlHelper::$UseUTF8 = true;\n            ";
                    ret+="return MysqlHelper::SafeQueryResult(\"SELECT * FROM "+TableName+" LIMIT ?,?\",\"dd\",$start,$num);\n        }\n\n";
            //GetList Action
            AString GetListCode = "<?php \n    include_once(\'"+GFileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return "+ClassName+"::GetList(\n            GetRealValueSafe('page',1),\n            GetRealValueSafe('num',10000));\n    })());";
            auto GetListFile = fopen(FolerName+"/GetList.php","wr");
            fwrite(GetListCode.c_str(),sizeof(char),GetListCode._length(),GetListFile);
            SAFE_CLOSE(GetListFile);
            
            //Find
            ret+="        //FIND FUNCTIONS START----------------------------------------------------\n";
            for(int indexY = 0;indexY<FiledSize ;indexY++){
                // SHOW_MESSAGE(FiledList.at(indexY).FiledComment.c_str(),1);
                // if(FiledList.at(indexY).FiledComment.Contain(FINDCONDITION)){
                    ret+="        public static function FindBy"+Normalize(FiledList.at(indexY).FiledName.ToUpperN())+"($value,$page=1,$num=10000){\n            $start = $num*($page-1);\n            MysqlHelper::$UseUTF8 = true;\n            ";
                    ret+="$count = MysqlHepler::SafeQueryResult(\"SELECT count(*) AS 'count' FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ?\",$value);\n            ";
                    ret+="$ret   = MysqlHelper::SafeQueryResult(\"SELECT * FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ? LIMIT ?,? \",\'"+ToType(FiledList.at(indexY).FiledType)+"dd\'"+",$value,$start,$num);\n            ";
                    ret+="array_push($ret,$count);\n            return $ret;\n        }\n";
                // }
            }
            ret+="        //FIND FUNCTIONS END----------------------------------------------------\n\n        ";
            //Find Action 
            for(int indexY=0;indexY<FiledSize;indexY++){
                AString temp = Normalize(FiledList.at(indexY).FiledName);
                AString FindCode = "<?php \n    include_once(\'"+GFileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return "+ClassName+"::FindBy"+temp+"(\n            GetRealValueSafe('value'),\n            GetRealValueSafe('page',1),\n            GetRealValueSafe('num',10000));\n    })());";
                SHOW_MESSAGE((FolerName+"/FindBy"+temp+".php").c_str(),1);
                auto FindFile = fopen((FolerName+"/FindBy"+temp+".php").c_str(),"wr");
                SHOW_MESSAGE(FindFile,1);
                fwrite(FindCode.c_str(),sizeof(char),FindCode._length(),FindFile);
                SAFE_CLOSE(FindFile);
            }

            //Delete
            ret+="public static function Delete($unique){\n            $ret = MysqlHelper::S_IsEmptySet(\"SELECT * FROM "+TableName+" WHERE "+ PrimaryKey.FiledName + " = ?\",'"+ToType(PrimaryKey.FiledType)+"',$unique);\n            if($ret->resultcode<0) return new ReturnObject(-2,\"delete a no exist record\");\n            ";
            ret+="//TODO: if have file link . delete link before delete record!\n            $ret = MysqlHelper::SafeQueryResult(\"SELECT * FROM "+TableName+" WHERE "+PrimaryKey.FiledName + "= ? \",'"+ToType(PrimaryKey.FiledType)+"',$unique);\n            if($ret->resultcode<0) return $ret;\n            \n            return MysqlHelper::SafeQuery(\"DELETE FROM "+TableName+" WHERE "+PrimaryKey.FiledName+" = ?\",'"+ToType(PrimaryKey.FiledType)+"',$unique);\n        }\n\n";
            AString DeleteCode = "<?php \n    include_once(\'"+GFileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return "+ClassName+"::Delete(\n            GetRealValueSafe('value')\n        );\n    })());\n";
            auto DeleteFile = fopen(FolerName+"/Delete.php","wr");
            fwrite(DeleteCode.c_str(),sizeof(char),DeleteCode._length(),DeleteFile);
            SAFE_CLOSE(DeleteFile);

            ret+="    }\n";
            auto file = fopen(GFileName,"wr");
            fwrite(ret.c_str(),sizeof(char),ret._length(),file);
            SAFE_CLOSE(file);

            break;
        }
    }
};

vector<AString> Parse::mList;
vector<CreateExpression> Parse::mCreateList;

int main(int argc, char const *argv[])
{

    {
        Parse::init("xk_show.sql");
        Parse::parse();
        Parse::Test();

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

