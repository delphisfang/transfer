#include "transfer_timer_info.h"
#include "transfer_mcd_proc.h"

#include <algorithm>

using namespace transfer;

extern char BUF[DATA_BUF_SIZE];

int SendMsgTimer::do_next_step(string& req_data)
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
			return on_req_state_svr();
        }
		
		case STATE_GET_IP:
		{
			if (on_rsp_state_svr(req_data, req_data.size()))
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
				return -1;
            }
			else
			{
				m_cur_step = STATE_END;
				return 1;
			}
        }

		default:
		{
			return -1;
		}
    }
}

int SendMsgTimer::resp_cp_first()
{
	Json::Value data = Json::objectValue;
	return on_send_reply(data);
}


// return -1 0 1
int SendMsgTimer::on_req_state_svr()
{
	if (false)// get from cache
	{
		// m_session_ip = xx;
		// m_session_port = xx;
		m_cur_step = STATE_SEND;
		return on_req_cp_send_msg();
	}
	else// get from status server
	{
		m_cur_step = STATE_GET_IP;

		Json::Value data;
		data["identity"]  = m_identity;
		if ("user" == m_identity)
		{
			data["userID"]    = m_userID;
		}
		else
		{
			data["serviceID"] = m_serviceID;
		}

		// send request to status server
		string   sip         = m_proc->m_cfg._state_server_ip;
		unsigned short sport = m_proc->m_cfg._state_server_port;
		return on_send_request("getChatProxyAddress", sip, sport, data);
	}
}

int SendMsgTimer::on_state_svr_error()
{
	Json::Value data = Json::objectValue;
	
	m_errno  = ERROR_STATE_SVR_ERROR;
	m_errmsg = "state_svr_error";
	on_error();
	return on_send_request("replyCpAddr", m_cpIP, m_cpPort, data);
}

// parse data from state server
int SendMsgTimer::on_rsp_state_svr(string req_data, int datalen)
{
	if (0 == m_state_svr_code)
	{
		return 0;
	}
	else
	{
		on_state_svr_error();
		return -1;
	}
}

int SendMsgTimer::on_req_cp_send_msg()
{
	Json::Value data;

	data["identity"]      = m_identity;
	data["appID"]         = m_appID;
	data["userID"]        = m_userID;
	data["serviceID"]     = m_serviceID;
	data["chatProxyIp"]   = m_session_ip;
	data["chatProxyPort"] = m_session_port;
	return on_send_request("replyCpAddr", m_cpIP, m_cpPort, data);
}

int SendMsgTimer::on_rsp_cp_send_msg(string req_data, int datalen)
{
	int retcode = 0;
	//int retcode = parse_retcode(req_data);

	if (retcode)
	{
		LogDebug("Success to get response from CP.");
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

void SendMsgTimer::on_expire()
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

SendMsgTimer::~SendMsgTimer()
{

}

