#ifndef __TXF_TIMER_INFO_H__
#define __TXF_TIMER_INFO_H__

#include "tfc_cache_proc.h"
#include "tfc_base_http.h"
#include "tfc_base_fast_timer.h"
#include "common_api.h"
#include "tfc_debug_log.h"

#include "transfer_error.h"
//#include "longconn_parse/longconn_parse.h"
#include "debug.h"
#include "transfer_mcd_proc.h"

using namespace tfc::base;

namespace transfer
{
        const unsigned DATA_BUF_SIZE = 1024 * 1024 * 32;

		class CMCDProc;

        class CTimerInfo : public tfc::base::CFastTimerInfo
        {   
        public:

            CTimerInfo()
            {
            }

            CTimerInfo(CMCDProc* const proc, unsigned msg_seq, const timeval& ccd_time, string ccd_client_ip
                                 , uint64_t ret_flow, uint64_t max_time_gap = 10000)
            {
                gettimeofday(&m_start_time, NULL);
                m_op_start      = m_start_time;
                m_end_time      = m_start_time;
                m_cur_step      = 0;
                m_errno         = 0;
                m_msg_seq       = msg_seq;                
                m_proc          = proc;
                m_max_time_gap  = max_time_gap;
                m_ccd_time      = ccd_time;
                m_ret_flow      = ret_flow;
                m_client_ip     = ccd_client_ip;
                m_errmsg.clear();

                m_cmd.clear();
                m_seq.clear();
                m_identity.clear();
                m_userID.clear();
                m_serviceID.clear();
                m_content.clear();
                m_changeServiceID.clear();
                m_lastServiceID.clear();
                m_priority.clear();
                m_ret_code.clear();
                m_ret_msg.clear();
                m_search_no.clear();

	            m_tags.clear();
                m_checkServices.clear();
                m_queuePriority = 0;
            }

            virtual ~CTimerInfo()
            {
                m_tags.clear();
                m_checkServices.clear();
                m_content.clear();
            }

            virtual int  do_next_step(string& req_data)=0;
            virtual int  init(string req_data, int datalen);
            virtual int  on_error();
            virtual int  on_stat();

            virtual void AddStatInfo(const char* itemName, timeval* begin, timeval* end, int retcode);
            virtual void FinishStat(const char* itemName);

            virtual void OpStart();
            virtual void OpEnd(const char* itemName, int retcode);

            virtual void on_expire();
            virtual bool on_expire_delete(){ return true; }           

			void on_error_parse_packet(string errmsg);

			int on_send_request(string cmd, string ip, unsigned short port, const Json::Value &data, bool with_code = false);

			int on_send_reply(const Json::Value &data);

            uint32_t GetMsgSeq();

            uint64_t GetTimeGap();

		public:
			int32_t         m_errno;
            string          m_errmsg;

        protected:

            CMCDProc*       m_proc;
            
            uint32_t        m_cur_step;            
            uint32_t        m_msg_seq; 
            struct timeval  m_start_time;
            struct timeval  m_end_time;
            struct timeval  m_op_start;
            struct timeval  m_ccd_time;
            uint64_t        m_max_time_gap;
            uint64_t        m_ret_flow;
            string          m_client_ip;

            string          m_cmd;
            string          m_seq;
            string          m_tag;
            timeval         m_cur_time;
	        string          m_appID;
            string          m_data;
            string          m_chatproxyIP;	//所连CP的IP地址
            unsigned        m_chatproxyPort;//所连CP的端口
            string          m_identity;//0-user,1-service
            string          m_userID;//用户ID
            string          m_serviceID;//坐席ID
            string          m_sessionID;
            Json::Value     m_content;
            string          m_changeServiceID;
            string          m_lastServiceID;
            set<string>     m_tags;
            set<string>     m_checkServices;
            string          m_priority;
            string          m_ret_code;
            string          m_ret_msg;
            string          m_search_no;
            string          m_channel;
            string          m_extends;
            string          m_serviceName;
            string          m_serviceAvatar;
            string          m_whereFrom;
            string          m_userInfo;
            unsigned        m_queuePriority;
            unsigned        m_userCount;
       };
}
#endif

