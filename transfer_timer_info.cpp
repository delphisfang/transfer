#include "transfer_timer_info.h"
#include "transfer_mcd_proc.h"

#include <algorithm>

using namespace transfer;

extern char BUF[DATA_BUF_SIZE];

#if 0
int SendMsgTimerInfo::do_next_step(string& req_data)
{
    switch (m_cur_step)
    {
        case STATE_INIT:
        {
            if (init(req_data, req_data.size()))
    		{
				ON_ERROR_PARSE_PACKET();
    			m_cur_step = STATE_END;
				return -1;
    		}

			resp_cp_first();
			return on_req_state_svr_get_cp_addr();
        }
		
		case STATE_GET_IP:
		{
			if (on_rsp_state_svr_get_cp_addr(req_data, req_data.size()))
			{
    			m_cur_step = STATE_END;
				return -1;
			}
			return on_req_cp_send_msg();
		}
		
        case STATE_SEND:
        {
            if (on_rsp_cp_send_msg(req_data, req_data.size()))
            {
    			m_cur_step = STATE_END;
				return -1;//fail, delete timer
            }
			else
			{
				m_cur_step = STATE_END;
				return 1; //success, delete timer
			}
        }

		default:
		{
			return -1;//error, delete timer
		}
    }
}

int SendMsgTimerInfo::on_req_state_svr_get_cp_addr()
{
	if (false)// get from cache
	{
		// m_session_ip = xx;
		// m_session_port = xx;
		m_cur_step = STATE_SEND;
		return on_req_cp_send_msg(); // return -1 0 1
	}
	else
	{
		// send get req to state server
		m_cur_step = STATE_GET_IP;

		Json::Value data;
		string sip;
		unsigned short sport;

		data["identity"]  = m_identity;
		data["userID"]    = m_userID;
		data["serviceID"] = m_serviceID;
		sip   = m_proc->m_cfg._state_server_ip;
		sport = m_proc->m_cfg._state_server_port;
		on_send_request("getCpAddr", sip, sport, data);
	}
}

int SendMsgTimerInfo::on_state_svr_error()
{
	Json::Value data;
	
	m_errno = ERROR_STATE_SVR_ERROR;
	// m_errmsg = "state_svr_error:" + parse_error(req_data);
	on_error();
	return on_send_request("replyCpAddr", m_chatproxyIP, m_chatproxyPort, data);
}

// parse data from state server
int SendMsgTimerInfo::on_rsp_state_svr_get_cp_addr(string req_data, int datalen)
{
	///TODO
	//int retcode = parse_retcode(req_data);
	int retcode = 0;
	
	if (retcode == 0)
	{
		m_session_ip   = xx;
		m_session_port = xx;

		///TODO
		m_session_ip = "12.34.56.78";
		m_session_port = 1234;
		return 0;
	}
	else
	{
		on_state_svr_error();
		return -1;
	}	
}

int SendMsgTimerInfo::on_req_cp_send_msg()
{
	Json::Value data;

	data["identity"]  = m_identity;
	data["appID"]     = m_appID;
	data["userID"]    = m_userID;
	data["serviceID"] = m_serviceID;
	data["chatproxyIp"] = xxx;
	data["chatproxyPort"] = yyy;
	return on_send_request("replyCpAddr", m_chatproxyIP, m_chatproxyPort, data);
}

int SendMsgTimerInfo::on_rsp_cp_send_msg(string req_data, int datalen)
{
	int retcode = 0;
	//int retcode = parse_retcode(req_data);

	if (retcode == 0)
	{
		LogDebug("Success to send response to CP.");
		return 0;
	}
	else
	{
		// m_errno = ERROR_STATE_SVR_ERROR;
	    // m_errmsg = "state_svr_error:" + parse_error(req_data);
	    on_error();
		return -1;
	}
}

// resp to cp
int SendMsgTimerInfo::resp_cp_first()
{
	Json::Value data = Json::objectValue;
	return on_send_reply(data);
}

void SendMsgTimerInfo::on_expire()
{
	if (m_retry_times >= m_proc->m_cfg._trsf_max_retry_times)
	{
		m_expire_del = true;
		LogError("[on_expire] searchid[%s]:handle timer timeout, state[%d].", m_search_no.c_str(), m_cur_step);
	}
	else
	{
		on_req_cp_send_msg();
		m_retry_times ++;
	}
	
	return;
}


SendMsgTimerInfo::~SendMsgTimerInfo()
{

}
#endif
