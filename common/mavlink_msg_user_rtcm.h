// MESSAGE USER_RTCM PACKING

#define MAVLINK_MSG_ID_USER_RTCM 13

typedef struct __mavlink_user_rtcm_t
{
	uint16_t rtcm_msg_type;
	uint16_t rtcm_msg_len;
	uint8_t rtcm_seq_total;
	uint8_t rtcm_seq;
	uint8_t rtcm_data[240];
} mavlink_user_rtcm_t;



#define MAVLINK_MSG_ID_USER_RTCM_LEN 255
#define MAVLINK_MSG_ID_13_LEN 255

#define MAVLINK_MSG_ID_USER_RTCM_CRC 0
#define MAVLINK_MSG_ID_13_CRC 0


static __inline uint16_t mavlink_msg_user_rtcm_encode(uint8_t rtcm_st_len,uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_user_rtcm_t* user_rtcm)
{
			memcpy(_MAV_PAYLOAD_NON_CONST(msg),user_rtcm,rtcm_st_len);
			msg->len = rtcm_st_len;
			msg->msgid = MAVLINK_MSG_ID_USER_RTCM;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, rtcm_st_len, MAVLINK_MSG_ID_USER_RTCM_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, rtcm_st_len);
#endif
}

static __inline uint16_t mavlink_msg_user_rtcm_encode_chan(uint8_t rtcm_st_len,uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_user_rtcm_t* user_rtcm)
{
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), user_rtcm, rtcm_st_len);
		msg->msgid = MAVLINK_MSG_ID_USER_RTCM;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, rtcm_st_len, MAVLINK_MSG_ID_USER_RTCM_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, rtcm_st_len);
#endif
}

//#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

//#if MAVLINK_MSG_ID_USER_RTCM_LEN <= MAVLINK_MAX_PAYLOAD_LEN
///*
//  This varient of _send() can be used to save stack space by re-using
//  memory from the receive buffer.  The caller provides a
//  mavlink_message_t which is the size of a full mavlink message. This
//  is usually the receive buffer for the channel, and allows a reply to an
//  incoming message with minimum stack space usage.
// */
//static __inline void mavlink_msg_user_rtcm_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan)
//{
//	mavlink_user_rtcm_t *packet = (mavlink_user_rtcm_t *)msgbuf;

//#if MAVLINK_CRC_EXTRA
//    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_USER_RTCM, (const char *)packet, MAVLINK_MSG_ID_USER_RTCM_LEN, MAVLINK_MSG_ID_USER_RTCM_CRC);
//#else
//    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_USER_RTCM, (const char *)packet, MAVLINK_MSG_ID_USER_RTCM_LEN);
//#endif
//}
//#endif

//#endif

/**
 * @brief Decode a user_rtcm message into a struct
 *
 * @param msg The message to decode
 * @param user_rtcm C-struct to decode the message contents into
 */
static __inline void mavlink_msg_user_rtcm_decode(const mavlink_message_t* msg, mavlink_user_rtcm_t* user_rtcm)
{
	memcpy(user_rtcm, _MAV_PAYLOAD(msg), msg->len);
}
