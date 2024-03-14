#include "Msg.h"

BaseMsg::~BaseMsg()
{
}

BaseMsg::BaseMsg(unsigned _from, unsigned _to, const string &_buff)
    : from(_from), to(_to), buff(_buff)
{
}

