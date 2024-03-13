#include <iostream>
#include "Msg.h"
using namespace std;

void BaseMsg::run()
{
    cout << "From " << from << " To " << to << ", Buff: " << buff << endl;
}
