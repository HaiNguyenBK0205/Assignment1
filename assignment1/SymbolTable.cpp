#include "SymbolTable.h"

int countSpace(string line)
{
    int count = 0;
    for (char c : line)
    {
        if (c == ' ')
        {
            count++;
        }
    }
    return count;
}

bool compareString(string a, string b)
{
    if (a.size() != b.size())
    {
        return false;
    }

    for (int i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

bool validation(string line, int number)
{
    const string commands[7] = {"INSERT", "ASSIGN", "BEGIN", "END", "LOOKUP", "PRINT", "RPRINT"};

    string reg = "[a-zA-Z][a-zA-Z_0-9]*";

    string statements[3];

    int count = 0;
    for (char c : line)
    {
        if (c == ' ')
        {
            count++;
        }
        else
        {
            statements[count].push_back(c);
        }
    }

    if (number == 2)
    {
        if (compareString(statements[0], commands[0]))
        {
            if (regex_match(statements[1], regex(reg)) == 0)
            {
                return false;
            }

            if (compareString(statements[2], "number") && compareString(statements[2], "string"))
            {
                return false;
            }
        }
        else if (compareString(statements[0], commands[1]))
        {
            if (regex_match(statements[1], regex(reg)) == 0)
            {
                return false;
            }

            string regNumber = "[0-9]+";
            string regString = "'[a-zA-Z 0-9]*'";

            if (regex_match(statements[2], regex(regNumber)) == 0 && regex_match(statements[2], regex(regString)) == 0 && regex_match(statements[2], regex(reg)) == 0)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    if (number == 1)
    {
        if (compareString(statements[0], commands[4]))
        {
            if (regex_match(statements[1], regex(reg)) == 0)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    if (number == 0)
    {
        if (compareString(statements[0], commands[2]) && compareString(statements[0], commands[3]) && compareString(statements[0], commands[5]) && compareString(statements[0], commands[6]))
        {
            return false;
        }
    }
    return true;
}

void getStatement(string line, int number, string statements[3])
{
    if (validation(line, number) == 0)
    {
        throw InvalidInstruction(line);
    }

    int count = 0;
    for (char c : line)
    {
        if (c == ' ')
        {
            count++;
        }
        else
        {
            statements[count].push_back(c);
        }
    }

    if (number == 1)
    {
        statements[2] = "";
    }

    if (number == 0)
    {
        statements[2] = statements[1] = "";
    }
}

class Statement
{
public:
    string statement;
    string iden_name;
    string iden_type;
    int levelBlock;

    string value;

public:
    Statement()
    {
        this->statement = this->iden_name = this->iden_type = this->value = "";
        this->levelBlock = 0;
    }
    Statement(string s, string n, string t, int l = 0)
    {
        this->statement = s;
        this->iden_name = n;
        this->iden_type = t;
        this->levelBlock = l;
        this->value = "";
    }
    ~Statement(){};

    friend bool operator==(Statement a, Statement b)
    {
        if (a.iden_name == b.iden_name && a.levelBlock == b.levelBlock)
        {
            return true;
        }
        return false;
    }

    void operator=(const Statement &st)
    {
        this->statement = st.statement;
        this->iden_name = st.iden_name;
        this->iden_type = st.iden_type;
        this->levelBlock = st.levelBlock;
        this->value = st.value;
    }

    string getStatement()
    {
        string res = "";

        if (this->statement.size() != 0)
        {
            for (char c : this->statement)
            {
                res.push_back(c);
            }
        }

        if (this->iden_name.size() != 0)
        {
            res.push_back(' ');
            for (char c : this->iden_name)
            {
                res.push_back(c);
            }
        }

        if (this->iden_type.size() != 0)
        {
            res.push_back(' ');
            for (char c : this->iden_type)
            {
                res.push_back(c);
            }
        }

        return res;
    }
};

class DDL
{
public:
    class Node;

protected:
    Node *head;
    Node *tail;
    int count;
    int level;

public:
    DDL()
    {
        this->head = this->tail = NULL;
        this->count = this->level = 0;
    }
    ~DDL()
    {
        this->clear();
    }

public:
    class Node
    {
    public:
        Statement state;
        Node *prev;
        Node *next;
        friend class DDL;

    public:
        Node()
        {
            this->next = this->prev = NULL;
        }

        Node(const Statement st)
        {
            this->next = this->prev = NULL;
            this->state = st;
        }

        ~Node() {}
    };

public:
    int size()
    {
        return this->count;
    }

    bool empty()
    {
        return (this->count == 0) ? true : false;
    }

    void clear()
    {
        if (this->size() == 0)
            return;
        else
        {
            while (this->head != NULL)
            {
                Node *temporary = this->head;
                this->head = temporary->next;
                delete temporary;
            }
            this->head = this->tail = NULL;
            this->count = 0;
        }
    }

    int getLevel()
    {
        return this->level;
    }

    bool findState(Statement s, int lev)
    {
        Node *current = this->tail;
        while (true)
        {
            if (current->state.levelBlock < lev)
            {
                break;
            }

            if (compareString(current->state.statement, "INSERT") && compareString(current->state.iden_name, s.iden_name) && current->state.levelBlock == lev)
            {
                return true;
            }

            if (current->prev == NULL)
            {
                break;
            }

            current = current->prev;
        }

        return false;
    }

    void add(Statement e)
    {
        Node *temporary = new Node(e);
        e.levelBlock = this->level;

        if (this->count == 0)
        {
            if (compareString(e.statement, "INSERT"))
            {
                this->head = this->tail = temporary;
                temporary->state.levelBlock = this->level;
                cout << "success" << endl;
            }

            if (compareString(e.statement, "ASSIGN"))
            {
                string exc = e.getStatement();
                throw Undeclared(exc);
            }

            if (compareString(e.statement, "BEGIN"))
            {
                this->level++;
                this->head = this->tail = temporary;
                temporary->state.levelBlock = this->level;
            }

            if (compareString(e.statement, "END"))
            {
                throw UnknownBlock();
            }

            if (compareString(e.statement, "LOOKUP"))
            {
                string exc = e.getStatement();
                throw Undeclared(exc);
            }

            if (compareString(e.statement, "PRINT"))
            {
                this->head = this->tail = temporary;
                temporary->state.levelBlock = this->level;
            }

            if (compareString(e.statement, "RPRINT"))
            {
                this->head = this->tail = temporary;
                temporary->state.levelBlock = this->level;
            }
        }
        else
        {

            if (compareString(e.statement, "INSERT"))
            {
                Node *current = this->tail;
                while (true)
                {
                    if (current->state.levelBlock < this->level)
                    {
                        break;
                    }

                    if (current->state == e)
                    {
                        string exc = e.getStatement();
                        throw Redeclared(exc);
                    }

                    if (current->prev == NULL)
                    {
                        break;
                    }

                    current = current->prev;
                }
                this->tail->next = temporary;
                temporary->prev = this->tail;
                this->tail = temporary;
                temporary->state.levelBlock = this->level;
                cout << "success" << endl;
            }

            if (compareString(e.statement, "ASSIGN"))
            {
                string regNumber = "[0-9]*";
                string regString = "'[a-zA-Z 0-9]*'";
                string regVariable = "[a-zA-Z][a-zA-Z_0-9]*";

                if (regex_match(e.iden_type, regex(regNumber)) || regex_match(e.iden_type, regex(regString)))
                {
                    Node *current = this->tail;
                    bool success = false;
                    while (true)
                    {
                        if (current->state.levelBlock < this->level)
                        {
                            break;
                        }

                        if (current->state == e)
                        {
                            if (compareString(current->state.iden_type, "number") && regex_match(e.iden_type, regex(regNumber)))
                            {
                                success = true;
                            }
                            else if (compareString(current->state.iden_type, "string") && regex_match(e.iden_type, regex(regString)))
                            {
                                success = true;
                            }
                            else
                            {
                                string exc = e.getStatement();
                                throw TypeMismatch(exc);
                            }
                        }

                        if (current->prev == NULL)
                        {
                            break;
                        }

                        current = current->prev;
                    }

                    if (success)
                    {
                        this->tail->next = temporary;
                        temporary->prev = this->tail;
                        this->tail = temporary;
                        temporary->state.value = e.iden_type;
                        temporary->state.levelBlock = this->level;
                        cout << "success" << endl;
                    }
                    else
                    {
                        string exc = e.getStatement();
                        throw Undeclared(exc);
                    }
                }
                else
                {
                    Node *current = this->tail;
                    bool success = false;

                    Statement insertStateA;
                    Statement insertStateB;
                    Statement assignStateB;

                    while (true)
                    {
                        if (current->state.levelBlock < this->level)
                        {
                            break;
                        }

                        if (compareString(current->state.statement, "INSERT") && compareString(current->state.iden_name, e.iden_type) && current->state.levelBlock == this->level)
                        {
                            insertStateB = current->state;
                        }

                        if (compareString(current->state.statement, "INSERT") && compareString(current->state.iden_name, e.iden_name) && current->state.levelBlock == this->level)
                        {
                            insertStateA = current->state;
                        }

                        if (compareString(current->state.statement, "ASSIGN") && compareString(current->state.iden_name, e.iden_type) && current->state.levelBlock == this->level)
                        {
                            assignStateB = current->state;
                        }

                        if (current->prev == NULL)
                        {
                            break;
                        }

                        current = current->prev;
                    }

                    if (compareString(insertStateA.iden_name, "") || compareString(insertStateB.iden_name, "") || compareString(assignStateB.iden_name, ""))
                    {
                        string exc = e.getStatement();
                        throw Undeclared(exc);
                    }
                    else
                    {
                        if (!compareString(insertStateA.iden_type, insertStateB.iden_type))
                        {
                            string exc = e.getStatement();
                            throw TypeMismatch(exc);
                        }
                        else
                        {
                            this->tail->next = temporary;
                            temporary->prev = this->tail;
                            this->tail = temporary;
                            temporary->state.value = e.iden_type;
                            temporary->state.levelBlock = this->level;
                            cout << "success" << endl;
                        }
                    }
                }
            }

            if (compareString(e.statement, "BEGIN"))
            {
                this->level++;
                this->tail->next = temporary;
                temporary->prev = this->tail;
                this->tail = temporary;
                temporary->state.levelBlock = this->level;
            }

            if (compareString(e.statement, "END"))
            {
                this->level--;
                if (this->level < 0)
                {
                    throw UnknownBlock();
                }

                this->tail->next = temporary;
                temporary->prev = this->tail;
                this->tail = temporary;
                temporary->state.levelBlock = this->level;
            }

            if (compareString(e.statement, "LOOKUP"))
            {
                int lev = this->level;

                while (lev >= 0)
                {
                    if (findState(e, lev))
                    {
                        break;
                    }
                    lev--;
                }

                if (lev >= 0)
                {
                    cout << lev << endl;
                }
                else
                {
                    string exc = e.getStatement();
                    throw Undeclared(exc);
                }

                this->tail->next = temporary;
                temporary->prev = this->tail;
                this->tail = temporary;
                temporary->state.levelBlock = this->level;
            }

            if (compareString(e.statement, "PRINT"))
            {
                string result = "";
                string *ans = new string[this->count];
                int i = 0;

                Node *current = this->tail;
                int lev = this->level;

                while (lev >= 0)
                {
                    if (current->state.levelBlock < lev)
                    {
                        lev--;
                    }
                    if (compareString(current->state.statement, "INSERT") && current->state.levelBlock <= lev)
                    {
                        bool isContain = false;
                        for (int j = 0; j < i; j++)
                        {
                            if (compareString(current->state.iden_name, ans[j]))
                            {
                                isContain = true;
                                break;
                            }
                        }

                        if (!isContain)
                        {
                            string tem = "";
                            for (char c : current->state.iden_name)
                            {
                                tem.push_back(c);
                            }
                            tem = tem + "//" + to_string(current->state.levelBlock);

                            if (compareString(result, ""))
                            {
                                result = tem;
                            }
                            else
                            {
                                result = tem + " " + result;
                            }

                            ans[i] = current->state.iden_name;
                            i++;
                        }
                    }

                    if (current->prev == NULL)
                    {
                        break;
                    }

                    current = current->prev;
                }

                cout << result << endl;

                this->tail->next = temporary;
                temporary->prev = this->tail;
                this->tail = temporary;
                temporary->state.levelBlock = this->level;
            }

            if (compareString(e.statement, "RPRINT"))
            {
                string result = "";
                string *ans = new string[this->count];
                int i = 0;

                Node *current = this->tail;
                int lev = this->level;

                while (lev >= 0)
                {
                    if (current->state.levelBlock < lev)
                    {
                        lev--;
                    }
                    if (compareString(current->state.statement, "INSERT") && current->state.levelBlock <= lev)
                    {
                        bool isContain = false;
                        for (int j = 0; j < i; j++)
                        {
                            if (compareString(current->state.iden_name, ans[j]))
                            {
                                isContain = true;
                                break;
                            }
                        }

                        if (!isContain)
                        {
                            string tem = "";
                            for (char c : current->state.iden_name)
                            {
                                tem.push_back(c);
                            }
                            tem = tem + "//" + to_string(current->state.levelBlock);

                            if (compareString(result, ""))
                            {
                                result = tem;
                            }
                            else
                            {
                                result = result + " " + tem;
                            }

                            ans[i] = current->state.iden_name;
                            i++;
                        }
                    }

                    if (current->prev == NULL)
                    {
                        break;
                    }

                    current = current->prev;
                }

                cout << result << endl;

                this->tail->next = temporary;
                temporary->prev = this->tail;
                this->tail = temporary;
                temporary->state.levelBlock = this->level;
            }
        }
        this->count++;
    }
};

void SymbolTable::run(string filename)
{
    fstream input;
    input.open(filename);
    DDL Table;

    while (!input.eof())
    {
        string line;
        getline(input, line);
        int nSpace = countSpace(line);

        string statements[3];

        if (nSpace > 2 || compareString(line, ""))
        {
            throw InvalidInstruction(line);
        }

        getStatement(line, nSpace, statements);

        Statement item(statements[0], statements[1], statements[2]);
        Table.add(item);
    }
    if (Table.getLevel() > 0)
    {
        throw UnclosedBlock(Table.getLevel());
    }
}