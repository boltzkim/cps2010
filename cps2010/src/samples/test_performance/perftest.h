#ifndef __PERFTEST_CPP_H__
#define __PERFTEST_CPP_H__

#define NeoDDS

#if 1 // byungung, 20140312
#include <WINSOCK2.H>
/*
- Problem
 warning : 'AF_IPX' ��ũ�� ������
- Reason
 windows.h�� winsock.h�� �浹
 ������ windows.h�� winsock.h (�������� winsock) �� ���ԵǾ�,
������ �Ǿ��ٰ� �����⿡ windows.h�� ����� �������� winsock�� �����ؾ� �ȴ�.
- Solution
����, <window.h> ���� ���� ������ �ش�.
*/

#endif
#include <windows.h>


#include "MessagingIF.h"

class perftest_cpp
{
  public:
    perftest_cpp();
    ~perftest_cpp();
    
    int Run(int argc, char *argv[]);
    bool ParseConfig(int argc, char *argv[]);
    
  private:
    int Publisher();
    int Subscriber();

  public:
    static void MilliSleep(unsigned int millisec) {
        Sleep(millisec);
    }

    static void ThreadYield() {
        Sleep(0);
    }

  private:
    int  _DataLen;
    int  _BatchSize;
    int  _maxBinDataSize;
    int  _SamplesPerBatch;
    int  _NumIter;
    bool _IsPub;
    bool _IsScan;
    bool  _UseReadThread;
    int  _SpinLoopCount;
    int  _SleepMillisec;
    int  _LatencyCount;
    int  _NumSubscribers;
    int  _NumPublishers;
    int _InstanceCount;
    IMessaging *_MessagingImpl;
    const char *_ConfigFile;
    char **_MessagingArgv;
    int _MessagingArgc;
    bool _LatencyTest;
    bool _IsReliable;

  private:
    static char * StringDup(const char * str);

  public:
    static int  _SubID;
    static int  _PubID;
    static bool _PrintIntervals;
    static bool _IsDebug;
    static const char *_LatencyTopicName;
    static const char *_ThroughputTopicName;
    static const char *_AnnouncementTopicName;

    static LARGE_INTEGER _ClockFrequency;
    
    // Number of bytes sent in messages besides user data
    static const int OVERHEAD_BYTES = 28;
    
    // When running a scan, this determines the number of
    // latency pings that will be sent before increasing the 
    // data size
    static const int NUM_LATENCY_PINGS_PER_DATA_SIZE = 1000;
    
    // Flag used to indicate message is used for initialization only
    static const int INITIALIZE_SIZE = 1234;
    // Flag used to indicate end of test
    static const int FINISHED_SIZE = 1235;
    // Flag used to data packet length is changing
    static const int LENGTH_CHANGED_SIZE = 1236;

   public:
    static unsigned long long GetTimeUsec();
};

#endif // __PERFTEST_CPP_H__
