
//附加属性节点
typedef struct additional_attribute_node
{
    //附加属性名称
    string attr_name;
    //附加属性值以字符串形式存储
    string attr;  
    additional_attribute_node* next_attr;
}additional_attribute;

//语法分析栈节点
class SYNTAXNODE
{
//通用属性   
public:
    int val;
    string name;
    uint64_t addr;
    string type;
    map <string,string> nextattr;

    //初始化属性节点
    int init(string token_name){
        name = token_name;
        val = 0;
        addr = -1;
        return 0;
    };
};