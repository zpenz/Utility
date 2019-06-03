#include "pString.hpp"
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <stdarg.h>
#include <sys/stat.h>

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

constexpr const char* REPEATCOND  = "%NOREPEAT%";
constexpr const char* FILEREFCOND = "%FILEREF%";
constexpr const char* ALTERCOND   = "%ALTER%";


class Parse{
    public:
    static vector<AString> mList;
    static vector<CreateExpression> mCreateList;
    static bool   twocond;

    template<typename U>
    static size_t SkipBlank(U* src){
        U* temp = src;
        while(*src && (*src == ' ' || *src=='\r' || *src=='\n')) src++;
        return src-temp-1;
    }

    static bool IsInterface(const AString & src){
        return src.Equal("'") || src.Equal('`') ? true : false;
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
                }else if(buf[index] == ';' || buf[index] == '\'' || buf[index] == '(' || buf[index] == ')' || buf[index] == '=' || buf[index] == ',' || buf[index] == '`'){
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

        auto SkipSingle = [&](decltype(it) temp){
            while(it->Equal("'") || it->Equal("`")){
                it++;
            }
            return it;
        };

        for(it = mList.begin();it!=mList.end();it ++){
            if((it->ToUpper()) == "CREATE" && it+1 != mList.end() && (it+1)->ToUpper() == "TABLE"){
                it+=2;
                CreateExpression temp;
                
                SkipSingle(it);
                temp.TableName = *it++;
                SkipSingle(it);
                it++;
                while(1){
                    if(it->Equal(")") && (it+1)!=mList.end() && (it+1)->Equal(")")) break;
                    Filed filed;
                    bool  PushFlag = true;
                    
                    if(it->Equal(",")) it++;
                    if(it->ToLowwer().Equal("primary") && (it+1)->ToLowwer().Equal("key")){
                        it += 3;
                        SkipSingle(it);
                        temp.PrimaryKey.FiledName = *it++;
                        SkipSingle(it);
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

                    SkipSingle(it);
                    filed.FiledName = *it++;
                    SkipSingle(it);
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

        AString strBasePath = "dirname(__FILE__).'/'";
        
        for(int index=0;index<TableSize;index++){
            AString ret = "<?php\n    //=============This File Is Auto Generated By Test, Please Do Not Modify It.==============\n    include_once("+strBasePath+".\"../common.php\");\n    include_once("+strBasePath+".\"../const.php\");\n    if(file_exists("+strBasePath+".'config.php')) include_once("+strBasePath+".'config.php');\n\n";

            AString TableName = Parse::mCreateList.at(index).TableName;
            AString ClassName = Normalize(TableName);
            AString FolerName = ClassName;
            AString FileName  = ClassName+".php";
            AString GFileName = FolerName+"/"+ClassName+".php";
            mkdir(FolerName.c_str(),0755);
            Filed PrimaryKey  = Parse::mCreateList.at(index).PrimaryKey;
            auto   ThisFiled  = Parse::mCreateList.at(index);
            auto   FiledList  = Parse::mCreateList.at(index).FiledList;
            size_t FiledSize  = FiledList.size();
            ret+="    //TableName: "+TableName+"\n";
            ret+="    Class "+ClassName+"{\n";
            for(int indexY = 0;indexY<FiledSize;indexY++){
                ret+="        public $"+Parse::mCreateList.at(index).FiledList.at(indexY).FiledName+";\n";
            }
            ret+="\n";
            //construct
            ret+="        public function __construct(";
            for(int indexY = 0;indexY<FiledSize;indexY++){
                AString temp;

                //emit AUTO_INCREMENT record
                if(FiledList.at(indexY).OtherDesc.ToUpperN().Contain("AUTO_INCREMENT")) continue;

                if(indexY == FiledSize-1) 
                    temp = "){\n";
                else temp =",";
                ret+= "$"+Parse::mCreateList.at(index).FiledList.at(indexY).FiledName+temp;
            }
            for(int indexY = 0;indexY<FiledSize;indexY++){
                //emit AUTO_INCREMENT record
                if(FiledList.at(indexY).OtherDesc.ToUpperN().Contain("AUTO_INCREMENT")) continue;

                ret+= "            $this->"+Parse::mCreateList.at(index).FiledList.at(indexY).FiledName+" = $"+Parse::mCreateList.at(index).FiledList.at(indexY).FiledName+";\n";
            }
            ret+="        }\n\n";

            //Check
            ret+="        public function Check(){\n            ";
            for(int indexY =0;indexY<FiledSize;indexY++){
                AString TempFiledName = FiledList.at(indexY).FiledName;
                if(FiledList.at(indexY).FiledComment.ToUpperN().Contain(REPEATCOND) || FiledList.at(indexY).OtherDesc.ToUpperN().Contain("UNIQUE")){
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
                //emit AUTO_INCREMENT record
                if(FiledList.at(indexY).OtherDesc.ToUpperN().Contain("AUTO_INCREMENT")) continue;

                if(indexY == FiledSize-1) {
                    temp = ")VALUES(";
                    for(int indexZ = 0;indexZ <FiledSize;indexZ++){
                        //emit AUTO_INCREMENT record
                        if(FiledList.at(indexZ).OtherDesc.ToUpperN().Contain("AUTO_INCREMENT")) continue;

                        temp+=(indexZ==FiledSize-1?"?)\";\r":"?,");
                    }
                }
                else temp =",\n                ";
                ret+=FiledList.at(indexY).FiledName+temp;
            }
            ret+="\n\n            return MysqlHelper::SafeQuery($insertSql,\"";
            for(int indexY = 0;indexY<FiledSize;indexY++){
                //emit AUTO_INCREMENT record
                if(FiledList.at(indexY).OtherDesc.ToUpperN().Contain("AUTO_INCREMENT")) continue;

                AString aFiledType = FiledList.at(indexY).FiledType.ToLowwer();
                ret+=ToType(aFiledType);
                if(indexY == FiledList.size()-1){
                    ret+="\",\n                ";
                }
            }

            for(int indexY = 0;indexY<FiledSize;indexY++){
                //emit AUTO_INCREMENT record
                if(FiledList.at(indexY).OtherDesc.ToUpperN().Contain("AUTO_INCREMENT")) continue;

                if(indexY == FiledList.size()-1){
                    ret+="$this->"+FiledList.at(indexY).FiledName+");\n";
                }else
                {
                    ret+="$this->"+FiledList.at(indexY).FiledName+",\n                ";     
                }
            }
            ret+="        }\n";
            //Add Action
            AString AddCode = "<?php \n    include_once(\'"+FileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return (new "+ClassName+"(\n            ";
            for(int indexY=0;indexY<FiledSize;indexY++){
                AString temp = FiledList.at(indexY).FiledComment;

                //emit AUTO_INCREMENT record
                if(FiledList.at(indexY).OtherDesc.ToUpperN().Contain("AUTO_INCREMENT")) continue;

                if(indexY == FiledSize - 1){
                    if(!temp.Contain(FILEREFCOND))
                         AddCode  += "GetRealValueSafe(\""+FiledList.at(indexY).FiledName+"\")\n            ))->Add();\n    })());";
                    else AddCode  += "FileOperator::getFile(\""+FiledList.at(indexY).FiledName+"\")\n            ))->Add();\n    })());";
                }
                else{
                    if(!temp.Contain(FILEREFCOND))
                         AddCode  += "GetRealValueSafe(\""+FiledList.at(indexY).FiledName+"\"),\n            ";
                    else AddCode  += "FileOperator::getFile(\""+FiledList.at(indexY).FiledName+"\"),\n            ";
                }      
            }
            auto AddFile = fopen(FolerName+"/Add.php","wr");
            fwrite(AddCode.c_str(),sizeof(char),AddCode._length(),AddFile);
            fclose(AddFile);

            //TotalCount
            ret+="\n        public static function GetTotalCount(){\n            return MysqlHelper::SafeQueryResult(\"SELECT count(*) AS 'count' FROM "
            + TableName + " WHERE 1 = ?\",'i',1);\n        }\n";
            //Count Action
            AString CountCode = "<?php \n    include_once(\'"+FileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return "+ClassName+"::GetTotalCount();\n    })());";
            auto CountFile = fopen(FolerName+"/Count.php","wr");
            fwrite(CountCode.c_str(),sizeof(char),CountCode._length(),CountFile);
            SAFE_CLOSE(CountFile);

            //GetList temp
            ret+="\n        public static function GetList($page,$num=10000){\n            $start = $num*($page-1);\n            MysqlHelper::$UseUTF8 = true;\n            ";
                    ret+="return MysqlHelper::SafeQueryResult(\"SELECT * FROM "+TableName+" LIMIT ?,?\",\"dd\",$start,$num);\n        }\n\n";
            //GetList Action
            AString GetListCode = "<?php \n    include_once(\'"+FileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return "+ClassName+"::GetList(\n            GetRealValueSafe('page',1),\n            GetRealValueSafe('num',10000));\n    })());";
            auto GetListFile = fopen(FolerName+"/GetList.php","wr");
            fwrite(GetListCode.c_str(),sizeof(char),GetListCode._length(),GetListFile);
            SAFE_CLOSE(GetListFile);
            
            //Find 
            ret+="        //FIND FUNCTIONS START----------------------------------------------------\n";
            for(int indexY = 0;indexY<FiledSize ;indexY++){
                // SHOW_MESSAGE(FiledList.at(indexY).FiledComment.c_str(),1);
                // if(FiledList.at(indexY).FiledComment.Contain(FINDCONDITION)){
                for(int indexZ =0;indexZ<FiledSize ;indexZ++){
                    ret+="        public static function FindBy"+Normalize(FiledList.at(indexY).FiledName.ToUpperN())+"OrderBy"+Normalize(FiledList.at(indexZ).FiledName.ToUpperN())+"($value,$page=1,$num=10000){\n            $start = $num*($page-1);\n            MysqlHelper::$UseUTF8 = true;\n            ";
                    ret+="$CountRet = MysqlHelper::SafeQueryResult(\"SELECT count(*) AS 'count' FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ?\",'"+ToType(FiledList.at(indexY).FiledType)+"',$value);\n            ";
                    ret+="if($CountRet->resultcode<0) return $CountRet;";
                    ret+="$ret   = MysqlHelper::SafeQueryResult(\"SELECT * FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ? ORDER BY "+FiledList.at(indexZ).FiledName+" LIMIT ?,? \",\'"+ToType(FiledList.at(indexY).FiledType)+"dd\'"+",$value,$start,$num);\n            ";
                    ret+="$RetArray = array('total_count'=>$CountRet->value('count'),'list'=>$ret->data,'resultcode'=>$ret->resultcode);\n            return $RetArray;\n        }\n";
                }
                // }
            }
            //Find By Two Condition
            if(FiledList.size()>1 && twocond)
            for(int indexY = 0;indexY<FiledSize ;indexY++){
                // SHOW_MESSAGE(FiledList.at(indexY).FiledComment.c_str(),1);
                // if(FiledList.at(indexY).FiledComment.Contain(FINDCONDITION)){
                for(int indexZ =0;indexZ<FiledSize ;indexZ++){
                    for(int indexA =0 ;indexA<FiledSize;indexA++){
                        ret+="        public static function FindBy"+Normalize(FiledList.at(indexY).FiledName.ToUpperN())+"And"+Normalize(FiledList.at(indexZ).FiledName.ToUpperN())+"OrderBy"+Normalize(FiledList.at(indexA).FiledName.ToUpperN())+"($former,$latter,$page=1,$num=10000){\n            $start = $num*($page-1);\n            MysqlHelper::$UseUTF8 = true;\n            ";
                        ret+="$CountRet = MysqlHelper::SafeQueryResult(\"SELECT count(*) AS 'count' FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ? AND "+FiledList.at(indexZ).FiledName+" = ? \",'"+ToType(FiledList.at(indexY).FiledType)+ToType(FiledList.at(indexZ).FiledType)+"',$former,$latter);\n            ";
                        ret+="if($CountRet->resultcode<0) return $CountRet;";
                        ret+="$ret   = MysqlHelper::SafeQueryResult(\"SELECT * FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ? AND "+FiledList.at(indexZ).FiledName+" = ? ORDER BY "+FiledList.at(indexA).FiledName+" LIMIT ?,? \",\'"+ToType(FiledList.at(indexY).FiledType)+ToType(FiledList.at(indexZ).FiledType)+"dd\'"+",$former,$latter,$start,$num);\n            ";
                        ret+="$RetArray = array('total_count'=>$CountRet->value('count'),'list'=>$ret->data,'resultcode'=>$ret->resultcode);\n            return $RetArray;\n        }\n";
                    }
                }
                // }
            }

            ret+="        //FIND FUNCTIONS END----------------------------------------------------\n\n        ";
            //Find Action 
            for(int indexY=0;indexY<FiledSize;indexY++){
                AString temp = Normalize(FiledList.at(indexY).FiledName);
                AString FindCode = "<?php \n    include_once(\'"+FileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return "+ClassName+"::FindBy"+temp+"(\n            GetRealValueSafe('value'),\n            GetRealValueSafe('page',1),\n            GetRealValueSafe('num',10000));\n    })());";
                auto FindFile = fopen((FolerName+"/FindBy"+temp+".php").c_str(),"wr");
                fwrite(FindCode.c_str(),sizeof(char),FindCode._length(),FindFile);
                SAFE_CLOSE(FindFile);
            }

            //Alter 
            ret+="//ALTER FUNCTIONS START----------------------------------------------------\n";
            // for(int indexY = 0;indexY<FiledSize ;indexY++){
            //         if(FiledList.at(indexY).FiledName.Equal(ThisFiled.PrimaryKey.FiledName)) continue;
            //         ret+="        public static function Alter"+Normalize(FiledList.at(indexY).FiledName.ToUpperN())+"By"+Normalize(ThisFiled.PrimaryKey.FiledName)+"($unique,$value){\n            MysqlHelper::$UseUTF8 = true;\n            ";
            //         ret+="$count = MysqlHelper::SafeQueryResult(\"SELECT count(*) AS 'count' FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ?\",$value);\n            ";
            //         ret+="$ret   = MysqlHelper::SafeQueryResult(\"UPDATE "+TableName+" SET "+FiledList.at(indexY).FiledName+" = ? WHERE "+ThisFiled.PrimaryKey.FiledName+" = ?\",\'"+ToType(FiledList.at(indexY).FiledType)+ToType(ThisFiled.PrimaryKey.FiledType)+"\'"+",$value,$unique);\n            ";
            //         ret+="array_push($ret,$count);\n            return $ret;\n        }\n";
            // }

            for(int indexY = 0;indexY<FiledSize ;indexY++){
                // if(FiledList.at(indexY).FiledName.Equal(ThisFiled.PrimaryKey.FiledName)) continue;

                // if(FiledList.at(indexY).FiledComment.Contain(ALTERCOND))
                for(int indexZ = 0;indexZ<FiledSize;indexZ++){
                    // if(FiledList.at(indexY).FiledName.Equal(FiledList.at(indexZ).FiledName)) continue;
                    // if(FiledList.at(indexZ).FiledName.Equal(ThisFiled.PrimaryKey.FiledName)) continue;
                    ret+="        public static function Alter"+Normalize(FiledList.at(indexY).FiledName.ToUpperN())+"By"+Normalize(FiledList.at(indexZ).FiledName)+"($unique,$value){\n            MysqlHelper::$UseUTF8 = true;\n            ";
                    ret+="$count = MysqlHelper::SafeQueryResult(\"SELECT count(*) AS 'count' FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ?\",'"+ToType(FiledList.at(indexY).FiledType)+"',$value);\n            ";
                    ret+="$ret   = MysqlHelper::SafeQuery(\"UPDATE "+TableName+" SET "+FiledList.at(indexY).FiledName+" = ? WHERE "+FiledList.at(indexZ).FiledName+" = ?\",\'"+ToType(FiledList.at(indexY).FiledType)+ToType(FiledList.at(indexZ).FiledType)+"\'"+",$value,$unique);\n            ";
                    ret+="array_push($ret,$count);\n            return $ret;\n        }\n";
                }
            }
            ret+="        //ALTER FUNCTIONS END----------------------------------------------------\n\n        ";


            //Delete
            ret+="//DELTE FUNCTIONS START----------------------------------------------------\n        ";
            for(int indexY=0;indexY<FiledSize;indexY++){
                ret+="public static function DeleteBy"+Normalize(FiledList.at(indexY).FiledName)+"($unique){\n            $ret = MysqlHelper::S_IsEmptySet(\"SELECT * FROM "+TableName+" WHERE "+ FiledList.at(indexY).FiledName + " = ?\",'"+ToType(FiledList.at(indexY).FiledName)+"',$unique);\n            if($ret->resultcode<0) return new ReturnObject(-2,\"delete a no exist record\");\n            ";
                ret+="//TODO: if have file link . delete link before delete record!\n            $ret = MysqlHelper::SafeQueryResult(\"SELECT * FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName + "= ? \",'"+ToType(FiledList.at(indexY).FiledName)+"',$unique);\n            if($ret->resultcode<0) return $ret;\n            ";
                for(int indexZ=0;indexZ<FiledSize;indexZ++){
                    //Delete File Ref
                    AString temp = FiledList.at(indexZ).FiledName;
                    if(FiledList.at(indexZ).FiledComment.Contain(FILEREFCOND)){
                        ret+="unlink($ret->value(\""+temp+"\"));\n            ";
                    }
                }
                ret+="\n            return MysqlHelper::SafeQuery(\"DELETE FROM "+TableName+" WHERE "+FiledList.at(indexY).FiledName+" = ?\",'"+ToType(FiledList.at(indexY).FiledName)+"',$unique);";
                ret+="\n        }\n\n        ";
            }
            ret+="//DELETE FUNCTIONS END----------------------------------------------------\n\n        ";

            //DeleteAction
            AString DeleteCode = "<?php \n    include_once(\'"+FileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return "+ClassName+"::DeleteBy"+Normalize(ThisFiled.PrimaryKey.FiledName)+"(\n            GetRealValueSafe('value')\n        );\n    })());\n        ";
            auto DeleteFile = fopen(FolerName+"/Delete.php","wr");
            fwrite(DeleteCode.c_str(),sizeof(char),DeleteCode._length(),DeleteFile);
            SAFE_CLOSE(DeleteFile);

            //DeleteArray
            ret+="//DELTE ALL FUNCTIONS START----------------------------------------------------\n";
            for(int indexY=0;indexY<FiledSize;indexY++){
                ret+="        public static function DeleteArrayBy"+Normalize(FiledList.at(indexY).FiledName)+"($IdArray){\n            foreach($IdArray as $item){\n              if(($ret="+ClassName+"::DeleteBy"+Normalize(FiledList.at(indexY).FiledName)+"($item))->resultcode<0) return new ReturnObject(-224,\"failed when delete {$item} failed reason: {$ret->data}\");\n            }\n            return new ReturnObject(0,\"Delete All Ok!\");"+"\n        }\n\n";
            }
            ret+="        //DELETE FUNCTIONS END----------------------------------------------------\n\n";
            AString DeleteArrayCode = "<?php \n    include_once(\'"+FileName+"');\n\n    echo json_encode((function(){\n        header(\"content-type:application/json\");\n        return "+ClassName+"::DeleteArrayBy"+Normalize(ThisFiled.PrimaryKey.FiledName)+"(\n            GetRealValueSafe('array','none')\n        );\n    })());\n";
            auto DeleteArrayFile = fopen(FolerName+"/DeleteArray.php","wr");
            fwrite(DeleteArrayCode.c_str(),sizeof(char),DeleteArrayCode._length(),DeleteArrayFile);
            SAFE_CLOSE(DeleteArrayFile);

            ret+="    }\n";
            auto file = fopen(GFileName,"wr");
            fwrite(ret.c_str(),sizeof(char),ret._length(),file);
            SAFE_CLOSE(file);
        }
    }
};

vector<AString> Parse::mList;
vector<CreateExpression> Parse::mCreateList;
bool Parse::twocond = false;

int main(int argc, char const *argv[])
{

    {
        // SHOW_MESSAGE(*++argv++,1);
        Parse::init(argc>1?*++argv:"xk_show.sql");
        // Parse::init("xk_show.sql");
        if(argc>2) {
            Parse::twocond = AString(*++argv).ToUpperN().Equal("COMPLEX")?true:false;
        }

        Parse::parse();
        Parse::Test();

        // SHOW_MESSAGE(Parse::mCreateList1.size(),1);        

        // for(int index=0;index<Parse::mCreateList.size();index++){
             
        //      for(int indexY=0;indexY<Parse::mCreateList.at(index).FiledList.size();indexY++){
        //          SHOW_MESSAGE(Parse::mCreateList.at(index).FiledList.at(indexY).FiledName.c_str(),1);
        //      }
        //  }
        
        // SHOW_MESSAGE(Parse::mList.size(),1);
        // for(int index=0;index<Parse::mList.size();index++){
        //     SHOW_MESSAGE(Parse::mList.at(index).c_str(),1);
        // }
    }

    return 0;
}

