/*
 * tcp_ip.h
 *
 *  Created on: Dec 3, 2025
 *      Author: alrenm3
 */

#ifndef SRC_TCP_IP_H_
#define SRC_TCP_IP_H_

#define USE_ECHO	1	//에코 기능

#include "lwip/tcp.h"
#include <stdbool.h>
#include "lwip/netif.h" // Required for struct netif

#define TCP_RECV_BUF_SIZE 1024/2
extern uint8_t g_tcp_recv_buffer[TCP_RECV_BUF_SIZE];
extern volatile uint32_t g_tcp_recv_len;
extern volatile bool g_tcp_new_data_flag;

extern struct tcp_pcb *g_active_tpcb;

bool tcp_server_init(uint16_t Port_No);

#if USE_ECHO == 1
void tcp_echo_data(struct tcp_pcb *tpcb, struct pbuf *p);
#endif

void tcp_send_data(uint8_t *data, uint16_t len);
void tcp_ip_config(struct netif *netif, uint8_t *user_ip, uint8_t *gete_way_ip);

#endif /* SRC_TCP_IP_H_ */
