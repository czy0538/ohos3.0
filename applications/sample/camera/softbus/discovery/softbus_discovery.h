#ifndef SOFTBUS_DISCOVERY_H
#define SOFTBUS_DISCOVERY_H

void onBytesReceivedFunc(int sessionId, const void *data, unsigned int dataLen);
void onSessionClosedFunc(int sessionId);
int onSessionOpenedFunc(int sessionId);
void onMessageReceivedFunc(int sessionId, const void *data, unsigned int dataLen);
int StartSessionServer();
void TestDeviceFound(const DeviceInfo *device);
void TestDiscoverFailed(int subscribeId, DiscoveryFailReason failReason);
void TestDiscoverySuccess(int subscribeId);
void onJoinLNNResultFunc(ConnectionAddr *addr, const char *networkId, int32_t retCode);
void onStreamReceivedFunc(int sessionId, const StreamData *data, const StreamData *ext, const FrameInfo *param);
#endif  // SOFTBUS_DISCOVERY_H