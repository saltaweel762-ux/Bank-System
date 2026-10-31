#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <limits>
#include <stdexcept>

using namespace std;

// التحقق من صحة البيانات 
class Validation
{
public:
    static bool validName(const string& name)
    {
        if (name.length() < 3 || name.length() > 20)
            return false;

        for (size_t i = 0; i < name.length(); i++)
        {
            unsigned char c = static_cast<unsigned char>(name[i]);
            if (!isalpha(c))
                return false;
        }
        return true;
    }

    static bool validPass(const string& pass)
    {
        if (pass.length() < 8 || pass.length() > 20)
            return false;

        for (size_t i = 0; i < pass.length(); i++)
        {
            if (isspace(static_cast<unsigned char>(pass[i])) || pass[i] == '&')
                return false;
        }
        return true;
    }

    static bool validBalance(double balance)
    {
        return balance >= 1500.0;
    }

    static bool validSalary(double salary)
    {
        return salary >= 5000.0;
    }
};

// التصريحات المسبقة للكلاسات
class Client;
class Employee;
class Admin;

// الشخص 
class Person
{
protected:
    int id;
    string name;
    string pass;

public:
    Person(int id, string name, string pass)
    {
        this->id = id;
        this->name = "";
        this->pass = "";

        setName(name);
        setPass(pass);
    }

    virtual ~Person() {}

    //دوال الحصول على البيانات
    int getId() const { return id; }
    string getName() const { return name; }
    string getPass() const { return pass; }

    // Setters | دوال تعديل البيانات
    void setId(int id) { this->id = id; }

    void setName(const string& name)
    {
        if (Validation::validName(name))
            this->name = name;
        else
            cout << "Invalid name. Name must contain only letters and be 3-20 characters.\n";
    }

    void setPass(const string& pass)
    {
        if (Validation::validPass(pass))
            this->pass = pass;
        else
            cout << "Invalid password. Password must be 8-20 characters without spaces or '&'.\n";
    }

    virtual void display() const
    {
        cout << "ID : " << id << endl;
        cout << "Name : " << name << endl;
    }
};

// العميل 
class Client : public Person
{
private:
    double balance;

public:
    Client(int id, string name, string pass, double balance)
        : Person(id, name, pass)
    {
        if (Validation::validBalance(balance))
            this->balance = balance;
        else
        {
            this->balance = 1500.0;
            cout << "Invalid balance. Balance set to minimum 1500.\n";
        }
    }

    double getBalance() const { return balance; }

    void setBalance(double balance)
    {
        if (Validation::validBalance(balance))
            this->balance = balance;
        else
            cout << "Invalid balance. Minimum balance is 1500.\n";
    }

    void deposit(double amount)
    {
        if (amount <= 0)
        {
            cout << "Invalid amount. Deposit amount must be positive.\n";
            return;
        }

        balance += amount;
        cout << "Deposit successful.\n";
    }

    void withdraw(double amount)
    {
        if (amount <= 0)
        {
            cout << "Invalid amount. Withdrawal amount must be positive.\n";
            return;
        }

        if (balance - amount < 1500.0)
        {
            cout << "Withdrawal failed. Minimum balance is 1500.\n";
            return;
        }

        balance -= amount;
        cout << "Withdrawal successful.\n";
    }

    void transferTo(double amount, Client& recipient)
    {
        if (amount <= 0)
        {
            cout << "Invalid amount. Transfer amount must be positive.\n";
            return;
        }

        if (this->id == recipient.id)
        {
            cout << "Transfer failed. You cannot transfer to yourself.\n";
            return;
        }

        if (balance - amount < 1500.0)
        {
            cout << "Transfer failed. Minimum balance is 1500.\n";
            return;
        }

        balance -= amount;
        recipient.balance += amount;
        cout << "Transfer successful.\n";
    }

    void checkBalance() const
    {
        cout << "Current balance: " << balance << endl;
    }

    void display() const override
    {
        Person::display();
        cout << "Balance : " << balance << endl;
    }
};

// الموظف 
class Employee : public Person
{
protected:
    double salary;

public:
    Employee(int id, string name, string pass, double salary)
        : Person(id, name, pass)
    {
        if (Validation::validSalary(salary))
            this->salary = salary;
        else
        {
            this->salary = 5000.0;
            cout << "Invalid salary. Salary set to minimum 5000.\n";
        }
    }

    double getSalary() const { return salary; }

    void setSalary(double salary)
    {
        if (Validation::validSalary(salary))
            this->salary = salary;
        else
            cout << "Invalid salary. Minimum salary is 5000.\n";
    }

    void display() const override
    {
        Person::display();
        cout << "Salary : " << salary << endl;
    }

    void addClient(Client& client);
    Client* searchClient(int id);
    void listClient() const;
    void editClient(int id, string name, string password, double balance);
};

//  المدير 
class Admin : public Employee
{
public:
    Admin(int id, const string& name, const string password, double salary)
        : Employee(id, name, password, salary) {
    }

    void addEmployee(Employee& employee);
    Employee* searchEmployee(int id);
    void editEmployee(int id, string name, string password, double salary);
    void listEmployee() const;
};

//الفكتير حبيبنا اللي هنخزن فيهم 
vector<Client> allClients;
vector<Employee> allEmployees;
vector<Admin> allAdmins;

// تحليل البيانات
class Parser
{
public:
    static vector<string> split(const string& line)
    {
        vector<string> data;
        string word;

        for (size_t i = 0; i < line.length(); i++)
        {
            if (line[i] == '&')
            {
                data.push_back(word);
                word.clear();
            }
            else
                word += line[i];
        }

        data.push_back(word);
        return data;
    }

    static bool validRecord(const vector<string>& data)
    {
        return data.size() == 4 &&
            !data[0].empty() &&
            !data[1].empty() &&
            !data[2].empty() &&
            !data[3].empty();
    }

    static Client parseToClient(const string& line)
    {
        vector<string> data = split(line);
        if (!validRecord(data))
            throw runtime_error("Invalid client record");

        size_t used = 0;
        int id = stoi(data[0], &used);
        if (used != data[0].length())
            throw runtime_error("Invalid client ID");

        used = 0;
        double balance = stod(data[3], &used);
        if (used != data[3].length())
            throw runtime_error("Invalid client balance");

        return Client(id, data[1], data[2], balance);
    }

    static Employee parseToEmployee(const string& line)
    {
        vector<string> data = split(line);
        if (!validRecord(data))
            throw runtime_error("Invalid employee record");

        size_t used = 0;
        int id = stoi(data[0], &used);
        if (used != data[0].length())
            throw runtime_error("Invalid employee ID");

        used = 0;
        double salary = stod(data[3], &used);
        if (used != data[3].length())
            throw runtime_error("Invalid employee salary");

        return Employee(id, data[1], data[2], salary);
    }

    static Admin parseToAdmin(const string& line)
    {
        vector<string> data = split(line);
        if (!validRecord(data))
            throw runtime_error("Invalid admin record");

        size_t used = 0;
        int id = stoi(data[0], &used);
        if (used != data[0].length())
            throw runtime_error("Invalid admin ID");

        used = 0;
        double salary = stod(data[3], &used);
        if (used != data[3].length())
            throw runtime_error("Invalid admin salary");

        return Admin(id, data[1], data[2], salary);
    }
};

// المساعد المسؤول عن الملفات
class FilesHelper
{
public:
    static void saveLast(const string& fileName, int id)
    {
        ofstream file(fileName.c_str());
        if (!file)
        {
            cout << "Could not open " << fileName << " for writing.\n";
            return;
        }
        file << id;
    }

    static int getLast(const string& fileName)
    {
        ifstream file(fileName.c_str());
        int id = 0;
        if (file >> id)
            return id;
        return 0;
    }

    static void saveClient(const Client& c)
    {
        ofstream file("Clients.txt", ios::app);
        if (!file)
        {
            cout << "Could not open Clients.txt for writing.\n";
            return;
        }

        file << c.getId() << "&" << c.getName() << "&"
            << c.getPass() << "&" << c.getBalance() << endl;
        file.close();
        saveLast("ClientLastId.txt", c.getId());
    }

    static void saveEmployee(const string& fileName,
        const string& lastIdFile,
        const Employee& e)
    {
        ofstream file(fileName.c_str(), ios::app);
        if (!file)
        {
            cout << "Could not open " << fileName << " for writing.\n";
            return;
        }

        file << e.getId() << "&" << e.getName() << "&"
            << e.getPass() << "&" << e.getSalary() << endl;
        file.close();
        saveLast(lastIdFile, e.getId());
    }

    static void saveAdmin(const Admin& a)
    {
        saveEmployee("Admins.txt", "AdminLastId.txt", a);
    }

    // تحميل بيانات العملاء من الملف 
    static void getClients()
    {
        allClients.clear();
        ifstream file("Clients.txt");
        string line;

        while (getline(file, line))
        {
            if (line.empty())
                continue;

            try
            {
                allClients.push_back(Parser::parseToClient(line));
            }
            catch (const exception&)
            {
                cout << "Skipping invalid record in Clients.txt.\n";
            }
        }
    }

    // تحميل بيانات الموظفين من الملف 
    static void getEmployees()
    {
        allEmployees.clear();
        ifstream file("Employees.txt");
        string line;

        while (getline(file, line))
        {
            if (line.empty())
                continue;

            try
            {
                allEmployees.push_back(Parser::parseToEmployee(line));
            }
            catch (const exception&)
            {
                cout << "Skipping invalid record in Employees.txt.\n";
            }
        }
    }

    // تحميل بيانات المديرين من الملف 
    static void getAdmins()
    {
        allAdmins.clear();
        ifstream file("Admins.txt");
        string line;

        while (getline(file, line))
        {
            if (line.empty())
                continue;

            try
            {
                allAdmins.push_back(Parser::parseToAdmin(line));
            }
            catch (const exception&)
            {
                cout << "Skipping invalid record in Admins.txt.\n";
            }
        }
    }

    // مسح محتويات الملف وتصفير آخر رقم تعريفي 
    static void clearFile(const string& fileName, const string& lastIdFile)
    {
        ofstream file(fileName.c_str());
        file.close();
        saveLast(lastIdFile, 0);
    }

    // حفظ جميع العملاء 
    static void saveAllClients()
    {
        ofstream file("Clients.txt");
        if (!file)
        {
            cout << "Could not open Clients.txt for writing.\n";
            return;
        }

        for (size_t i = 0; i < allClients.size(); i++)
        {
            file << allClients[i].getId() << "&"
                << allClients[i].getName() << "&"
                << allClients[i].getPass() << "&"
                << allClients[i].getBalance() << endl;
        }
        file.close();

        int maxId = 0;
        for (size_t i = 0; i < allClients.size(); i++)
            if (allClients[i].getId() > maxId)
                maxId = allClients[i].getId();
        saveLast("ClientLastId.txt", maxId);
    }

    // حفظ جميع الموظفين 
    static void saveAllEmployees()
    {
        ofstream file("Employees.txt");
        if (!file)
        {
            cout << "Could not open Employees.txt for writing.\n";
            return;
        }

        for (size_t i = 0; i < allEmployees.size(); i++)
        {
            file << allEmployees[i].getId() << "&"
                << allEmployees[i].getName() << "&"
                << allEmployees[i].getPass() << "&"
                << allEmployees[i].getSalary() << endl;
        }
        file.close();

        int maxId = 0;
        for (size_t i = 0; i < allEmployees.size(); i++)
            if (allEmployees[i].getId() > maxId)
                maxId = allEmployees[i].getId();
        saveLast("EmployeeLastId.txt", maxId);
    }

    // حفظ جميع المديرين
    static void saveAllAdmins()
    {
        ofstream file("Admins.txt");
        if (!file)
        {
            cout << "Could not open Admins.txt for writing.\n";
            return;
        }

        for (size_t i = 0; i < allAdmins.size(); i++)
        {
            file << allAdmins[i].getId() << "&"
                << allAdmins[i].getName() << "&"
                << allAdmins[i].getPass() << "&"
                << allAdmins[i].getSalary() << endl;
        }
        file.close();

        int maxId = 0;
        for (size_t i = 0; i < allAdmins.size(); i++)
            if (allAdmins[i].getId() > maxId)
                maxId = allAdmins[i].getId();
        saveLast("AdminLastId.txt", maxId);
    }
};

//الكلاس الداتا 
class DataSourceInterface
{
public:
    virtual void addClient(Client obj) = 0;
    virtual void addEmployee(Employee obj) = 0;
    virtual void addAdmin(Admin obj) = 0;

    virtual void getAllClients() = 0;
    virtual void getAllEmployees() = 0;
    virtual void getAllAdmins() = 0;

    virtual void removeAllClients() = 0;
    virtual void removeAllEmployees() = 0;
    virtual void removeAllAdmins() = 0;

    virtual ~DataSourceInterface() {}
};

// اداره الملفات  
class FileManager : public DataSourceInterface
{
public:
    void addClient(Client obj) override
    {
        FilesHelper::saveClient(obj);
        bool found = false;
        for (size_t i = 0; i < allClients.size(); i++)
        {
            if (allClients[i].getId() == obj.getId())
            {
                allClients[i] = obj;
                found = true;
                break;
            }
        }
        if (!found)
            allClients.push_back(obj);
    }

    void addEmployee(Employee obj) override
    {
        FilesHelper::saveEmployee("Employees.txt", "EmployeeLastId.txt", obj);
        bool found = false;
        for (size_t i = 0; i < allEmployees.size(); i++)
        {
            if (allEmployees[i].getId() == obj.getId())
            {
                allEmployees[i] = obj;
                found = true;
                break;
            }
        }
        if (!found)
            allEmployees.push_back(obj);
    }

    void addAdmin(Admin obj) override
    {
        FilesHelper::saveAdmin(obj);
        bool found = false;
        for (size_t i = 0; i < allAdmins.size(); i++)
        {
            if (allAdmins[i].getId() == obj.getId())
            {
                allAdmins[i] = obj;
                found = true;
                break;
            }
        }
        if (!found)
            allAdmins.push_back(obj);
    }

    // تحميل جميع البيانات 
    void getAllClients() override { FilesHelper::getClients(); }
    void getAllEmployees() override { FilesHelper::getEmployees(); }
    void getAllAdmins() override { FilesHelper::getAdmins(); }

    // حذف جميع العملاء 
    void removeAllClients() override
    {
        FilesHelper::clearFile("Clients.txt", "ClientLastId.txt");
        allClients.clear();
    }

    // حذف جميع الموظفين 
    void removeAllEmployees() override
    {
        FilesHelper::clearFile("Employees.txt", "EmployeeLastId.txt");
        allEmployees.clear();
    }

    // حذف جميع المديرين 
    void removeAllAdmins() override
    {
        FilesHelper::clearFile("Admins.txt", "AdminLastId.txt");
        allAdmins.clear();
    }
};

// الموظف
void Employee::addClient(Client& client)
{
    client.setId(FilesHelper::getLast("ClientLastId.txt") + 1);
    allClients.push_back(client);
    FilesHelper::saveClient(client);
}

// البحث عن عميل باستخدام الرقم التعريفي 
Client* Employee::searchClient(int id)
{
    for (size_t i = 0; i < allClients.size(); i++)
    {
        if (allClients[i].getId() == id)
            return &allClients[i];
    }
    return NULL;
}

// عرض جميع العملاء 
void Employee::listClient() const
{
    if (allClients.empty())
    {
        cout << "No clients found.\n";
        return;
    }

    for (size_t i = 0; i < allClients.size(); i++)
    {
        allClients[i].display();
        cout << "--------------------\n";
    }
}

// تعديل بيانات العميل 
void Employee::editClient(int id, string name, string password, double balance)
{
    Client* c = searchClient(id);
    if (c == NULL)
    {
        cout << "Client not found.\n";
        return;
    }

    c->setName(name);
    c->setPass(password);
    c->setBalance(balance);
    FilesHelper::saveAllClients();
}
//المدير
void Admin::addEmployee(Employee& employee)
{
    employee.setId(FilesHelper::getLast("EmployeeLastId.txt") + 1);
    allEmployees.push_back(employee);
    FilesHelper::saveEmployee("Employees.txt", "EmployeeLastId.txt", employee);
}

// البحث عن موظف باستخدام الرقم التعريفي 
Employee* Admin::searchEmployee(int id)
{
    for (size_t i = 0; i < allEmployees.size(); i++)
    {
        if (allEmployees[i].getId() == id)
            return &allEmployees[i];
    }
    return NULL;
}

// تعديل بيانات الموظف 
void Admin::editEmployee(int id, string name, string password, double salary)
{
    Employee* e = searchEmployee(id);
    if (e == NULL)
    {
        cout << "Employee not found.\n";
        return;
    }

    e->setName(name);
    e->setPass(password);
    e->setSalary(salary);
    FilesHelper::saveAllEmployees();
}

// عرض جميع الموظفين 
void Admin::listEmployee() const
{
    if (allEmployees.empty())
    {
        cout << "No employees found.\n";
        return;
    }

    for (size_t i = 0; i < allEmployees.size(); i++)
    {
        allEmployees[i].display();
        cout << "--------------------\n";
    }
}


int main()
{
    FileManager fm;

    // هنحمل البيانات من غير ما نمسحها 
    fm.getAllClients();
    fm.getAllEmployees();
    fm.getAllAdmins();

    // إنشاء مدير افتراضي فقط إذا لم يكن هناك أي مدير محفوظ
    if (allAdmins.empty())
    {
        int id = FilesHelper::getLast("AdminLastId.txt") + 1;
        Admin admin(id, "Admin", "Admin1234", 5000);
        fm.addAdmin(admin);
        cout << "Default admin created. ID: " << id
            << ", password: Admin1234\n";
    }

    // عرض بيانات النظام
    cout << "\n********** Bank System Data ********\n";

    // عرض جميع العملاء 
    cout << "\nAll Clients:\n";
    if (!allClients.empty())
    {
        for (size_t i = 0; i < allClients.size(); i++)
        {
            allClients[i].display();
            cout << "=========================\n";
        }
    }
    else
        cout << "No clients found.\n";

    // عرض جميع الموظفين 
    cout << "\nAll Employees:\n";
    if (!allEmployees.empty())
    {
        for (size_t i = 0; i < allEmployees.size(); i++)
        {
            allEmployees[i].display();
            cout << "===========================\n";
        }
    }
    else
        cout << "No employees found.\n";

    // عرض جميع المديرين 
    cout << "\nAll Admins:\n";
    for (size_t i = 0; i < allAdmins.size(); i++)
    {
        allAdmins[i].display();
        cout << "===============================\n";
    }

 
}