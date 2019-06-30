#include <iostream>
template<typename T>
class Fluid{
private:
public:
    T* prev, curr, next;
    
    T* mem[3];
    
    void sync(){*prev = *curr; *curr = *next;}
    
    T  Prev(){return *prev;}//only used in valves

    T  Curr(){return *curr;}//only used in valves
    
    T& read(){return *next;}
    
    T* write(T& t){
        *(next) = t;   
    };
    
    T& write(){
        std::cout <<"Current write count: " << writecount++ << std::endl;   
        return *next;
    }
    
    int writecount;
    
    Fluid(){
        writecount = 0;
        prev = mem[0] = new T;
        curr = mem[1] = new T;
        next = mem[2] = new T;
    }
    
    Fluid(T* t){
        writecount = 0;
        prev = mem[0] = new T; 
        curr = mem[1] = new T;
        next = mem[2] = new T;

        *(mem[2]) = *(mem[1]) = *(mem[0]) = *t;
    }
};

class Valve{

    virtual bool Pred ()  = 0;
};

class Task{
    Task(std::function, Args...);
    void run()
};

class Pred{
    Pred(bool(func*)(void));

    void run() {


    } = 0;
};
