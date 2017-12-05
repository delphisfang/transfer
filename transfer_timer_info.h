#ifndef _TRANSFER_TIMER_INFO_H_
#define _TRANSFER_TIMER_INFO_H_

#include <time.h>
#include <iostream>
#include <string>

#include "txf_timer_info.h"

using namespace std;
using namespace tfc;
using namespace tfc::base;
using namespace tfc::cache;

#if 0
namespace transfer
{
	class CMCDProc;
	
    class SendMsgTimerInfo: public CTimerInfo
	{
	    public:
		enum STATE
	    {
	        STATE_INIT		= 0,
			STATE_GET_IP	= 1,
			STATE_SEND		= 2,
	        STATE_END       = 255,
	    };
			
	    SendMsgTimerInfo(CMCDProc* const proc
	                  , unsigned msg_seq
	                  , const timeval& ccd_time
	                  , string ccd_client_ip
	                  , uint64_t ret_flow
	                  , uint64_t max_time_gap)
	                  : CTimerInfo(proc, msg_seq, ccd_time, ccd_client_ip, ret_flow, max_time_gap)
	    {
	    	m_expire_del  = false;
			m_retry_times = 0;
	    }
	    ~SendMsgTimerInfo();

	    int do_next_step(string& req_data);
	    int on_req_cp_send_msg();
		int on_rsp_cp_send_msg(string req_data, int datalen);
		int on_req_state_svr_get_cp_addr();
		int on_rsp_state_svr_get_cp_addr(string req_data, int datalen);
		int resp_cp_first();
		virtual void on_expire();
	    virtual bool on_expire_delete(){ return m_expire_del; }

		// self
		bool m_expire_del;
		int  m_retry_times;

		// mid
		string m_session_ip;
		unsigned short m_session_port;
	};
}
#endif

#endif
