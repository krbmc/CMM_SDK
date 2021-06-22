#include "KETIHA.hpp"
#include "KETI_halmp.hpp"
using namespace KETIHA_NSP;
 KETIHA* GetKetihaIf()
{
    return KETI_halmp::Instance();
}