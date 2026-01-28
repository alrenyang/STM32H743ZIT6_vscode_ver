/*
 * tcp_ip.c
 *
 *  Created on: Dec 3, 2025
 *      Author: alrenm3
 */

#include "tcp_ip.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"
#include <stdio.h> // for printf
#include <string.h>
#include <stdbool.h>
#include "lwip/opt.h" // For LWIP_UNUSED_ARG
#include "cmsis_os.h"


// Global TCP PCB pointer
struct tcp_pcb *g_active_tpcb = NULL;
extern volatile bool btcp_connect;
extern uint8_t gete_way_ip[4];
extern uint8_t user_ip[4];
extern uint16_t Port_No;

// TCP callback function prototypes (static to this file)
static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void error_callback(void *arg, err_t err);
static err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);

bool tcp_server_init(uint16_t Port_No)
{
    struct tcp_pcb *pcb;
    err_t err;

    // 1. 새로운 TCP PCB 생성
    pcb = tcp_new();
    if (pcb == NULL) {
        printf("Failed to create new pcb\n");
        return false;
    }

    // 2. IP 주소와 포트에 바인딩
    err = tcp_bind(pcb, IP_ADDR_ANY, Port_No);
    if (err != ERR_OK) {
        printf("Failed to bind pcb, error: %d\n", err);
        tcp_close(pcb);
        return false;
    }

    // 3. 연결 수신 대기 시작
    pcb = tcp_listen(pcb);
    if (pcb == NULL) {
        printf("Failed to listen on pcb\n");
        return false;
    }

    // 4. 연결 수락 콜백 함수 지정
    tcp_accept(pcb, accept_callback);

    printf("TCP Echo server started on port 7\n");

    return true;
}

// 5. 클라이언트 연결 시 호출되는 콜백
static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);

    printf("Client connected\n");

    g_active_tpcb = newpcb;

    btcp_connect = true;

	  // Send a response
	  char *response = "[-]Welcome from MCU Connected TCP/IP....\n";
	  tcp_send_data((uint8_t*)response, strlen(response));
	  osDelay(250);
	  char ip_info_str[100];
	  sprintf(ip_info_str, "[-]nGateway IP: %d.%d.%d.%d \n",
	  gete_way_ip[0], gete_way_ip[1], gete_way_ip[2], gete_way_ip[3]);
	  tcp_send_data((uint8_t*)ip_info_str, strlen(ip_info_str));
	  osDelay(250);

	  sprintf(ip_info_str, "[-]nUser IP:%d.%d.%d.%d\n-]Port No: %d\n",
	  user_ip[0], user_ip[1], user_ip[2], user_ip[3],Port_No);
	  tcp_send_data((uint8_t*)ip_info_str, strlen(ip_info_str));
	  osDelay(250);

    // 수신, 전송, 오류 콜백 함수 설정
    tcp_recv(newpcb, recv_callback);
    tcp_sent(newpcb, sent_callback);
    tcp_err(newpcb, error_callback);

    return ERR_OK;
}

void tcp_ip_config(struct netif *netif, uint8_t *user_ip, uint8_t *gete_way_ip)
{
  ip4_addr_t ipaddr;
  ip4_addr_t netmask;
  ip4_addr_t gw;

  uint8_t netmask_addr[4] = {255, 255, 255, 0}; // Common netmask

  IP4_ADDR(&ipaddr, user_ip[0], user_ip[1], user_ip[2], user_ip[3]);
  IP4_ADDR(&netmask, netmask_addr[0], netmask_addr[1], netmask_addr[2], netmask_addr[3]);
  IP4_ADDR(&gw, gete_way_ip[0], gete_way_ip[1], gete_way_ip[2], gete_way_ip[3]);

  netif_set_addr(netif, &ipaddr, &netmask, &gw);
}

// 6. 데이터 수신 시 호출되는 콜백
static err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    LWIP_UNUSED_ARG(arg);

    // 연결이 종료되었거나 데이터가 없는 경우
    if (p == NULL) {
        printf("Connection closed by remote host\n");
        tcp_close(tpcb);
        g_active_tpcb = NULL;
		btcp_connect = false;
        return ERR_OK;
    }

    // 오류 발생 시
    if (err != ERR_OK) {
        printf("Receive error: %d\n", err);
        pbuf_free(p);
        return err;
    }

    // Copy data to shared buffer
    if (p->tot_len > 0) {
		if (p->tot_len < TCP_RECV_BUF_SIZE) {
			memcpy(g_tcp_recv_buffer, p->payload, p->tot_len);
			g_tcp_recv_buffer[p->tot_len] = '\0'; // Null-terminate for string operations
			g_tcp_recv_len = p->tot_len;
			g_tcp_new_data_flag = true;
		} else {
			memcpy(g_tcp_recv_buffer, p->payload, TCP_RECV_BUF_SIZE -1);
			g_tcp_recv_buffer[TCP_RECV_BUF_SIZE-1] = '\0'; // Null-terminate
			g_tcp_recv_len = TCP_RECV_BUF_SIZE;
			g_tcp_new_data_flag = true;
			printf("Received data truncated\n");
		}
    }
#if USE_ECHO == 1
    tcp_echo_data(tpcb, p);
#endif
    // 7. 수신 버퍼 비웠음을 알림
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p); // pbuf 메모리 해제

    return ERR_OK;
}

void tcp_echo_data(struct tcp_pcb *tpcb, struct pbuf *p)
{
    err_t err;
    // 받은 데이터를 다시 전송 (에코)
    if (tcp_sndbuf(tpcb) > p->tot_len) {
        err = tcp_write(tpcb, p->payload, p->tot_len, TCP_WRITE_FLAG_COPY);
        if (err != ERR_OK) {
            printf("Error writing to TCP stream: %d\n", err);
        }
    } else {
        printf("Not enough space in TCP send buffer\n");
    }
}

// 전송 완료 시 호출되는 콜백 (특별한 처리가 필요 없으면 비워둠)
static err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(tpcb);
    LWIP_UNUSED_ARG(len);

    // printf("Sent %d bytes\n", len);

    return ERR_OK;
}

// 오류 발생 시 호출되는 콜백
static void error_callback(void *arg, err_t err)
{
    LWIP_UNUSED_ARG(arg);
    printf("TCP error callback: error %d\n", err);
    g_active_tpcb = NULL;
    btcp_connect = false;
}

void tcp_send_data(uint8_t *data, uint16_t len)
{
    if (g_active_tpcb != NULL) {
        if (tcp_sndbuf(g_active_tpcb) > len) {
            err_t err = tcp_write(g_active_tpcb, data, len, TCP_WRITE_FLAG_COPY);
            if (err != ERR_OK) {
                printf("Error writing to TCP stream: %d\n", err);
            } else {
                // Force the data to be sent
                tcp_output(g_active_tpcb);
            }
        } else {
            printf("Not enough space in TCP send buffer\n");
        }
    } else {
        printf("No active TCP connection\n");
    }
}
