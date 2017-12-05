#include "txf_timer_info.h"
//#include "transfer_mcd_proc.h"
#include <algorithm>

using namespace tfc::base;
using namespace transfer;

extern char BUF[DATA_BUF_SIZE];

int CTimerInfo::init(string req_data, int datalen)
{
	Json::Reader reader;
	Json::Value js_req_root;
	Json::Value js_req_data;

	if(!reader.parse(req_data, js_req_root))
	{
		LogError("Error init SerializeToString Fail: %s\n", req_data.c_str());
		return -1;
	}
	if(!js_req_root["whereFrom"].isNull() && js_req_root["whereFrom"].isString())
	{
		m_whereFrom = js_req_root["whereFrom"].asString();
	}
	if(!js_req_root["cmd"].isNull() && js_req_root["cmd"].isString())
	{
		m_cmd = js_req_root["cmd"].asString();
	}
	if(!js_req_root["seq"].isNull() && js_req_root["seq"].isString())
	{
		m_seq = js_req_root["seq"].asString();
	}
	if(!js_req_root["appID"].isNull() && js_req_root["appID"].isUInt())
	{
		m_appID = js_req_root["appID"].asString();
	}
	if(!js_req_root["chatproxyIP"].isNull() && js_req_root["chatproxyIP"].isString())
	{
		m_chatproxyIP = js_req_root["chatproxyIP"].asString();
	}
	if(!js_req_root["chatproxyPort"].isNull() && js_req_root["chatproxyPort"].isUInt())
	{
		m_chatproxyPort = js_req_root["chatproxyPort"].asUInt();
	}
	if(js_req_root["data"].isNull() || !js_req_root["data"].isObject())
	{
		m_search_no = m_appID + "_" + i2str(m_msg_seq);
		return 0;
	}
	js_req_data = js_req_root["data"];
	m_data = js_req_data.toStyledString();

	if(!js_req_data["userInfo"].isNull() && js_req_data["userInfo"].isObject())
	{
		m_userInfo = js_req_data["userInfo"].toStyledString();
	}
	if(!js_req_data["identity"].isNull() && js_req_data["identity"].isString())
	{
		m_identity = js_req_data["identity"].asString();
	}
	if(!js_req_data["tag"].isNull() && js_req_data["tag"].isString())
	{
		m_tag = m_appID + "_" + js_req_data["tag"].asString();
	}
	if(!js_req_data["userID"].isNull() && js_req_data["userID"].isString())
	{
		m_userID = m_appID + "_" + js_req_data["userID"].asString();
	}
	if(!js_req_data["serviceID"].isNull() && js_req_data["serviceID"].isString())
	{
		m_serviceID = m_appID + "_" + js_req_data["serviceID"].asString();
	}
	if(!js_req_data["channel"].isNull() && js_req_data["channel"].isString())
	{
		m_channel = js_req_data["channel"].asString();
	}
	if(!js_req_data["extends"].isNull() && js_req_data["extends"].isObject())
	{
		m_extends = js_req_data["extends"].toStyledString();
	}
	if(!js_req_data["serviceName"].isNull() && js_req_data["serviceName"].isString())
	{
		m_serviceName = js_req_data["serviceName"].asString();
	}
	if(!js_req_data["serviceAvatar"].isNull() && js_req_data["serviceAvatar"].isString())
	{
		m_serviceAvatar = js_req_data["serviceAvatar"].asString();
	}
	if(!js_req_data["content"].isNull() && js_req_data["content"].isObject())
	{
		m_content = js_req_data["content"];
	}
	if(!js_req_data["changeServiceID"].isNull() && js_req_data["changeServiceID"].isString())
	{
		m_changeServiceID = m_appID + "_" + js_req_data["changeServiceID"].asString();
	}
	if(!js_req_data["lastServiceID"].isNull() && js_req_data["lastServiceID"].isString())
	{
		m_lastServiceID = m_appID + "_" + js_req_data["lastServiceID"].asString();
	}
	if(!js_req_data["priority"].isNull() && js_req_data["priority"].isUInt())
	{
		m_queuePriority = js_req_data["priority"].asUInt();
	}
	if(!js_req_data["tags"].isNull() && js_req_data["tags"].isArray())
	{
		Json::Value jsTags;
		jsTags = js_req_data["tags"];
		for(int i = 0; i < jsTags.size(); i++)
		{
			m_tags.insert(m_appID + "_" + jsTags[i].asString());
		}
	}
	if(!js_req_data["priority"].isNull() && js_req_data["priority"].isString())
	{
		m_priority = js_req_data["priority"].asString();
	}
	if(!js_req_data["services"].isNull() && js_req_data["services"].isArray())
	{
		Json::Value services;
		services = js_req_data["services"];
		for(int i = 0; i < services.size(); i++)
		{
			m_checkServices.insert(m_appID + "_" + services[i].asString());
		}
	}

	char id_buf[64];
    snprintf (id_buf, sizeof(id_buf), "%s:%s--%s:%d", m_appID.c_str(), m_serviceID.c_str(), m_userID.c_str(), m_msg_seq);
	m_search_no = string(id_buf);
	LogError("Init request data OK![%s][%s][%s][%s][%u]\n", m_identity.c_str(), m_serviceID.c_str(), m_userID.c_str(), m_cmd.c_str(), m_msg_seq);
	return 0;
}

int CTimerInfo::on_error()
{
	Json::Value error_rsp;
	error_rsp["cmd"]  = m_cmd + "-reply";
	error_rsp["seq"]  = m_seq;
	error_rsp["code"] = m_errno;
	error_rsp["msg"]  = m_errmsg;
	string rsp = error_rsp.toStyledString();  
	m_proc->EnququeHttp2CCD(m_ret_flow, (char*)rsp.c_str(), rsp.size());

	if (m_errno < 0)
	{
		Json::Value post_data;
		Json::Value post_array;
		Json::Value post_err;
		string err_str;
		string tmp_str;
		timeval nowTime;
		gettimeofday(&nowTime, NULL);

		post_err["project"] = "Transfer";
		post_err["module"] = "transfer";
		post_err["code"] = m_errno;
		post_err["desc"] = m_data;
		post_err["env"] = m_proc->m_cfg._env;
		post_err["ip"] = m_proc->m_cfg._local_ip;
		post_err["appid"] = m_appID;
		post_err["time"] = l2str(nowTime.tv_sec*1000 + nowTime.tv_usec / 1000);

		if (ERROR_NOT_READY == m_errno || ERROR_SESSION_WRONG  == m_errno)
		{
			post_err["level"] = 30;
		}
		else
		{
			post_err["level"] = 20;
		}

		post_array["headers"] = post_err;
		post_data.append(post_array);

		err_str = post_err.toStyledString();

		DEBUG_P(LOG_NORMAL, "just test which line\n");

		m_proc->EnququeErrHttp2DCC((char *)err_str.c_str(), err_str.size());
	}
	
	on_stat();
	return 0;
}

int CTimerInfo::on_stat()
{
	gettimeofday(&m_end_time, NULL);
	string staticEntry = m_identity + "_" + m_cmd;
	m_proc->AddStat(m_errno, staticEntry.c_str(), &m_start_time, &m_end_time);

	int32_t cur_errno = m_errno;
	if(errno < 0)
	{
		if(m_identity == "user")
		{
			m_proc->AddErrCmdMsg(m_appID, m_cmd, m_identity, m_userID, m_start_time, cur_errno);
		}
		else
		{
			m_proc->AddErrCmdMsg(m_appID, m_cmd, m_identity, m_serviceID, m_start_time, cur_errno);
		}
	}
}

void CTimerInfo::OpStart()
{
	gettimeofday(&m_op_start, NULL);
}

void CTimerInfo::on_expire()
{
	LogError("[on_expire] searchid[%s]:handle timer timeout, statue[%d].", m_search_no.c_str(), m_cur_step);

	Json::Value error_rsp;
	error_rsp["cmd"]  = m_cmd + "-reply";
	error_rsp["seq"]  = m_seq;
	error_rsp["code"] = ERROR_SYSTEM_WRONG;
	error_rsp["msg"]  = "System handle timeout";
	string rsp    = error_rsp.toStyledString();  
	m_proc->EnququeHttp2CCD(m_ret_flow, (char*)rsp.c_str(), rsp.size());
	
	return;
}

void CTimerInfo::OpEnd(const char* itemName, int retcode)
{
    struct timeval end;
    gettimeofday(&end, NULL);
	m_proc->AddStat(retcode, itemName, &m_op_start, &end);
}

void CTimerInfo::AddStatInfo(const char* itemName, timeval* begin, timeval* end, int retcode)
{
    m_proc->AddStat(retcode, itemName, begin, end);
}

void CTimerInfo::FinishStat(const char* itemName)
{
	m_proc->AddStat(m_errno, itemName, &m_start_time, &m_end_time);

	char buff[24] = {0};
    snprintf(buff, sizeof(buff), "ccd_%s", itemName);
	m_proc->AddStat(0, buff, &m_ccd_time, &m_end_time);
}




void CTimerInfo::on_error_parse_packet(string errmsg)
{
	m_errno  = ERROR_UNKNOWN_PACKET;
	m_errmsg = errmsg;
	on_error();
}

uint32_t CTimerInfo::GetMsgSeq()
{ 
	return m_msg_seq; 
}

uint64_t CTimerInfo::GetTimeGap()
{
	struct timeval end;
	gettimeofday(&end, NULL);

	uint64_t timecost = CalcTimeCost_MS(m_start_time, end);
	DEBUG_P(LOG_NORMAL, "##### Timer GetTimeGap() max_time_gap[%u], timecost[%u]\n"
					, m_max_time_gap, timecost);

	if (timecost > m_max_time_gap)
	{
		m_max_time_gap = 1;
	}
	else
	{
		m_max_time_gap -= timecost;
	}

	// m_op_start = end;

	return m_max_time_gap;
}

int CTimerInfo::on_send_request(string cmd, string ip, unsigned short port, const Json::Value &data, bool with_code)
{
	Json::Value req;
	req["appID"]	= m_appID;
	req["method"]	= cmd;
	req["innerSeq"] = m_msg_seq;
	req["data"] 	= data;
	if (with_code)
		req["code"] = 0;
	string strReq	= req.toStyledString();
	LogTrace("send request to <%s, %d>: %s", ip.c_str(), port, strReq.c_str());
	if (m_proc->EnququeHttp2DCC((char *)strReq.c_str(), strReq.size(), ip, port))
	{
		LogError("[%s]: Error send request %s", m_appID.c_str(), cmd.c_str());
		return -1;
	}
	return 0;
}

int CTimerInfo::on_send_reply(const Json::Value &data)
{
	Json::Value rsp;
	rsp["appID"]	= m_appID;
	rsp["method"]	= m_cmd + "-reply";
	rsp["innerSeq"] = m_seq;
	rsp["code"] 	= 0;
	rsp["data"] 	= data;
	string strRsp	= rsp.toStyledString();
	LogTrace("send response: %s", strRsp.c_str());
	if (m_proc->EnququeHttp2CCD(m_ret_flow, (char *)strRsp.c_str(), strRsp.size()))
	{
		LogError("searchid[%s]: Fail to SendReply <%s>\n", m_search_no.c_str(), m_cmd.c_str());
		m_errno  = ERROR_SYSTEM_WRONG;
		m_errmsg = "Error send to ChatProxy";
		on_error();
		return -1;
	}
	else
	{
		on_stat();
		return 0;
	}
}

