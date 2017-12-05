#ifndef _TRANSFER_MCD_PROC_H_
#define _TRANSFER_MCD_PROC_H_

#include "tfc_object.h"
#include "tfc_base_fast_timer.h"
#include "tfc_cache_proc.h"
#include "tfc_net_ipc_mq.h"
#include "tfc_net_ccd_define.h"
#include "tfc_net_dcc_define.h"
#include "tfc_base_http.h"
#include "tfc_debug_log.h"

#include "transfer_cfg_mng.h"
#include "Statistic.hpp"
#include "common_api.h"
#include "debug.h"
//#include "app_config.h"
#include "txf_timer_info.h"
#include "transfer_timer_info.h"

#include <time.h>

using namespace tfc::base;
using namespace tfc::cache;
using namespace tfc::http;

using namespace transfer;

#define CLENGTH_LEN 32


namespace transfer
{
        enum workMode
        {
            WORKMODE_READY = 0,
            WORKMODE_WORK = 1,
        };

        enum cmdMode
        {
			ADMIN_CONFIG = 0,
            CP_SEND_MSG  = 1,
        };

        class CTimerInfo;

        class CMCDProc : public tfc::cache::CacheProc
        {
        public:
            CMCDProc()
            {
                m_recv_buf = NULL;
                m_send_buf = NULL;
                m_last_stat_time = time(NULL);
				m_last_check	 = m_last_stat_time;
				m_adjust_times_seq = 0;
                m_workMode = WORKMODE_READY;
                m_serviceTimeoutOn = 0;
                //m_appCmdList.clear();
                m_errCmd.clear();
            }

            virtual ~CMCDProc()
            {
                if (m_recv_buf)
                {
                    delete []m_recv_buf;
                    m_recv_buf = NULL;
                }

                if (m_send_buf)
                {
                    delete []m_send_buf;
                    m_send_buf = NULL;
                }

            }

            virtual void run(const std::string& conf_file);
            int32_t Init(const std::string& conf_file);
            int32_t ReloadCfg();
            int32_t EnququeHttp2CCD(unsigned long long flow, char *data, unsigned data_len);
            int32_t Enqueue2CCD(unsigned long long flow, char *data, unsigned data_len);
            int32_t Enqueue2DCC(char *data, unsigned data_len, const string& ip, unsigned short port);
            int32_t EnququeHttp2DCC(char* data, unsigned data_len, const string& ip, unsigned short port);
            int32_t EnququeConfigHttp2DCC();
            int32_t EnququeErrHttp2DCC(char* data, unsigned data_len);
            int32_t PostErrLog(string &data, int type, string appID, unsigned level);
            void    DispatchCCD();
            void    DispatchInnerCCD();
            
            int32_t HandleResponseHttp(char* data,
                                 unsigned data_len,
                                 unsigned long long flow,
                                 uint32_t down_ip, unsigned down_port, timeval& dcc_time);

            int     Echo_ping (const char *out_buf, uint32_t out_buf_len, unsigned seq_no, unsigned long long flow);
            int     Update_config (const char *out_buf, uint32_t out_buf_len, unsigned seq_no, unsigned long long flow);
            int     Enqueue_2_inner_ccd (const string &msg, uint16_t serviceType, unsigned seq_no, unsigned long long flow);
            void    DispatchDCC();

            int32_t HttpGetBu(const char *uri_buf, string &bu_name, string &param_str);
            int32_t HttpParseCmd(char* data, unsigned data_len, string& outdata, unsigned& out_len);
            void    DispatchDCCHttp();

            int32_t HandleRequest(char* data,
                                unsigned data_len,
                                unsigned long long flow,
                                uint32_t client_ip,
                                timeval& ccd_time);

            
            int32_t HandleResponse(char* data, unsigned data_len, unsigned long long flow,
                                         uint32_t down_ip, unsigned down_port, timeval& dcc_time);
            

            void AddStat(int retcode, const char* entry, struct timeval* begin, struct timeval* end, int cnt = 1)
            {
              if (0 == retcode)
              {
                  m_stat.AddStat(const_cast<char*>(entry), retcode, begin, end, NULL, cnt);
              }
              else
              {
                  char buff[128];
                  snprintf(buff, sizeof(buff), "%s[%d]", entry, retcode);
                  buff[sizeof(buff) - 1] = '\0';
                  m_stat.AddStat(buff, retcode, begin, end, NULL, cnt);
              }
            }

            unsigned GetMsgSeq()
            {
                if (m_msg_seq == 0)
                {
                    m_msg_seq++;
                }
                return m_msg_seq++;
            }

            void AddErrCmdMsg(string appID, string cmd, string identity, string ID, struct timeval startTime, int cur_errno)
            {
                Json::Value errMsg; 
                errMsg["appID"] = appID;
                errMsg["cmd"] = cmd;
                errMsg["identity"] = identity;

                if(identity == "user")
                {
                    errMsg["userID"] = ID;
                }
                else
                {
                    errMsg["serviceID"] = ID;
                }

                errMsg["time"] = GetFormatTime(startTime);
                errMsg["errno"] = i2str(cur_errno);

                m_errCmd.push_back(errMsg.toStyledString());

                if(m_errCmd.size() > 10000)
                {
                    m_errCmd.erase(m_errCmd.begin());
                }
            }

            void GetErrCmdMsg(vector<string> &value)
            {
                value = m_errCmd;
            }

        private:
          tfc::base::CFastTimerQueue m_timer_queue;
          tfc::net::CFifoSyncMQ* m_mq_ccd_2_mcd;
          tfc::net::CFifoSyncMQ* m_mq_mcd_2_ccd;
          tfc::net::CFifoSyncMQ* m_mq_mcd_2_dcc;
          tfc::net::CFifoSyncMQ* m_mq_dcc_2_mcd;
          
          /*tfc::net::CFifoSyncMQ* m_mq_dcc_2_mcd_http;
          tfc::net::CFifoSyncMQ* m_mq_mcd_2_dcc_http;
			*/
          
          CStatistic  m_stat;
          time_t      m_last_stat_time;
          time_t      m_last_get_conf_time;
          char*       m_recv_buf;
          char*       m_send_buf;

          unsigned    m_msg_seq;
          char**      m_arg_vals;
	      int         m_arg_cnt;
	      char        m_r_clength[CLENGTH_LEN];
	      time_t m_last_print_conf;

		  time_t 	  m_last_check;
		  int		  m_adjust_times_seq;

          map<string, unsigned> m_cmdMap;
          //map<unsigned, list<unsigned> > m_appCmdList;

          string m_seq;
          vector<string>  m_errCmd;

        public:
          CTransferCfgMng m_cfg;
          CHttpTemplate  m_http_template;

          int         m_workMode;
          bool        m_serviceTimeoutOn;
          //CBusinessConfig m_busi_config;
          static const int KNetDataBufLen = (32 << 20);
            
        private:
			int32_t InitSo();
            int32_t InitBuffer();
            int32_t InitLog();
            int32_t InitStat();
            int32_t InitIpc();
            int32_t InitTemplate();
            int32_t InitCmdMap();
           void    CheckFlag(bool);            

        public:
          int32_t DoNextStep(CTimerInfo* info, char* data, unsigned data_len, int retcode);            
        };

        const unsigned char FLG_CTRL_RELOAD = 0x01;
        const unsigned char FLG_CTRL_STOP   = 0x10;

        class CCheckFlag
        {
        public:
            CCheckFlag(): _flag((unsigned char)0){}
            ~CCheckFlag(){}
            void set_flag(unsigned char flag){ _flag |= flag;}
            inline bool is_flag_set( unsigned char flag )
            {
                return (( _flag & flag ) == flag);
            };

            inline bool IsReload()
            {
              return is_flag_set(FLG_CTRL_RELOAD);
            }
            inline bool IsStop()
            {
              return is_flag_set(FLG_CTRL_STOP);
            }

            inline void clear_flag()
            {
                _flag = (unsigned char)0;
            }

        protected:
            unsigned char _flag;

        };

        static CCheckFlag obj_checkflag;

}

static void sigusr2_handle(int sig_val)
{
    transfer::obj_checkflag.set_flag(transfer::FLG_CTRL_STOP);
    signal(SIGUSR2, sigusr2_handle);
}

static void sigusr1_handle(int sig_val)
{
    transfer::obj_checkflag.set_flag(transfer::FLG_CTRL_RELOAD);
    signal(SIGUSR1, sigusr1_handle);
}

#endif  // _TRANSFER_MCD_PROC_H_


