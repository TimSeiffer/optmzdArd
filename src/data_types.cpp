#include <iostream>
char ref;
int8_t sref;
uint8_t uref;

int x;            // Default initialization: uinitialized (garbage value)
int x{};          // Value initialization: 0
int x = {};       // Value initialization (with equals): 0
int x = int();    // Value initialization (creates temporary, then copy): 0
int x = 10;       // Copy initialization: 10
int x = (10);     // Copy initialization (creates temporary, then copy): 10
int x = (0, 1);   // Copy initialization (comma operator evalutes to rightmost): 0
int x(10);        // Direct initialization: 10
int x{10};        // Direct list (uniform) initialization: 10
int x = {10};     // Copy list initialization: 10
auto x = 10;      // Type deduction: int, value 10
auto x{10};       // Type deduction: int, value 10 (since C++17, ealier was std::initializer_list<int>)
auto x = int{10}; // Type deduction: int, value 10 (creates temporary, then copy)
auto x = (1, 0);  // Type deduction: int, value 0 (comma operator)

char x{1};
signed char x{1};
unsigned char x{1};
const signed char x{1};
const unsigned char x{1};
static signed char x{1};
static unsigned char x{1};
constexpr signed char x{1};
constexpr unsigned char x{1};

char *ptr{&ref};
signed char *ptr{&sref};
unsigned char *ptr{&uref};
const signed char *ptr{&sref};
signed char *const ptr{&sref};
const unsigned char *ptr{&uref};
unsigned char *const ptr{&uref};
static signed char *ptr{&sref};
static unsigned char *ptr{&uref};
constexpr signed char *ptr{&sref};
constexpr const signed char *ptr{&sref};
constexpr const unsigned char *ptr{&uref};

char x;
short int x;
int x;
long x;        // or long int
long long x;   // or long long int
std::string x; // or char *x
float x;
double x;

int main()
{
        float myFloat{1.3};
        int intigerfiedFloat = (int)myFloat;
        return 0;
}

int s = int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{int{}}}}}}}}}}}}}}}}}}}}}}}}}}}};
