#ifndef _CC_PBX_H_
#define _CC_PBX_H_

#include<switch.h>
#include<stdbool.h>
#include<time.h>
#include<stddef.h>
#include<stdio.h>


#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define IS_NULL(x)  (x == NULL)?true:false
#define MAX 10
#define NULL ((void *)0)


// appointment struct start.  

typedef struct {
char *apmt_id;
char *apmt_name;
char *wc_prompt;
char * invalid_prompt;
char * tm_out_prompt;
char *wc_pmt_path;
char *invalid_pmt_path;
char *tm_pmt_path;
char *phone_no1;
char *phone_no2;
char *grp_name;
int contact_id;
int grp_contact_id;
char *name;
char *ext_no;
uint16_t  dig_tm_out;  uint16_t  intm_dig_out;   //seconds
uint8_t   mx_tm_out;
uint8_t  mx_invalid;
char    tm_interval [4];
bool is_ext;
bool is_pstn;



}apmt_details_t;


typedef struct {
int     id;
int    app_id;
char   *did;
char   *src;
char    date_slot[18];
//time_t   time_slot;
char    time_start[18];
char    time_end[18];
bool is_init;


}apmt_slots_details_t;


// appointment struct end.


typedef struct {
    bool is_dnd;                                                                                                  
    bool is_init;
    bool is_vm_on;
    bool is_recording_allowed;
    bool is_outbound_allowed;
    bool is_sd_allowed;
    bool is_call_frwd;
    bool is_call_trns;
    bool blacklist;
    bool is_admin;
    bool is_rng_ton;
    bool is_fmfm;
    uint8_t   dial_timeout; //dial time out
    char *cid_hdr_value;
    char *cid_name;
    char *external_cid;
    char *codec;
    unsigned int cid_hdr_type;
    unsigned int pkg_id;
    unsigned int rng_ton_id;
    char *  cust_id;
    unsigned int id;
    char num[20];
    char * ann_pmt;
    char * mobile;
    char * is_sms_allowed;
} ext_detail_t;


typedef struct {
	bool is_callee_did;
	bool is_did_free;
	bool is_billing_on;
	bool is_call_transferred;
	bool is_frwd_all;
	bool is_frwded;
	bool is_outbound;
	bool is_frwd_outbound;
	bool is_call_internal;
	bool is_call_inbound;
	bool is_inbound_sip;
	bool is_call_outbound;
	bool is_call_speeddial;
        bool is_outbound_mnt;
        bool is_outbound_grp_mnt;
}call_status_flag_t;


typedef struct{
	bool gw_grp_id;
	bool is_lcr_on;
	bool is_init;
        char * is_mnt;
        char * talktime_mnt;
        bool is_size;
	//         min_dur;
	//unsigned int incr_dur; // billing block
	char*  gw_id;
	unsigned int rate_card_id;
	unsigned int call_hdr_typ;
	unsigned int clr_id_hdr;
        float crdt_lmt;
        float blnce;
        int billing_typ;
 	char * clr_id;
	char * clr_id_hdr_vlue;
	char * clr_pfile;
	char*  call_plan_id;
	char* buy_rate;
	char* incr_dur;//billing block
	char*  min_dur;
	char* sell_rate;
	char* dial_prefix;
	char* bill_type;
	char* gw_ip;
	char* gw_port;
	char* gw_codec;
	char* gw_prepend;
	char* gw_prepend_cid;
        char * call_plan_rate_id;
        char* strp_clr_id;
        char* strp_clr_name;

}outbound_detail_t;

typedef struct{
        bool pkg_blng_typ;
        char*  dialout_id;
        bool is_init;
        bool is_size;
        char* dlout_cp_id;
        char*  gw_id;
        char* gw_ip;
        char* gw_port;
        char* gw_codec;
        char* gw_prepend;
        char* gw_prepend_cid;
        char* gw_hdr_typ;
        char* gw_caller_id;
        char * gw_clr_id_hdr;
        char* gw_id_prof;
        char* is_mnt_plan;
        char* remain_mnt;
        char* is_group;
        char* group_id;
        char* group_mnt; 
        char* dial_prefix;
        char* strp_clr_id;
        char* strp_clr_name;
}mnt_detail_t;

typedef struct {
	uint8_t type;// 0=Disabled,1=Voicemail,2=External,3=Extension
	char num[15];
}call_frwd_t;



typedef struct{
	bool is_init;
	bool is_recording_on;
	uint8_t grp_type;
	unsigned int ring_timeout;
	unsigned int moh;
        char * id;
	char* extensions;
}callgrp_details_t;

typedef struct{
        bool is_init;
        unsigned int ring_timeout;
        unsigned int id;
        char * fmfm1;
        char * fmfm2;
        char * fmfm3;         
}fmfm_details_t;

typedef struct{
	bool is_init;
	bool is_recording;
	char *ext;
	char *admin_pin;
	char *mem_pin;
	char * wc_prompt;
	unsigned int moh;
	char *  cust_id;
}conf_details_t;

typedef struct {
	bool is_init;
	bool is_blacklist_on;
	bool is_vm_on;
	bool is_recording_on;
	bool is_outbound_on;
	bool is_call_barging_on;
	uint8_t type;
	unsigned int  tm_gp_id;
	int id;
	char *  cust_id;
	char *  bill_type;
	unsigned int max_cc;
	unsigned int actv_ftr_id;
	unsigned int dst_id;
	double fixrate;
	char * selling_rate;
	char * conn_charge;
	char num[20]; //did_no
        int crdt_lmt;
        int blnce;
        int billing_typ;
}did_details_t;


typedef struct{
	bool is_init;
	bool unauth_usr;
	char q_name[20];
	char * file_path;
	char * welcome_pmt;
	char * ann_pmt;
	unsigned int max_wait_call;
	bool p_anc; // periodic announcement
	unsigned int p_anc_time;
	bool play_pos_on_call;
	bool play_pos_prdcly;
	bool stcky_agnt;
}queue_details_t;

typedef struct{
        bool is_init;
        
        char * id;
        char *cntct1;
        char * cntct2 ;
        char * mnt_id;
         int   mnt;
       char * tc_id; 
        }tc_details_t ;

typedef struct{
        bool is_init;
        int actve_ftre;
        int actve_val;
        }tc_failover;

typedef struct{
        bool is_init;
        char *id;
	int  rcd;
	int sat;
	char * agnt;
        }sta_details_t ;

typedef struct{
        bool is_init;
        char *id;
        int  rcd;
        char * agnt;
        }sta_cg_details_t ;


typedef struct{
	bool is_init;
	char ivr_name[20];
	char  ivr_prm[200];
	char  ivr_dgt[200];
	char * welcome_pmt;
	char * repeat_pmt;
	char * invalid_pmt;  
	char * timeout_pmt;
	unsigned int timeout;
	unsigned int dgt_timeout;
	unsigned int mx_timeout;
	unsigned int invld_count;
	unsigned int drct_ext_call;
	unsigned int is_extn_dtmf;
}ivr_details_t ; 
/*typedef struct{
  bool is_init;
  char* tm_start;
  char *  tm_finish;
  char *mt_st_dy;
  char * mt_ft_dy;
  char * sedule_weekly ;
  char * sedule_weekly_cstm;
  char * holidy;
  }time_details;*/

typedef struct{
bool is_init;
int bling_typ;
char* blnce;
char* crdt_lmt;
}feture_details_t;

typedef struct {
	ext_detail_t caller;
	ext_detail_t callee;
	did_details_t did;
	//call_frwd_t frwd[4]; // 4 -> all,busy,noans,unavail
	outbound_detail_t obd;
	call_status_flag_t flags;
	callgrp_details_t cg;
	conf_details_t conf;
	queue_details_t cq;
	ivr_details_t cv;
    tc_details_t tc;
    tc_failover  tf;
    sta_details_t sta;
    feture_details_t fd;
    // time_details tm;
    call_frwd_t frwd[4]; // 4 -> all,busy,noans,unavail
    fmfm_details_t fmfm;
    mnt_detail_t mnt;  
    apmt_details_t apmt;
    apmt_slots_details_t apmt_slots;

} call_details_t;

struct node
{
	char *data;
	char * key;
	struct node *next;
};


struct stack
{
        int maxsize;
        int top;
        int *items;
};



typedef struct {
  int items[MAX];
  int top;
} st;

// fs_dbh
switch_status_t execute_sql(char *odbc_dsn, char *sql, switch_mutex_t *mutex);
char *execute_sql2str(char *odbc_dsn, switch_mutex_t *mutex, char *sql, char *resbuf, size_t len);
switch_bool_t execute_sql_callback(char *odbc_dsn, switch_mutex_t *mutex, char *sql, switch_core_db_callback_func_t callback,void *pdata);


// cc_pbx_feature
switch_status_t handle_call(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call_info);
bool handle_sip_call(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
bool handle_sd(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
void eaves_drop(switch_core_session_t *session,const char * extension,call_details_t * call_info,char * dsn,switch_mutex_t *mutex) ;
void ivr(switch_core_session_t *session,uint8_t min,  uint8_t max, char* audio_file,const char * result, call_details_t * call_info,char * dsn,switch_mutex_t *mutex);
void feature_code(switch_core_session_t *session,const char *callee,const char * caller,call_details_t *  call_info,char * dsn,
		switch_mutex_t *mutex );
void valetpark(switch_core_session_t *session,const char *callee);
void park(switch_core_session_t *session,const char *callee);
char  ** split(char string[], int * num, char * sep);
void addLast(struct node **head, char * val,char * key);
struct node * search(struct  node *head,char *key);
void printList(struct node *head);
int  handle_cg_stcky_agnt(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);

//cc_pbx_function

//void  verify_apmt(switch_channel_t *channel,apmt_details_t *apmt,char * dsn,switch_mutex_t *mutex);
bool verify_apmt_slots(switch_channel_t *channel,call_details_t *call,char * dsn,switch_mutex_t *mutex);
void  handle_appointment(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call );
void  handle_appointment(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);


void handle_prompt(switch_channel_t *channel ,const char* dialstatus);
void handle_test(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex);
bool is_black_listed(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
//bool is_black_listed(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
bool is_black_list_outgoing(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
bool get_ext_details(switch_channel_t *channel,ext_detail_t *extn,char * dsn,switch_mutex_t *mutex,const char* num);
void forward_call(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,int type);
//void set_recording(switch_channel_t *channel,const char* type,call_details_t *call);
void set_recording(switch_channel_t *channel,const char* type,call_details_t *call,char * dsn,switch_mutex_t *mutex);
void voicemail(switch_core_session_t *session,char *check,char *auth,const char *num);
void bridge_call(switch_core_session_t *session,call_details_t *call,const char *dial_num);
void check_call_frwd(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
bool outbound(switch_core_session_t *session,char * dsn,switch_mutex_t *mutex,call_details_t *call,const char *num);
bool verify_did(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,did_details_t *did);
bool verify_internal_exten(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,const char *num);
void handle_conf(switch_channel_t *channel,call_details_t *call);
void handle_cg(switch_channel_t *channel,call_details_t *call,char * dsn,switch_mutex_t *mutex);
void handle_fmfm(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
void handle_ivr(switch_channel_t *channel ,char * dsn,switch_mutex_t *mutex,call_details_t *call,struct stack *pt,int ivr);
void handle_did_dest(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
void handle_tc_failover(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,char* tc_id);
void handle_queue(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,int i);
//void handle_ivr(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
 int  handle_stcky_agnt(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
void handle_play_bck(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,char* id);
int pop(struct stack *pt);
int peek(struct stack *pt);
void push(struct stack *pt, int x);
int isFull(struct stack *pt);
int isEmpty(struct stack *pt);
int size(struct stack *pt);
struct stack* newStack(int capacity);

//void handle_time(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex, unsigned int  num);
#endif

#ifndef _CC_PBX_H_
#define _CC_PBX_H_

#include<switch.h>
#include<stdbool.h>
#include<time.h>
#include<stddef.h>
#include<stdio.h>


#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))
#define IS_NULL(x)  (x == NULL)?true:false
#define MAX 10
#define NULL ((void *)0)


// appointment struct start.  

typedef struct {
char *apmt_id;
char *apmt_name;
char *wc_prompt;
char * invalid_prompt;
char * tm_out_prompt;
char *wc_pmt_path;
char *invalid_pmt_path;
char *tm_pmt_path;
char *phone_no1;
char *phone_no2;
char *grp_name;
int contact_id;
int grp_contact_id;
char *name;
char *ext_no;
uint16_t  dig_tm_out;  uint16_t  intm_dig_out;   //seconds
uint8_t   mx_tm_out;
uint8_t  mx_invalid;
char    tm_interval [4];
bool is_ext;
bool is_pstn;



}apmt_details_t;


typedef struct {
int     id;
int    app_id;
char   *did;
char   *src;
char    date_slot[18];
//time_t   time_slot;
char    time_start[18];
char    time_end[18];
bool is_init;


}apmt_slots_details_t;


// appointment struct end.


typedef struct {
    bool is_dnd;                                                                                                  
    bool is_init;
    bool is_vm_on;
    bool is_recording_allowed;
    bool is_outbound_allowed;
    bool is_sd_allowed;
    bool is_call_frwd;
    bool is_call_trns;
    bool blacklist;
    bool is_admin;
    bool is_rng_ton;
    bool is_fmfm;
    uint8_t   dial_timeout; //dial time out
    char *cid_hdr_value;
    char *cid_name;
    char *external_cid;
    char *codec;
    unsigned int cid_hdr_type;
    unsigned int pkg_id;
    unsigned int rng_ton_id;
    char *  cust_id;
    unsigned int id;
    char num[20];
    char * ann_pmt;
    char * mobile;
    char * is_sms_allowed;
} ext_detail_t;


typedef struct {
	bool is_callee_did;
	bool is_did_free;
	bool is_billing_on;
	bool is_call_transferred;
	bool is_frwd_all;
	bool is_frwded;
	bool is_outbound;
	bool is_frwd_outbound;
	bool is_call_internal;
	bool is_call_inbound;
	bool is_inbound_sip;
	bool is_call_outbound;
	bool is_call_speeddial;
        bool is_outbound_mnt;
        bool is_outbound_grp_mnt;
}call_status_flag_t;


typedef struct{
	bool gw_grp_id;
	bool is_lcr_on;
	bool is_init;
        char * is_mnt;
        char * talktime_mnt;
        bool is_size;
	//         min_dur;
	//unsigned int incr_dur; // billing block
	char*  gw_id;
	unsigned int rate_card_id;
	unsigned int call_hdr_typ;
	unsigned int clr_id_hdr;
        float crdt_lmt;
        float blnce;
        int billing_typ;
 	char * clr_id;
	char * clr_id_hdr_vlue;
	char * clr_pfile;
	char*  call_plan_id;
	char* buy_rate;
	char* incr_dur;//billing block
	char*  min_dur;
	char* sell_rate;
	char* dial_prefix;
	char* bill_type;
	char* gw_ip;
	char* gw_port;
	char* gw_codec;
	char* gw_prepend;
	char* gw_prepend_cid;
        char * call_plan_rate_id;
        char* strp_clr_id;
        char* strp_clr_name;

}outbound_detail_t;

typedef struct{
        bool pkg_blng_typ;
        char*  dialout_id;
        bool is_init;
        bool is_size;
        char* dlout_cp_id;
        char*  gw_id;
        char* gw_ip;
        char* gw_port;
        char* gw_codec;
        char* gw_prepend;
        char* gw_prepend_cid;
        char* gw_hdr_typ;
        char* gw_caller_id;
        char * gw_clr_id_hdr;
        char* gw_id_prof;
        char* is_mnt_plan;
        char* remain_mnt;
        char* is_group;
        char* group_id;
        char* group_mnt; 
        char* dial_prefix;
        char* strp_clr_id;
        char* strp_clr_name;
}mnt_detail_t;

typedef struct {
	uint8_t type;// 0=Disabled,1=Voicemail,2=External,3=Extension
	char num[15];
}call_frwd_t;



typedef struct{
	bool is_init;
	bool is_recording_on;
	uint8_t grp_type;
	unsigned int ring_timeout;
	unsigned int moh;
        char * id;
	char* extensions;
}callgrp_details_t;

typedef struct{
        bool is_init;
        unsigned int ring_timeout;
        unsigned int id;
        char * fmfm1;
        char * fmfm2;
        char * fmfm3;         
}fmfm_details_t;

typedef struct{
	bool is_init;
	bool is_recording;
	char *ext;
	char *admin_pin;
	char *mem_pin;
	char * wc_prompt;
	unsigned int moh;
	char *  cust_id;
}conf_details_t;

typedef struct {
	bool is_init;
	bool is_blacklist_on;
	bool is_vm_on;
	bool is_recording_on;
	bool is_outbound_on;
	bool is_call_barging_on;
	uint8_t type;
	unsigned int  tm_gp_id;
	int id;
	char *  cust_id;
	char *  bill_type;
	unsigned int max_cc;
	unsigned int actv_ftr_id;
	unsigned int dst_id;
	double fixrate;
	char * selling_rate;
	char * conn_charge;
	char num[20]; //did_no
        int crdt_lmt;
        int blnce;
        int billing_typ;
}did_details_t;


typedef struct{
	bool is_init;
	bool unauth_usr;
	char q_name[20];
	char * file_path;
	char * welcome_pmt;
	char * ann_pmt;
	unsigned int max_wait_call;
	bool p_anc; // periodic announcement
	unsigned int p_anc_time;
	bool play_pos_on_call;
	bool play_pos_prdcly;
	bool stcky_agnt;
}queue_details_t;

typedef struct{
        bool is_init;
        
        char * id;
        char *cntct1;
        char * cntct2 ;
        char * mnt_id;
         int   mnt;
       char * tc_id; 
        }tc_details_t ;

typedef struct{
        bool is_init;
        int actve_ftre;
        int actve_val;
        }tc_failover;

typedef struct{
        bool is_init;
        char *id;
	int  rcd;
	int sat;
	char * agnt;
        }sta_details_t ;

typedef struct{
        bool is_init;
        char *id;
        int  rcd;
        char * agnt;
        }sta_cg_details_t ;


typedef struct{
	bool is_init;
	char ivr_name[20];
	char  ivr_prm[200];
	char  ivr_dgt[200];
	char * welcome_pmt;
	char * repeat_pmt;
	char * invalid_pmt;  
	char * timeout_pmt;
	unsigned int timeout;
	unsigned int dgt_timeout;
	unsigned int mx_timeout;
	unsigned int invld_count;
	unsigned int drct_ext_call;
	unsigned int is_extn_dtmf;
}ivr_details_t ; 
/*typedef struct{
  bool is_init;
  char* tm_start;
  char *  tm_finish;
  char *mt_st_dy;
  char * mt_ft_dy;
  char * sedule_weekly ;
  char * sedule_weekly_cstm;
  char * holidy;
  }time_details;*/

typedef struct{
bool is_init;
int bling_typ;
char* blnce;
char* crdt_lmt;
}feture_details_t;

typedef struct {
	ext_detail_t caller;
	ext_detail_t callee;
	did_details_t did;
	//call_frwd_t frwd[4]; // 4 -> all,busy,noans,unavail
	outbound_detail_t obd;
	call_status_flag_t flags;
	callgrp_details_t cg;
	conf_details_t conf;
	queue_details_t cq;
	ivr_details_t cv;
    tc_details_t tc;
    tc_failover  tf;
    sta_details_t sta;
    feture_details_t fd;
    // time_details tm;
    call_frwd_t frwd[4]; // 4 -> all,busy,noans,unavail
    fmfm_details_t fmfm;
    mnt_detail_t mnt;  
    apmt_details_t apmt;
    apmt_slots_details_t apmt_slots;

} call_details_t;

struct node
{
	char *data;
	char * key;
	struct node *next;
};


struct stack
{
        int maxsize;
        int top;
        int *items;
};



typedef struct {
  int items[MAX];
  int top;
} st;

// fs_dbh
switch_status_t execute_sql(char *odbc_dsn, char *sql, switch_mutex_t *mutex);
char *execute_sql2str(char *odbc_dsn, switch_mutex_t *mutex, char *sql, char *resbuf, size_t len);
switch_bool_t execute_sql_callback(char *odbc_dsn, switch_mutex_t *mutex, char *sql, switch_core_db_callback_func_t callback,void *pdata);


// cc_pbx_feature
switch_status_t handle_call(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call_info);
bool handle_sip_call(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
bool handle_sd(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
void eaves_drop(switch_core_session_t *session,const char * extension,call_details_t * call_info,char * dsn,switch_mutex_t *mutex) ;
void ivr(switch_core_session_t *session,uint8_t min,  uint8_t max, char* audio_file,const char * result, call_details_t * call_info,char * dsn,switch_mutex_t *mutex);
void feature_code(switch_core_session_t *session,const char *callee,const char * caller,call_details_t *  call_info,char * dsn,
		switch_mutex_t *mutex );
void valetpark(switch_core_session_t *session,const char *callee);
void park(switch_core_session_t *session,const char *callee);
char  ** split(char string[], int * num, char * sep);
void addLast(struct node **head, char * val,char * key);
struct node * search(struct  node *head,char *key);
void printList(struct node *head);
int  handle_cg_stcky_agnt(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);

//cc_pbx_function

//void  verify_apmt(switch_channel_t *channel,apmt_details_t *apmt,char * dsn,switch_mutex_t *mutex);
bool verify_apmt_slots(switch_channel_t *channel,call_details_t *call,char * dsn,switch_mutex_t *mutex);
void  handle_appointment(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call );
void  handle_appointment(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);


void handle_prompt(switch_channel_t *channel ,const char* dialstatus);
void handle_test(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex);
bool is_black_listed(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
//bool is_black_listed(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
bool is_black_list_outgoing(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
bool get_ext_details(switch_channel_t *channel,ext_detail_t *extn,char * dsn,switch_mutex_t *mutex,const char* num);
void forward_call(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,int type);
//void set_recording(switch_channel_t *channel,const char* type,call_details_t *call);
void set_recording(switch_channel_t *channel,const char* type,call_details_t *call,char * dsn,switch_mutex_t *mutex);
void voicemail(switch_core_session_t *session,char *check,char *auth,const char *num);
void bridge_call(switch_core_session_t *session,call_details_t *call,const char *dial_num);
void check_call_frwd(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
bool outbound(switch_core_session_t *session,char * dsn,switch_mutex_t *mutex,call_details_t *call,const char *num);
bool verify_did(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,did_details_t *did);
bool verify_internal_exten(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,const char *num);
void handle_conf(switch_channel_t *channel,call_details_t *call);
void handle_cg(switch_channel_t *channel,call_details_t *call,char * dsn,switch_mutex_t *mutex);
void handle_fmfm(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
void handle_ivr(switch_channel_t *channel ,char * dsn,switch_mutex_t *mutex,call_details_t *call,struct stack *pt,int ivr);
void handle_did_dest(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
void handle_tc_failover(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,char* tc_id);
void handle_queue(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,int i);
//void handle_ivr(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
 int  handle_stcky_agnt(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call);
void handle_play_bck(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,char* id);
int pop(struct stack *pt);
int peek(struct stack *pt);
void push(struct stack *pt, int x);
int isFull(struct stack *pt);
int isEmpty(struct stack *pt);
int size(struct stack *pt);
struct stack* newStack(int capacity);

//void handle_time(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex, unsigned int  num);
#endif

