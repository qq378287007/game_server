#pragma once

class Sunnet
{
private:
    Sunnet() {}

public:
    static Sunnet *inst();
    void Start();
};
