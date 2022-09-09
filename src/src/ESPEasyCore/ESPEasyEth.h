#ifndef ESPEASY_ETH_H
#define ESPEASY_ETH_H

#include "../../ESPEasy_common.h"

#if FEATURE_ETHERNET

#include "../DataStructs/MAC_address.h"

bool     ethUseStaticIP();
void     ethSetupStaticIPconfig();
void     ethSetDNS(const IPAddress& dns0, const IPAddress& dns1);
bool     ethCheckSettings();
bool     ethPrepare();
void     ethPrintSettings();
bool     ETHConnectRelaxed();
bool     ETHConnected();
void     ethPower(bool enable);
MAC_address ETHMacAddress();

#endif // if FEATURE_ETHERNET
#endif // ifndef ESPEASY_ETH_H
