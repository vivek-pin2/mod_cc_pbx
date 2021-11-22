#include "cc_pbx.h"


static int exten_init_callback(void *parg,int argc,char **argv,char **column_names){

   ext_detail_t *exten = (ext_detail_t *) parg;
 /*   if (argc < 17){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Callback failed\n");                                
        return -1;
    }*/

   
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Callback count : %d\n",argc);                                
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"exten : %d %s %s\n",i,column_names[i],argv[i]);

    exten -> pkg_id               = atoi(argv[0]);
     if(!IS_NULL(argv[1]) && strlen(argv[1])){

    exten -> cust_id              = strdup(argv[1]);
    }
    if(!IS_NULL(argv[2]) && strlen(argv[2])){
        exten -> cid_name  = strdup(argv[2]);
    }
    exten -> dial_timeout         = atoi(argv[3]);
    if(!IS_NULL(argv[4]) && strlen(argv[4])){
        exten -> external_cid = strdup(argv[4]);
    }
    exten -> cid_hdr_type         = atoi(argv[5]);
    if(!IS_NULL(argv[6]) && strlen(argv[6])){
        exten -> cid_hdr_value    = strdup(argv[6]);
    }
    if(!IS_NULL(argv[7]) && strlen(argv[7])){
        exten -> codec            = strdup(argv[7]);
    }
    exten -> is_vm_on             = atoi(argv[8]);
    exten -> is_dnd               = atoi(argv[9]);
    exten -> is_outbound_allowed  = atoi(argv[10]);
    exten -> is_recording_allowed = atoi(argv[11]);
    exten -> is_call_frwd         = atoi(argv[12]);
    exten -> blacklist            = atoi(argv[13]);
    exten -> is_call_trns         = atoi(argv[14]);
    exten -> id                   = atoi(argv[15]);
    exten -> is_sd_allowed        = atoi(argv[16]);
    if(!IS_NULL(argv[17]) && strlen(argv[17])){
        strcpy(exten -> num,argv[17]);
    }
      if(strcmp(argv[18],"")){
              exten ->ann_pmt=strdup(argv[18]);
     }
     else{
	exten ->ann_pmt="NULL";
	}
   if(!IS_NULL(argv[19]) && strlen(argv[19])){
   exten ->mobile=strdup(argv[19]);

   
   }
   if(!IS_NULL(argv[20]) && strlen(argv[20])){
    
             exten->is_sms_allowed =strdup(argv[20]);
     }

    exten -> is_admin        = atoi(argv[21]);
    exten ->is_rng_ton      = atoi(argv[22]);
    exten  ->is_fmfm         = atoi(argv[23]);
    exten  ->rng_ton_id      = atoi(argv[24]); 
    exten -> is_init              = true;
    return 0;
}


   

bool get_ext_details(switch_channel_t *channel,ext_detail_t *extn,char * dsn,switch_mutex_t *mutex,const char* num){
    char *query = NULL;
    const char* ext_id = switch_channel_get_variable(channel,"ext_id");
     switch_channel_set_variable(channel,"ann_pmt","0");
 //   switch_channel_set_variable(channel,"application","intercom"); 
  if(IS_NULL(ext_id)){
        /*query = switch_mprintf("SELECT package_id,customer_id,caller_id_name,ring_time_out,\
            external_caller_id,caller_id_header_type,caller_id_header_value,codec,\
            voicemail,dnd,outbound,recording,forward,black_list,call_transfer,id,speed_dial,ext_number\
            FROM extension_master\
            WHERE status='1' AND ext_number='%s'",num);*/
          query = switch_mprintf("SELECT e.package_id,e.customer_id,e.caller_id_name,e.dial_time_out,e.external_caller_id\
             ,e.caller_id_header_type,e.caller_id_header_value,e.codec,e.voicemail,e.dnd,e.outbound,e.recording,e.forward\
             ,e.black_list,e.call_transfer,e.id,e.speed_dial,e.ext_number ,v.announceCallerID,e.mobile ,e.send_misscall_notification\
             ,e.admin,e.ringtone,e.find_me_follow_me,e.ringtone_id FROM extension_master e left join pbx_voicemail v on(e.id=v.extension_id)\
              WHERE e.status='1' AND  e.ext_number='%s' limit 1 ", num);
    }else{
      /*query = switch_mprintf("SELECT package_id,customer_id,caller_id_name,ring_time_out,\
            external_caller_id,caller_id_header_type,caller_id_header_value,codec,\
            voicemail,dnd,outbound,recording,forward,black_list,call_transfer,id,speed_dial,ext_number\
            FROM extension_master\
            WHERE status='1' AND id='%s'",ext_id);*/
           query = switch_mprintf("SELECT e.package_id,e.customer_id,e.caller_id_name,e.dial_time_out,e.external_caller_id\
             ,e.caller_id_header_type,e.caller_id_header_value,e.codec,e.voicemail,e.dnd,e.outbound,e.recording,e.forward\
             ,e.black_list,e.call_transfer,e.id,e.speed_dial,e.ext_number ,v.announceCallerID,e.mobile,e.send_misscall_notification\
             ,e.admin,e.ringtone,e.find_me_follow_me,e.ringtone_id FROM extension_master e left join pbx_voicemail v on(e.id=v.extension_id) \
              WHERE e.status='1' AND e.id='%s' limit 1",ext_id);
    }
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",query);
     execute_sql_callback(dsn,mutex,query,exten_init_callback,extn);
     switch_safe_free(query);
    // switch_channel_set_variable(channel,"ann_pmt",exten ->ann_pmt);
    if(!extn->is_init){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"failed to get extension details.\n");
        return false;
    }
    switch_channel_set_variable(channel,"ann_pmt",extn ->ann_pmt);

    return true;
}
bool is_black_listed(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call){

    char *query = NULL;
    const char* caller;
    char result[20] = {'\0'};
   

    caller = switch_channel_get_variable(channel,"sip_from_user");
    if(strlen(caller)==4)
       caller=switch_mprintf("%s%s",call->callee.cust_id,caller);
    query = switch_mprintf("select phone_number \
                from pbx_black_list \
                where status='1' and (blocked_for='1' or blocked_for='3') \
                and customer_id=%s \
                and extension_id in (0,%d) \
                and phone_number=%s",call->callee.cust_id,call->callee.id,caller);
    execute_sql2str(dsn,mutex,query,result,NELEMS(result));
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"result : %s :: Query : %s :: %d\n",result,query,IS_NULL(result));
    switch_safe_free(query);
    return  strlen(result)?true:false;
}

bool is_black_list_outgoing(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call){

    char *query = NULL;
    const char* caller;
    char result[20] = {'\0'};


    caller = switch_channel_get_variable(channel,"sip_to_user");
    query = switch_mprintf("select phone_number \
                from pbx_black_list \
                where status='1' \
                and (blocked_for='2' or blocked_for='3')\
                and customer_id=%s \
                and extension_id in (0,%d) \
                and phone_number=%s",call->caller.cust_id,call->caller.id,caller);
    execute_sql2str(dsn,mutex,query,result,NELEMS(result));
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"result : %s :: Query : %s :: %d\n",result,query,IS_NULL(result));
    switch_safe_free(query);
    return  strlen(result)?true:false;
}

/*bool is_black_listed(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call){
    
    char *query = NULL;
    const char* callee;
    char result[20] = {'\0'};

    callee = switch_channel_get_variable(channel,"sip_to_user");
    query = switch_mprintf("select phone_number \
                from pbx_black_list \
                where status='1' \
                and customer_id=%s \
                and extension_id in (0,%d) \
                and phone_number=%s",call->callee.cust_id,call->caller.id,callee); 
    execute_sql2str(dsn,mutex,query,result,NELEMS(result));
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"result : %s :: Query : %s :: %d\n",result,query,IS_NULL(result));    
    switch_safe_free(query);
    return  strlen(result)?true:false;
}*/

void set_recording(switch_channel_t *channel,const char* type,call_details_t *call,char * dsn,switch_mutex_t *mutex){
    
    char *tmp_str = NULL;
    switch_time_exp_t tm;
    char date[50] = "";
    switch_size_t retsize;
    switch_time_t ts;
    const char* callee = switch_channel_get_variable(channel,"sip_req_user");
    char * insert_str=NULL;
    switch_core_session_t* session = switch_channel_get_session(channel); 
    const char recording_path[] = "/home/cloudconnect/pbx_new/upload";
    
    ts = switch_time_now();
    switch_time_exp_lt(&tm, ts);
    switch_strftime(date,&retsize,sizeof(date),"%Y-%m-%d-%T",&tm);
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"RECORD DETAILS: Type:%s DATE:%s Caller:%s\n",type,callee,date);    

    switch_core_session_execute_application(session,"set","RECORD_STEREO=true");
    switch_core_session_execute_application(session,"set","media_bug_answer_req=true");
    tmp_str = switch_mprintf("%s/%s/recording/%s_clr%s_cle%s_%s.wav",recording_path,call->caller.cust_id,type,call->caller.num,callee,date);
    switch_core_session_execute_application(session,"record_session",tmp_str);
    insert_str=switch_mprintf("INSERT INTO `cc_master`.`pbx_recording` (`file_name`, `customer_id`, `src`,\
               `type`, `dest`, `status`, `created_at`) VALUES\
                ('%s_clr%s_cle%s_%s.wav', '%s', '%s', '%s', %s, '1', '%s')\
                ",type,call->caller.num,callee,date,call->caller.cust_id,call->caller.num,type,callee,date);
    execute_sql(dsn,insert_str,mutex); 
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"insert_string : %s",insert_str);
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"tmp_string : %s",tmp_str);
    switch_channel_set_variable(channel,"set_recording","1");
    switch_channel_set_variable(channel,"recording_path",recording_path);
    switch_channel_set_variable(channel,"rec_cust_id",call->caller.cust_id);
    switch_channel_set_variable(channel,"rec_type",type);
    switch_channel_set_variable(channel,"rec_num",call->caller.num);
    switch_channel_set_variable(channel,"rec_callee",callee);
    switch_channel_set_variable(channel,"rec_date",date);
    switch_safe_free(tmp_str);
    switch_safe_free(insert_str);

}

void voicemail(switch_core_session_t *session,char *check,char *auth,const char *num){
	char *tmp_str = NULL;
        switch_channel_t * channel = switch_core_session_get_channel(session); 
        const char *ann_pmt = switch_channel_get_variable(channel,"ann_pmt");
       
       switch_channel_set_variable(channel,"call_type","call_voicemail");
     //   const char *ann_pmt = switch_channel_get_variable(channel,"ann_pmt");
        //switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"vivek%s",ann_pmt);

	tmp_str = switch_mprintf("%s %s default 119.18.55.154  %s",check,auth,num);
         if(strcmp(ann_pmt,"1"))  
          switch_core_session_execute_application(session,"set","skip_greeting=true");
         switch_core_session_execute_application(session,"voicemail",tmp_str);
         switch_safe_free(tmp_str);
       // switch_safe_free(ann_pmt);
}


/*void bridge_call(switch_core_session_t *session,call_details_t *call,const char *dial_num){

    char *tmp_str = NULL;
    switch_channel_t * channel = switch_core_session_get_channel(session);
    const char *opsp_ip = switch_channel_get_variable(channel,"sip_network_ip");
    const char *opsp_port = switch_channel_get_variable(channel,"sip_network_port");

  //CALLER ID OVERRIDE PENDING should be handled as different function
   
   // if(call->cust.is_callerid_override_on && (!IS_NULL(call->caller.callerid_number))) {
     //  tmp_str = switch_mprintf("{originate_timeout=%d,origination_caller_id_name=%s}sofia/internal/%s@%s:%s",   call->caller.ring_timeout,call->caller.cid_name,dial_num,opsp_ip,opsp_port);
       //   } else {
         if(call->flags.is_frwd_outbound || call->flags.is_call_outbound||call->did.is_outbound_on){
        int ring_timeout = (call->flags.is_frwd_outbound == true)?call->callee.ring_timeout:call->caller.ring_timeout;
        tmp_str = switch_mprintf("{originate_timeout=%d}sofia/gateway/gw_%s/%s",ring_timeout,call->obd.gw_id,dial_num);
    // tmp_str = switch_mprintf("{originate_timeout=%d}sofia/internal/%s@%s:%s",call->caller.ring_timeout,dial_num,opsp_ip,opsp_port);

    }
    else{
       tmp_str = switch_mprintf("{originate_timeout=%d}sofia/internal/%s@%s:%s",call->caller.ring_timeout,dial_num,opsp_ip,opsp_port);
   
}
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n",tmp_str);
    switch_core_session_execute_application(session,"bridge",tmp_str);
    switch_safe_free(tmp_str);
}*/
void bridge_call(switch_core_session_t *session,call_details_t *call,const char *dial_num){
    char  blnce[20];
    char  crdt_lmt[20];
    char  billing_typ[20];
    char *tmp_str = NULL;
    char * call_hdr=NULL;
    switch_channel_t * channel = switch_core_session_get_channel(session);
    const char *opsp_ip = switch_channel_get_variable(channel,"sip_network_ip");
    const char *opsp_port = switch_channel_get_variable(channel,"sip_network_port");
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s",opsp_ip);
   
  /*CALLER ID OVERRIDE PENDING should be handled as different function
 *    *
 *       * if(call->cust.is_callerid_override_on && (!IS_NULL(call->caller.callerid_number))) {
 *               tmp_str = switch_mprintf("{originate_timeout=%d,origination_caller_id_name=%s}sofia/internal/%s@%s:%s",   call->caller.ring_timeout,call->caller.cid_name,dial_num,opsp_ip,opsp_port);
 *                   } else {
 * call->did.is_outbound_on
 *                       */
    if(call->flags.is_outbound_mnt||call->flags.is_outbound_grp_mnt){
          call->obd.call_hdr_typ=atoi(call->mnt.gw_clr_id_hdr);
          call->obd.gw_codec=call->mnt.gw_codec;
          call->obd.clr_id=call->mnt.gw_caller_id; 
          call->obd.gw_prepend_cid=call->mnt.gw_prepend_cid;
          call->obd.gw_id=call->mnt.gw_id;
          call->obd.dial_prefix=call->mnt.dial_prefix; 
          //call->obd.call_plan_rate_id=call->mnt.dlout_cp_id;  
     } 


    if(call->flags.is_frwd_outbound || call->flags.is_call_outbound ){
     int dial_timeout = (call->flags.is_frwd_outbound == true)?call->callee.dial_timeout:call->caller.dial_timeout;
                      sprintf(blnce,"%f",call->obd.blnce);
                      switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : \n,\n");
                      sprintf(crdt_lmt,"%f",call->obd.crdt_lmt);
                      sprintf(billing_typ,"%d",call->obd.billing_typ);
                     //switch_channel_set_variable(channel,"call_type","outbound");
                      switch_channel_set_variable(channel,"application","outbound");
                      switch_channel_set_variable(channel,"sell_rate",call->obd.sell_rate);
                      switch_channel_set_variable(channel,"selling_min_duration",call->obd.min_dur);
                      switch_channel_set_variable(channel,"selling_billing_block",call->obd.incr_dur);
                      switch_channel_set_variable(channel,"call_plan_id",call->obd.call_plan_id);
                      switch_channel_set_variable(channel,"buy_rate",call->obd.buy_rate);
                      switch_channel_set_variable(channel,"gateway_group_id",call->obd.gw_id);
                      switch_channel_set_variable(channel,"dial_prefix",call->obd.dial_prefix);
                      //switch_channel_set_variable(channel,"is_mnt_plan",call->obd.is_mnt);
                      //switch_channel_set_variable(channel,"talking_mnt",call->obd.talktime_mnt);
                      switch_channel_set_variable(channel,"talking_blance",blnce);
                      switch_channel_set_variable(channel,"credit_blance",crdt_lmt);
                      switch_channel_set_variable(channel,"account_type",billing_typ);
                      switch_channel_set_variable(channel,"call_plan_rate_id",call->obd.call_plan_rate_id);
     if(call->obd.call_hdr_typ==0){
          call_hdr="rpid";
         }
        else if(call->obd.call_hdr_typ==1){ 
          call_hdr="pid";
         }
         else{ 
           call_hdr="none";
          }
           
         if(dial_num[0] != '+'){
                dial_num = switch_mprintf("+%s",dial_num);
                }else{
                dial_num = switch_mprintf("%s",dial_num);
                }
        
 
      //tmp_str = switch_mprintf("{absolute_codec_string='%s',sip_cid_type=%s,outbound_caller_from_user=%s,origination_caller_id_name='%s',origination_caller_id_number=%s%s,leg_timeout=%d}sofia/gateway/gw_%s/%s",call->obd.gw_codec,call_hdr,call->obd.clr_id,call->obd.clr_id ,call->obd.gw_prepend_cid,call->obd.clr_id,dial_timeout ,call->obd.gw_id,dial_num);
//,call->obd.gw_prepend,*/
   
tmp_str = switch_mprintf("{absolute_codec_string='%s',sip_cid_type=%s,outbound_caller_from_user=%s,origination_caller_id_name='%s',origination_caller_id_number=%s%s,leg_timeout=%d}sofia/gateway/gw_%s/%s",call->obd.gw_codec,call_hdr,call->obd.clr_id,call->obd.clr_id ,call->obd.gw_prepend_cid,call->obd.clr_id,dial_timeout ,call->obd.gw_id,dial_num);

   //  tmp_str = switch_mprintf("{originate_timeout=%d}sofia/gateway/gw_%s/%s",ring_timeout,call->obd.gw_id,dial_num);

    }
    else{
         // switch_channel_set_variable(channel,"ann_pmt",call->callee.ann_pmt);
       tmp_str = switch_mprintf("{absolute_codec_string='%s',originate_timeout=%d}sofia/internal/%s@119.18.55.154:%s",call->caller.codec,call->caller.dial_timeout,dial_num,opsp_port);
   }

    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n,%s\n",tmp_str,call_hdr);
    switch_core_session_execute_application(session,"bridge",tmp_str);
    switch_safe_free(tmp_str);
}


static int call_frwd_callback(void *parg,int argc,char **argv,char **column_name){

    call_frwd_t *cfrwd = (call_frwd_t *)parg;
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Call frwd : callback \n");
    if(argc < 8)
        return -1;

    cfrwd[0].type = atoi(argv[0]);
    if(!IS_NULL(argv[1]) && strlen(argv[1])){
        strcpy(cfrwd[0].num,argv[1]);
    }
    cfrwd[1].type = atoi(argv[2]);
    if(!IS_NULL(argv[3]) && strlen(argv[3])){
        strcpy(cfrwd[1].num,argv[3]);
    }
    cfrwd[2].type = atoi(argv[4]);
    if(!IS_NULL(argv[5]) && strlen(argv[5])){
        strcpy(cfrwd[2].num,argv[5]);
    }
    cfrwd[3].type = atoi(argv[6]);
    if(!IS_NULL(argv[7]) && strlen(argv[7])){
        strcpy(cfrwd[3].num,argv[7]);
    }
    //cfrwd->is_init = true;

    return 0;
}

  void check_call_frwd(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call){
    char *query = NULL;
    if(!(call->callee.is_init)){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Callee not system user. No check for frwd\n");    
        return;
    }
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Call frwd check\n");    
    /*query = switch_mprintf("SELECT  `universal_type`, `universal_external`, \
            `busy_type`, `busy_external`, `noanswer_type`, \
            `noanswer_external`, `unavailable_type`, `unavailable_external` \
            FROM `pbx_call_forward` \
            WHERE `customer_id`=%s \
            AND `extension_id`=%d \
            AND `status`='1'",call->callee.cust_id,call->callee.id);*/
      query=switch_mprintf("SELECT universal_type, if ( (universal_type='2'),\
          (select concat(`universal_country_code`,`universal_external`)), \
          (select `universal_external`) ) as universal_external,busy_type,\
          if ( (busy_type='2'),(select concat(`busy_country_code`,`busy_external`)),\
          (select `busy_external`) ) as busy_external,noanswer_type, if ( (noanswer_type='2'),\
          (select concat(`noanswer_country_code`,`noanswer_external`)), (select `noanswer_external`) )\
          as noanswer_external,unavailable_type, if ( (unavailable_type='2'),\
          (select concat(unavailable_country_code,unavailable_external)), (select unavailable_external) )\
          as unavailable_external from pbx_call_forward WHERE customer_id=%s\
          and extension_id=%d and STATUS='1' ",call->callee.cust_id,call->callee.id);


    execute_sql_callback(dsn,mutex,query,call_frwd_callback,&call->frwd);
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Call frwd : %s\n",query);    
    switch_safe_free(query);
}




void forward_call(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,int i){
       

        switch_core_session_t* session = switch_channel_get_session(channel);
         

        if(i < 0){
            switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Forwarding enabled but No setup found\n");
             switch_core_session_execute_application(session,"sleep","2000");

	    //switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/call_rejected.wav",NULL);
	    switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);	
            return ;
        }
	if(call->frwd[i].type == 0){
            	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Forwarding enabled but No setup found\n");
	        switch_core_session_execute_application(session,"sleep","2000");
         
              //  switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/call_rejected.wav",NULL);
		//switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);	
		return;	
	}
	if(call->flags.is_frwded){
		
                      switch_core_session_execute_application(session,"sleep","2000");
                switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/call_forwarding.wav",NULL);
		switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);	
		return ;
	}
             if(call->callee.is_dnd == true){
		
                  switch_core_session_execute_application(session,"sleep","2000");
                switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/ivr-dnd_activated.wav", NULL);
                switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                return ;
        }
                switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/call_forwarding.wav",NULL);
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"%d",call->callee.is_dnd);

      // Handle Blacklisted number
               if( (call->callee.blacklist==true) && (is_black_listed(channel,dsn,mutex,call)) ){
                    switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session),SWITCH_LOG_ERROR,"Dialed extension/Number is blacklisted.\n");
		    switch_core_session_execute_application(session,"sleep","2000");
                    switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/call_blacklist.wav",NULL);
               
                    
                    switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                    return ;
                           }
                 if( (call->caller.blacklist==true) && (is_black_list_outgoing(channel,dsn,mutex,call)) ){
                    switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session),SWITCH_LOG_ERROR,"Dialed extension/Number is blacklisted.\n");
                    switch_core_session_execute_application(session,"sleep","2000");
                    switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/call_blacklist.wav",NULL);
              
                    switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                    return ;
                           }
                                                                                       
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"Type: %d\n",call->frwd[i].type);
	call->flags.is_frwded = true;
        switch(call->frwd[i].type){
            case 1:
                    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"forwarding to voicemail: %s\n",call->callee.num);
		   if(call->callee.is_vm_on){
                    voicemail(session,NULL,NULL,call->callee.num);
                    break;
			}
		    else{ 
                      switch_core_session_execute_application(session,"sleep","2000");
                      switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/forward_vm_blocked.wav",NULL);
			}
		      break;
            case 2:
                    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"forwarding to outbound: %s\n",call->frwd[i].num);
		    call->flags.is_frwd_outbound = true;
                    switch_channel_set_variable(channel,"set_frwd","1");
		   // call->caller.cust_id=call->did.cust_id; 
		    outbound(session,dsn,mutex,call,call->frwd[i].num);
		    if(call->obd.gw_id != 0){ // Add no gw found prompt
				
		           // call->caller.cust_id=call->did.cust_id; 
                	    bridge_call(session,call,call->frwd[i].num);
			
               	      switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
		    }
                    break;
            case 3:
                   switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"forwarding to extension: %s\n",call->frwd[i].num);
                    switch_channel_set_variable(channel,"set_frwd","1");
                    get_ext_details(channel,&call->callee,dsn,mutex,call->frwd[i].num);
		    if(call->callee.is_dnd == true){
                      switch_core_session_execute_application(session,"sleep","2000");
                      switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/ivr-dnd_activated.wav", NULL);
               	      switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                       return ;
                     }
 
                    bridge_call(session,call,call->frwd[i].num);
		     call->callee.is_vm_on = false;
     		    //switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                    break;
            default:
		    call->flags.is_frwded = false;
                    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Forwarding enabled but No setup found\n");
			
                      switch_core_session_execute_application(session,"sleep","2000");
                      switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/call_rejected.wav",NULL);
                    break;
        }


}


static int outbound_callback(void *parg,int argc,char **argv,char **column_name){
	outbound_detail_t* obd = (outbound_detail_t*) parg;
       	for(int i = 0 ; i<argc;i++)
            switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"exten : %d %s %s\n",i,column_name[i],argv[i]);
        
        obd->is_init       = true;
       	if(!IS_NULL(argv[0]) && strlen(argv[0])){
	   obd->bill_type=strdup(argv[0]);
	}
	obd->gw_grp_id = atoi(argv[1]);
	obd->is_lcr_on = atoi(argv[2]);
	obd->rate_card_id = atoi(argv[3]);
       if(!IS_NULL(argv[4]) && strlen(argv[4])){
           obd->call_plan_id = strdup(argv[4]);
         }
      	if(!IS_NULL(argv[5]) && strlen(argv[5])){
	obd->dial_prefix=strdup(argv[5]);
     	}
         if(!IS_NULL(argv[6]) && strlen(argv[6])){

	obd->buy_rate = strdup(argv[6]);
             }
	 if(!IS_NULL(argv[7]) && strlen(argv[7])){
	obd->sell_rate = strdup(argv[7]);
        }
	 if(!IS_NULL(argv[8]) && strlen(argv[8])){
	obd->min_dur = strdup(argv[8]);
	 }
	 if(!IS_NULL(argv[9]) && strlen(argv[9])){
	obd->incr_dur = strdup(argv[9]);
	 }
        if(!IS_NULL(argv[10]) && strlen(argv[10])){

	obd->gw_id   = strdup(argv[10]);
         }
	if(!IS_NULL(argv[11]) && strlen(argv[11])){
		obd->gw_prepend = strdup(argv[11]);
	}
	if(!IS_NULL(argv[12]) && strlen(argv[12])){
		obd->gw_prepend_cid = strdup(argv[12]);
	}
	if(!IS_NULL(argv[13]) && strlen(argv[13])){
		obd->gw_codec = strdup(argv[13]);
	}
	obd->is_init = strtol(argv[10],NULL,10);
	obd->call_hdr_typ=atoi(argv[14]);
           if(!IS_NULL(argv[15]) && strlen(argv[15])){
                obd->clr_id = strdup(argv[15]);
        } 
          obd->clr_id_hdr=atoi(argv[16]);
        /* if(!IS_NULL(argv[17]) && strlen(argv[17])){
                obd->clr_id_hdr_vlue = strdup(argv[17]);
        }*/
         if(!IS_NULL(argv[17]) && strlen(argv[17])){
                obd->clr_pfile = strdup(argv[17]);
        }
        
          obd->crdt_lmt=atof(argv[18]);
          obd->blnce=atof(argv[19]);
          obd->billing_typ=atoi(argv[20]);
          if(!IS_NULL(argv[21]) && strlen(argv[21])){
                obd->is_mnt = strdup(argv[21]);
        }
        if(!IS_NULL(argv[22]) && strlen(argv[22])){
                obd->talktime_mnt = strdup(argv[22]);
        }
        if(!IS_NULL(argv[23]) && strlen(argv[23])){
                obd->call_plan_rate_id = strdup(argv[23]);
        }





        return 0;
}

static int outbound_mnt_callback(void *parg,int argc,char **argv,char **column_name){
        mnt_detail_t* mnt = (mnt_detail_t*) parg;
        for(int i = 0 ; i<argc;i++)
            switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"exten : %d %s %s\n",i,column_name[i],argv[i]);

         mnt->is_init       = true;
        if(!IS_NULL(argv[0]) && strlen(argv[0])){
          mnt->pkg_blng_typ=strdup(argv[0]);
        }
        if(!IS_NULL(argv[1]) && strlen(argv[1])){
          mnt->dialout_id=strdup(argv[1]);
        }
       if(!IS_NULL(argv[2]) && strlen(argv[2])){
          mnt->dlout_cp_id=strdup(argv[2]);
        }
       if(!IS_NULL(argv[3]) && strlen(argv[3])){
          mnt->dial_prefix=strdup(argv[3]);
        }
       if(!IS_NULL(argv[4]) && strlen(argv[4])){
          mnt->gw_id=strdup(argv[4]);
        }
        if(!IS_NULL(argv[5]) && strlen(argv[5])){
          mnt->gw_prepend=strdup(argv[5]);
        }
        if(!IS_NULL(argv[6]) && strlen(argv[6])){
          mnt->gw_prepend_cid=strdup(argv[6]);
        }
        if(!IS_NULL(argv[7]) && strlen(argv[7])){
          mnt->gw_codec=strdup(argv[7]);
        }
       if(!IS_NULL(argv[8]) && strlen(argv[8])){
          mnt->gw_hdr_typ=strdup(argv[8]);
        }
       /*if(!IS_NULL(argv[9]) && strlen(argv[9])){
          mnt->gw_prepend_cid=strdup(argv[9]);
        } 
        if(!IS_NULL(argv[10]) && strlen(argv[10])){
          mnt->gw_caller_id=strdup(argv[10]);
        }*/ 
        if(!IS_NULL(argv[9]) && strlen(argv[9])){
          mnt->gw_caller_id=strdup(argv[9]);
        }
        /*if(!IS_NULL(argv[12]) && strlen(argv[12])){
          mnt->gw_clr_id_hdr=strdup(argv[12]);
        }*/
        if(!IS_NULL(argv[10]) && strlen(argv[10])){
          mnt->gw_clr_id_hdr=strdup(argv[10]);
        }
       if(!IS_NULL(argv[11]) && strlen(argv[11])){
           mnt->gw_id_prof=strdup(argv[11]);
         }
        if(!IS_NULL(argv[12]) && strlen(argv[12]))Fec{
        mnt->is_mnt_plan=strdup(argv[12]);
        }
         if(!IS_NULL(argv[13]) && strlen(argv[13])){
        mnt->remain_mnt= strdup(argv[13]);
             }
         if(!IS_NULL(argv[14]) && strlen(argv[14])){
           mnt->is_group= strdup(argv[14]);
        }
        if(!IS_NULL(argv[15]) && strlen(argv[15])){
          mnt->group_id = strdup(argv[15]);
         }
         if(!IS_NULL(argv[16]) && strlen(argv[16])){
           mnt->group_mnt = strdup(argv[16]);
         }

         return 0;
}
              
           
bool outbound(switch_core_session_t *session,char * dsn,switch_mutex_t *mutex,call_details_t *call,const char *num){
        switch_channel_t * channel = switch_core_session_get_channel(session);
        bool is_outbound_allowed = false;
        int type;
        float total_blance;
        char *  cust_id = NULL;
	char * currnt_call=NULL;
	char is_call[20]={'\0'};
        char *tmp_num = NULL;
        char *query = NULL;
        char *mnt_call = NULL;
        char total_blnc[20];
     currnt_call =switch_mprintf ("call  verify_concurrent_call(%s)",call->caller.cust_id);
     execute_sql2str(dsn,mutex,currnt_call,is_call,NELEMS(is_call));
     switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"currnt_call%s,%s",call->caller.cust_id,is_call);
      if(!atoi(is_call)){
	
        switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/cc_limit_exhaust.wav", NULL);
        switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);

               }
                if(call->flags.is_frwd_outbound == true){
                       switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %d\n",call->caller.is_outbound_allowed);

                         is_outbound_allowed = call->callee.is_outbound_allowed;
                        cust_id = call->callee.cust_id;

                       
                }
                 else if (call->did.is_outbound_on  == true){
                      
                               switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %d\n",call->caller.is_outbound_allowed);
    
                     is_outbound_allowed = call->did.is_outbound_on;
                        cust_id = call->did.cust_id;
                }
                else {
                              switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %d\n",call->caller.is_outbound_allowed);

                        is_outbound_allowed = call->caller.is_outbound_allowed;
                        cust_id = call->caller.cust_id;

                  }

          if(num[0] != '+'){
                tmp_num = switch_mprintf("+%s",num);
                }else{
                tmp_num = switch_mprintf("%s",num);
                }
          
              switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session),SWITCH_LOG_ERROR,"diled number %s",tmp_num+(strlen(tmp_num)-10));
 
	 if( (call->caller.blacklist==true) && (is_black_list_outgoing(channel,dsn,mutex,call) )){
                    switch_log_printf(SWITCH_CHANNEL_SESSION_LOG(session),SWITCH_LOG_ERROR,"Dialed extension/Number is blacklisted.\n");
                    switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/call_blacklist.wav", NULL);
                    switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                    return 0 ;
                           }  

        if(is_outbound_allowed){
                mnt_call=switch_mprintf("SELECT f.billing_type as package_billing_type, cpr.id as dialout_id,\
                         cpr.call_plan_id as dialout_call_plan_id, cpr.dial_prefix as dialout_prefix,  g.id\
                         as gateway_id, g.prependDigit_dialnumber as gateway_prependDigit_dialnumber,\
                         g.prependDigit__callerID as gateway_prependDigit_callerID, g.codec as gateway_codec,\
                         g.callerID_headertype as gateway_headertype, g.callerID as gateway_callerid, g.callerID_header as\
                         gateway_callerid_header, g.calling_profile as gateway_callerid_profile,  f.minute_plan as minute_plan_enable,\
                         cpr.talktime_minutes-cpr.used_minutes as dialout_remaining_minutes,  cpr.is_group, cpr.group_id,\
                         (CASE\
	                 WHEN cpr.is_group = '1'  THEN (SELECT minutes-used_minutes FROM pbx_call_rate_group AS cprg\
                         WHERE cprg.id=cpr.group_id  AND cprg.minutes >= 0 )\
                         ELSE -1\
                         END ) AS group_minutes\
                         FROM customer cust\
                         JOIN pbx_feature f\
                         JOIN map_customer_package mcp ON cust.id=mcp.customer_id AND mcp.customer_id=333 AND mcp.product_id=1\
                         JOIN pbx_call_plan cp ON cp.id =\
                        (CASE\
    	                 WHEN f.is_bundle_type = 1 THEN (SELECT call_plan from pbx_bundle_plan WHERE id=f.bundle_plan_id)\
                         ELSE 0\
                         END)\
                         JOIN pbx_call_plan_rates cpr ON cpr.call_plan_id =\
	                 CASE\
    	                 WHEN f.minute_plan = 1 AND f.is_bundle_type = 1 AND cpr.is_group = '1' AND cpr.call_plan_id =mcp.package_id\
                         THEN cpr.call_plan_id\
    	                 WHEN f.minute_plan = 1 AND f.is_bundle_type = 1 AND cpr.group_id = 0 AND\
                         cpr.talktime_minutes-cpr.used_minutes > 1 THEN (SELECT call_plan from pbx_bundle_plan\
                         WHERE id=f.bundle_plan_id)\
                         WHEN f.minute_plan = 1 AND f.is_roaming_type = 1 THEN (SELECT call_plan from pbx_bundle_plan\
                         WHERE id=f.roaming_plan_id)\
                         ELSE 0\
                         END\
	                 AND '+91' like concat(cpr.dial_prefix,'%%')\
                         AND (DATE_FORMAT(cpr.expiry_date,'%%Y-%%m-%%d')= '0000-00-00' OR\
                         DATE_FORMAT(cpr.expiry_date,'%%Y-%%m-%%d')>CURRENT_DATE())\
                         JOIN gateway g ON g.status='1' AND g.id=cpr.gateway_id\
                         JOIN package p ON p.id = mcp.package_id AND p.product_id=1 AND f.id = p.feature_id \
                         order by is_group desc\
                         " ); 
                      switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %s\n",mnt_call);
                      execute_sql_callback(dsn,mutex,mnt_call,outbound_mnt_callback,&call->mnt);
                      switch_safe_free(mnt_call); 
                      switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"group : %s\n",call->mnt.remain_mnt);
                      if(call->mnt.is_init==true && (atoi(call->mnt.group_mnt)>0 ) && (!strcmp(call->mnt.is_group,"1")) ){
                         call->flags.is_outbound_grp_mnt=true;
                         call->obd.talktime_mnt  =call->mnt.group_mnt;
                         switch_channel_set_variable(channel,"variable_is_group_mnt_plan",call->mnt.is_group);
                         switch_channel_set_variable(channel,"talking_group_mnt",call->mnt.group_mnt);
                         switch_channel_set_variable(channel,"call_plan_rate_group_mnt_id" , call->mnt.group_id);
                         switch_channel_set_variable(channel,"billing_type","2");
                               return 1;
                                }
                      else if(call->mnt.is_init==true && (atoi(call->mnt.remain_mnt)>0 )&& (!strcmp(call->mnt.is_mnt_plan,"1"))   ){
                                  call->obd.talktime_mnt  =call->mnt.remain_mnt;
                                  switch_channel_set_variable(channel,"is_mnt_plan",call->mnt.is_mnt_plan);
                                  switch_channel_set_variable(channel,"talking_mnt",call->mnt.remain_mnt);
                                  switch_channel_set_variable(channel,"call_plan_rate_mnt_id" ,call->mnt.dialout_id);
                                  switch_channel_set_variable(channel,"billing_type","1");
                                  call->flags.is_outbound_mnt=true; 
                                  switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"is_mnt_plan : %s\n",call->mnt.is_mnt_plan);
                                  return 1;
                                 }
                            
                       
                   else{ 
                     
                /* query=switch_mprintf("select a.billing_type,a.gateway_group_id,c.lc_type,d.id,d.call_plan_id,\
                      d.dial_prefix,d.buying_rate,d.selling_rate,d.selling_min_duration,d.selling_billing_block,\
                      e.id,e.prependDigit_dialnumber,e.prependDigit__callerID,e.codec,e.callerID_headertype,\
                      e.callerID ,e.callerID_header ,e.calling_profile,cust.credit_limit,cust.balance as charge,\
                      cust.billing_type,a.minute_plan,d.talktime_minutes-d.used_minutes,d.id from customer cust,\
                      pbx_feature a,map_customer_package b,pbx_call_plan c,pbx_call_plan_rates d,gateway e,package f\
                      where cust.id=b.customer_id and b.customer_id='%s' and '%s' like concat(d.dial_prefix,'%%')\
                      and e.status='1' and d.call_plan_id = a.call_plan_id and f.id = b.package_id and a.id = f.feature_id\
                      and f.product_id=1 and b.product_id=1 and e.id=d.gateway_id and c.id=a.call_plan_id \
                      and b.start_date <= CURDATE() and b.end_date>= CURDATE() group BY CASE\
                      when c.lc_type='1' THEN  (d.selling_rate)\
                      WHEN c.lc_type='0' THEN (d.buying_rate)\
                      END limit 1 ",cust_id, tmp_num);*/
                      
                      query= switch_mprintf(" SELECT f.billing_type, f.gateway_group_id, cp.lc_type, cpr.id, cpr.call_plan_id,\
                             cpr.dial_prefix, cpr.buying_rate, cpr.selling_rate, cpr.selling_min_duration,cpr.selling_billing_block,\
                             g.id, g.prependDigit_dialnumber, g.prependDigit__callerID, g.codec, g.callerID_headertype,\
                             g.callerID, g.callerID_header, g.calling_profile, cust.credit_limit, cust.balance AS charge,\
                             cust.billing_type, f.minute_plan, cpr.talktime_minutes-cpr.used_minutes AS available_min,cpr.area_code,\
                            (CASE\
                             when STRCMP(SUBSTRING('%s',1,5),cpr.area_code)=0 then '5'\
                             when STRCMP(SUBSTRING('%s',1,4),cpr.area_code)=0 then '4'\
                             when STRCMP(SUBSTRING('%s',1,3),cpr.area_code)=0 then '3'\
                             when STRCMP(SUBSTRING('%s',1,2),cpr.area_code)=0 then '2'\
                             when STRCMP(SUBSTRING('%s',1,1),cpr.area_code)=0 then '1'\
                             END) as len_area  FROM customer cust\
                             JOIN package p ON p.product_id=1 \
                             JOIN pbx_feature f ON f.id = p.feature_id\
                             JOIN map_customer_package mcp ON p.id = mcp.package_id\
                             AND mcp.product_id=1\
                             AND mcp.start_date <= CURDATE()\
	                     AND mcp.end_date>= CURDATE()\
	                     AND mcp.customer_id='%s'\
                             JOIN pbx_call_plan cp ON cp.id=f.call_plan_id\
                             JOIN pbx_call_plan_rates cpr ON '%s' like concat(cpr.dial_prefix,'%%')\
                             AND cpr.call_plan_id = f.call_plan_id\
                             JOIN gateway g ON g.status='1' AND g.id=cpr.gateway_id\
                             WHERE cust.id=mcp.customer_id\
                             ORDER BY  len_area DESC,\
                             CASE when cp.lc_type='1' THEN  cpr.selling_rate  END desc,\
                             case  WHEN cp.lc_type='0' THEN cpr.buying_rate  end ASC limit 1",tmp_num+3,
                              tmp_num+3,tmp_num+3,tmp_num+3,tmp_num+3,cust_id,tmp_num);
                      switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %s\n",query);
                      execute_sql_callback(dsn,mutex,query,outbound_callback,&call->obd);
                      switch_safe_free(query);
                      switch_channel_set_variable(channel,"talking_mnt","0");
                      switch_channel_set_variable(channel,"billing_type","3");
             /* lc_type= switch_mprintf ("select c.lc_type\
                from pbx_feature a,map_customer_package b,pbx_call_plan c,pbx_call_plan_rates d,gateway e,package f \
                where b.customer_id=%s and '%s' like concat(d.dial_prefix,'%%') and a.status='1' and b.status='1' and \
                c.status='1' and d.status='1' and e.status='1' and d.call_plan_id = a.call_plan_id and \
                f.id = b.package_id and a.id = f.feature_id and f.product_id=1 and b.product_id=1 and \
                e.id=d.gateway_id and c.id=a.call_plan_id and b.start_date <= CURDATE() and \
                b.end_date>= CURDATE() limit 1 ",cust_id,tmp_num);

               
                execute_sql2str(dsn,mutex,lc_type,result,NELEMS(result));
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %s\n",lc_type);

 
            if(!strcmp(result,"0" )){
              char *query = NULL;
               query = switch_mprintf("select a.billing_type,a.gateway_group_id,c.lc_type,d.id,d.call_plan_id,\
                d.dial_prefix,d.buying_rate,d.selling_rate,d.selling_min_duration,\
                d.selling_billing_block,e.id,e.prependDigit_dialnumber,e.prependDigit__callerID,e.codec,\
                e.callerID_headertype,e.callerID ,e.callerID_header ,e.calling_profile,cust.credit_limit,\
                cust. balance as charge,cust.billing_type,a.minute_plan,d.talktime_minutes-d.used_minutes,\
                d.id from customer cust, pbx_feature a,map_customer_package b,\
                pbx_call_plan c,pbx_call_plan_rates d,gateway e,package f  where cust.id=b.customer_id and \
                b.customer_id=%s and '%s' like concat(d.dial_prefix,'%%') and a.status='1' and b.status='1' and \
                c.status='1' and d.status='1' and e.status='1' and d.call_plan_id = a.call_plan_id and \
                f.id = b.package_id and a.id = f.feature_id and f.product_id=1 and b.product_id=1 and \
                e.id=d.gateway_id and c.id=a.call_plan_id and b.start_date <= CURDATE() and \
                b.end_date>= CURDATE()  group by (d.buying_rate) asc limit 1 ",cust_id, tmp_num);
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %s\n",query);
                execute_sql_callback(dsn,mutex,query,outbound_callback,&call->obd);
                switch_safe_free(query);
              }
            else if(!strcmp(result,"1" )){
               char *query = NULL;
               query = switch_mprintf("select a.billing_type,a.gateway_group_id,c.lc_type,d.id,d.call_plan_id,\
                d.dial_prefix,d.buying_rate,d.selling_rate,d.selling_min_duration,\
                d.selling_billing_block,e.id,e.prependDigit_dialnumber,e.prependDigit__callerID,e.codec,\
                e.callerID_headertype,e.callerID ,e.callerID_header ,e.calling_profile,cust.credit_limit,\
                cust.balance as charge,cust.billing_type,a.minute_plan,d.talktime_minutes-d.used_minutes,\
                d.id from customer cust, pbx_feature a,map_customer_package b,\
                pbx_call_plan c,pbx_call_plan_rates d,gateway e,package f  where cust.id=b.customer_id and \
                b.customer_id=%s and '%s' like concat(d.dial_prefix,'%%') and a.status='1' and b.status='1' and \
                c.status='1' and d.status='1' and e.status='1' and d.call_plan_id = a.call_plan_id and \
                f.id = b.package_id and a.id = f.feature_id and f.product_id=1 and b.product_id=1 and \
                e.id=d.gateway_id and c.id=a.call_plan_id and b.start_date <= CURDATE() and \
                b.end_date>= CURDATE() group by (d.selling_rate) asc limit 1  ",cust_id,tmp_num);
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %s\n",query);
                execute_sql_callback(dsn,mutex,query,outbound_callback,&call->obd);
                switch_safe_free(query);
              }*/
               
               switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %d\n",call->obd.is_init);
               if (!(call->obd.is_init) ){
		 switch_channel_answer  (channel);
  		 switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/dialout_rate_missing.wav", NULL);
                 switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"Outbound hangup:");
                 return 0;
                 }
             if(call->obd.billing_typ==1)
                type=0;
               else
                 type=1;
               
              total_blance=call->obd.blnce+call->obd.crdt_lmt*type;
              sprintf(total_blnc,"%f",total_blance);
              switch_channel_set_variable(channel,"total_balance",total_blnc);
              
               switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : %f,%f,%f\n",total_blance,call->obd.blnce,call->obd.crdt_lmt);
              if(total_blance<1){
                   switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/balance_exhaust.wav", NULL);
                   switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                   return 0;
                  }
           }
            }else{
                switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/balance_exhaust.wav", NULL);
                switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                return 0;
        }

  return 1; 
}



 static int did_init_callback(void *parg, int argc, char **argv, char **column_names)
{
  did_details_t *did = (did_details_t *)parg;
  char *tmp;
  if (argc < 20)
  {
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " DID CALLBACK ERROR : NO DATA %d\n", argc);
    return -1;
  }
  for (int i = 0; i < argc; i++)
    switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "DID %d %s %s\n", i, column_names[i], argv[i]);

// Here making assumption that if we get result from DB it will be all valid
//   // data type value no negative no string in numeric columns or null values.





  if (!IS_NULL(argv[0]) && strlen(argv[0]))
  {

    did->cust_id = strdup(argv[0]);
  }
  strncpy(did->num, argv[1], strlen(argv[1])); //did_no 
  if (!IS_NULL(argv[2]) && strlen(argv[2]))
  {
    did->bill_type = strdup(argv[2]);
  }
  did->fixrate = strtod(argv[3], &tmp); // add tmp check via macro / func if error abort
  if (!IS_NULL(argv[4]) && strlen(argv[4]))
  {
    did->conn_charge = strdup(argv[4]); // add tmp check via macro / func if error abort
  }

if (!IS_NULL(argv[5]) && strlen(argv[5]))
  {

    did->selling_rate = strdup(argv[5]); // add tmp check via macro / func if error abort
  }
  did->max_cc = atoi(argv[6]);
  did->type = atoi(argv[7]);
  did->is_blacklist_on = atoi(argv[8]);
  did->is_vm_on = atoi(argv[9]);
  did->is_recording_on = atoi(argv[10]);
  did->is_outbound_on = atoi(argv[11]);
  did->is_call_barging_on = atoi(argv[12]);
  did->actv_ftr_id = atoi(argv[13]);
  did->dst_id = atoi(argv[14]);
  did->tm_gp_id = atoi(argv[15]);
  did->id = atoi(argv[16]);
  did->crdt_lmt = atoi(argv[17]);
  did->blnce = atoi(argv[18]);
  did->billing_typ = atoi(argv[19]);
  did->is_init = true;
  
  

  return 0;
}




bool verify_did(switch_channel_t *channel, char *dsn, switch_mutex_t *mutex, did_details_t *did)
{
  char *sql = NULL;

  const char *callee = switch_channel_get_variable(channel, "sip_req_user");
  sql = switch_mprintf("SELECT \
            a.customer_id,a.did,a.billingtype,a.fixrate,\
            a.connection_charge,a.selling_rate,a.max_concurrent,a.did_type,\
            d.black_list,d.voicemail,d.recording,d.outbound_call,d.barging,\
            f.active_feature_id,f.destination_id ,\
            f.time_group_id ,a.id ,b.credit_limit, b.balance as charge,b.billing_type\
            FROM did a,customer b,map_customer_package c,pbx_feature d,package e,did_destination f\
            WHERE a.`status`='1' AND a.`activated` = '1' \
            AND b.id=a.customer_id \
            AND b.status='1'\
	        and b.id = c.customer_id \
            and c.start_date <= CURRENT_TIMESTAMP \
            and c.end_date >= CURRENT_TIMESTAMP \
            and c.status='1' \
            and c.product_id=1 \
            and d.id = e.feature_id \
            and d.status='1' \
            and e.id = c.package_id \
            and e.status='1'\
	    and f.did_id = a.id and f.customer_id = a.customer_id and a.reserved='1'\
	    and f.active_feature_id > 0\
	    AND  a.`did`=%s", callee + 2);

  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " DID CALLBACK ERROR : %s\n", sql);
  execute_sql_callback(dsn, mutex, sql, did_init_callback, did);

  switch_safe_free(sql);
  switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, " DID CALLBACK ERROR : %s\n", did->cust_id);
  return (did->cust_id != NULL) ? true : false;
}

           






static int conf_init_callback(void *parg,int argc,char **argv,char **column_names){
    conf_details_t *conf = (conf_details_t*) parg;
    if(argc < 7){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CONF CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"INTERNAL CONF %d %s %s\n",i,column_names[i],argv[i]);

    conf->is_init       = true;
    if(!IS_NULL(argv[0]) && strlen(argv[0]))
            conf->ext   = strdup(argv[0]);
   if(!IS_NULL(argv[1]) && strlen(argv[1]))
         conf->cust_id            = strdup(argv[1]);
    
    if(!IS_NULL(argv[2]) && strlen(argv[2]))
            conf->admin_pin = strdup(argv[2]);
    if(!IS_NULL(argv[3]) && strlen(argv[3]))
            conf->mem_pin = strdup(argv[3]);
  // conf->wc_prompt     = atoi(argv[4]);
    conf->is_recording  = atoi(argv[4]);
      if(!IS_NULL(argv[6]) && strlen(argv[5]))
            conf->wc_prompt   = strdup(argv[5]);

    return 0;
}

static int cg_init_callback(void *parg,int argc,char **argv,char **column_names){
    callgrp_details_t *cg = (callgrp_details_t*) parg;
    if(argc < 5){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CG CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"INTERNAL CG %d %s %s\n",i,column_names[i],argv[i]);

    cg->is_init         = true;
    if(!IS_NULL(argv[0]) && strlen(argv[0])){
     cg->id              = strdup(argv[0]);
     }
    cg->grp_type        = atoi(argv[1]);
    cg->is_recording_on = atoi(argv[2]);
    cg->ring_timeout    = atoi(argv[3]);
    if(!IS_NULL(argv[4]) && strlen(argv[4])){
        cg -> extensions  = strdup(argv[4]);
    }
    return 0;
}

static int fmfm_init_callback(void *parg,int argc,char **argv,char **column_names){
    fmfm_details_t *fmfm = (fmfm_details_t*) parg;
    if(argc < 5){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," fmfm CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"INTERNAL fmfm %d %s %s\n",i,column_names[i],argv[i]);

    fmfm->is_init         = true;
    fmfm->ring_timeout    = atoi(argv[0]);
    fmfm->id              = atoi(argv[1]);
    if(!IS_NULL(argv[2]) && strlen(argv[2])){
        fmfm ->fmfm1  = strdup(argv[2]);
    }
   if(!IS_NULL(argv[3]) && strlen(argv[3])){
        fmfm -> fmfm2  = strdup(argv[3]);
    }
   if(!IS_NULL(argv[4]) && strlen(argv[4])){
        fmfm -> fmfm3  = strdup(argv[4]);
    } 
    return 0;
}


void handle_fmfm(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call){
        switch_core_session_t* session = switch_channel_get_session(channel);
        //const char* opsp_ip = switch_channel_get_variable(channel,"sip_network_ip");
        //const char* opsp_port = switch_channel_get_variable(channel,"sip_network_port");
        char * query=NULL;
        const char* dialstatus;
       // char * tmp_str=NULL;
       // char *dial_num = NULL;
        //char *token,*rest,*new_str,*tmp;
        //token = rest = new_str = tmp = NULL;
        const char* ext_id = switch_channel_get_variable(channel,"ext_id");
        switch_channel_set_variable(channel,"call_type","fmfm");
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Bridge string:: CALL fmfm\n");
        query=switch_mprintf("SELECT ring_timeout,extension_id, IF((find_me_follow_me_type_1='PSTN'),(select phone_number1 from \
              pbx_contact_list where id=caller_id_1),(select concat(ext_number) from extension_master where id=caller_id_1)) AS \
              fmfm1, IF((find_me_follow_me_type_2='PSTN'),(select phone_number1 from pbx_contact_list where id=caller_id_2), \
              (select concat(ext_number) from extension_master where id=caller_id_2)) AS fmfm2, IF((find_me_follow_me_type_3='PSTN'), \
              (select phone_number1 from pbx_contact_list where id=caller_id_3),(select concat(ext_number) from extension_master \
              where id=caller_id_3)) AS fmfm3 FROM pbx_fmfm WHERE extension_id=%d",call->callee.id);                
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," fmfm SQL : %s\n",ext_id);
        execute_sql_callback(dsn,mutex,query,fmfm_init_callback,&call->fmfm);
        if(call->fmfm.is_init){
      
        if (call->fmfm.fmfm1[0]=='+'){
           switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," fmfm1 outbound SQL : %s\n",call->fmfm.fmfm1);
           call->flags.is_call_outbound="true"; 
	   outbound(session,dsn,mutex,call,call->fmfm.fmfm1);
           bridge_call(session,call,call->fmfm.fmfm1);
 
	 }
         else{
            switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," fmfm1 sip : %s\n",call->fmfm.fmfm1);
            bridge_call(session,call,call->fmfm.fmfm1);
             }
          call->flags.is_call_outbound="false";
          dialstatus =switch_channel_get_variable(channel,"DIALSTATUS");

         if( dialstatus!=NULL  &&!strcmp(dialstatus,"SUCCESS")  ){
                                 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," causefefefe: %s\n",dialstatus);
                                 switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                    }
 
       if (call->fmfm.fmfm2[0]=='+'){
           switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"fmfm2 outbounnd  : %s\n",call->fmfm.fmfm2);
           call->flags.is_call_outbound="true";
           outbound(session,dsn,mutex,call,call->fmfm.fmfm2);
           bridge_call(session,call,call->fmfm.fmfm2);

         }
         else{
            
            call->flags.is_call_outbound="false";
            switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," fmfm2 sip : %s\n",call->fmfm.fmfm2);
            bridge_call(session,call,call->fmfm.fmfm2);
             }
              dialstatus =switch_channel_get_variable(channel,"DIALSTATUS");

             if( dialstatus!=NULL  &&!strcmp(dialstatus,"SUCCESS")  ){
                                 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," causefefefe: %s\n",dialstatus);
                                 switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                         }
  
                 if (call->fmfm.fmfm3[0]=='+'){
           switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," fmfm3 outbound : %s\n",call->fmfm.fmfm3);
           call->flags.is_call_outbound="true";
           outbound(session,dsn,mutex,call,call->fmfm.fmfm3);
           bridge_call(session,call,call->fmfm.fmfm3);

         }
         else{

            call->flags.is_call_outbound="false";
            switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," fmfm3 sip : %s\n",call->fmfm.fmfm3);
            bridge_call(session,call,call->fmfm.fmfm3);
             }
              dialstatus =switch_channel_get_variable(channel,"DIALSTATUS");

             if( dialstatus!=NULL  &&!strcmp(dialstatus,"SUCCESS")  ){
                                 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," causefefefe: %s\n",dialstatus);
                                 switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                         }

       
  
          switch_safe_free(query);
 
        
        /*while( token != NULL){
                dial_num = switch_mprintf("[leg_timeout=%d]sofia/internal/%s@%s:%s",call->cg.ring_timeout,token,opsp_ip,opsp_port);
                if(tmp == NULL){
                        new_str = switch_mprintf("%s",dial_num);
                }else{
                         new_str = switch_mprintf("%s%s %s",tmp,sign,dial_num);
                }
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Bridge string:: %s\n",dial_num);
                switch_safe_free(dial_num);
                switch_safe_free(tmp);
                tmp = new_str;
                token = strtok_r(NULL,",", &rest);
        }
        if(call->cg.is_recording_on){
                set_recording(channel,"cg",call);
        }
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Bridge string:: %s \n",new_str);
        switch_core_session_execute_application(session,"bridge",new_str);
        switch_safe_free(new_str);*/
 }
}



 bool verify_internal_exten(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,const char *num){
   char *sql = NULL;
   bool res  = false;
   switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cust_id : %s,did_dest:%d\n",call->did.cust_id,call->did.dst_id);
   //New
	if(call->did.is_init && call->did.actv_ftr_id==5){//new
		sql = switch_mprintf("SELECT `id`, `group_type`, `recording`, `ringtimeout`, `sip` \
                   FROM pbx_callgroup \
                   WHERE id=%d AND customer_id=%s",call->did.dst_id,call->did.cust_id);

	}else{
   		sql = switch_mprintf("SELECT `id`, `group_type`, `recording`, `ringtimeout`, `sip` \
                   FROM pbx_callgroup \
                   WHERE group_ext=%s AND customer_id=%s",num+1,call->caller.cust_id);
	}
   switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CG SQL : %s\n",sql);
   execute_sql_callback(dsn,mutex,sql,cg_init_callback,&call->cg);
   switch_safe_free(sql);
   res = call->cg.is_init;

   switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CG SQL : %d\n",res);
   if(res == false){
	if(call->did.is_init){
	 	/*sql = switch_mprintf("SELECT `conf_ext`, `customer_id`, `admin_pin`,\
                       `participant_pin`, `welcome_prompt`, `moh`, `recording` \
                        FROM pbx_conference\
                        WHERE  conf_join_start_date <= CURRENT_TIMESTAMP and\
                        `conf_join_end_date` >= CURRENT_TIMESTAMP and status = '1'\
                        AND `id`=%d AND `customer_id`=%s",call->did.dst_id,call->did.cust_id);*/
                    sql  =switch_mprintf("SELECT  conf.conf_ext,conf.customer_id,conf.admin_pin,conf.participant_pin,\
                                            conf.moh,conf.recording,GROUP_CONCAT(DISTINCT (pro.file_path)) as file_path\
                                            FROM `pbx_conference` as conf LEFT JOIN `pbx_prompts` as\
                                             pro on ( conf.welcome_prompt = pro.id)  where conf.status ='1'\
                                             and conf.conf_join_start_date <= CURRENT_TIMESTAMP and conf.conf_join_end_date >=\
                                             CURRENT_TIMESTAMP\
                                             AND conf.id=%d AND conf.customer_id=%s  group by\
                                             conf.id",call->did.dst_id,call->did.cust_id);
	}else{
        	/*sql = switch_mprintf("SELECT `conf_ext`, `customer_id`, `admin_pin`,\
                       `participant_pin`, `welcome_prompt`, `moh`, `recording` \
                        FROM pbx_conference\
                        WHERE  conf_join_start_date <= CURRENT_TIMESTAMP and\
                        `conf_join_end_date` >= CURRENT_TIMESTAMP and status = '1'\
                        AND `conf_ext`=%s AND `customer_id`=%s",num,call->caller.cust_id);*/
                 sql  =switch_mprintf("SELECT  conf.conf_ext,conf.customer_id,conf.admin_pin,conf.participant_pin,\
                                            conf.moh,conf.recording,GROUP_CONCAT(DISTINCT (pro.file_path)) as file_path\
                                            FROM `pbx_conference` as conf LEFT JOIN `pbx_prompts` as\
                                             pro on ( conf.welcome_prompt = pro.id)  where conf.status ='1'\
                                             and conf.conf_join_start_date <= CURRENT_TIMESTAMP and conf.conf_join_end_date >=\
                                             CURRENT_TIMESTAMP\
                                             AND conf.conf_ext=%s AND conf.customer_id=%s group by conf.id",num+1,call->caller.cust_id);

	}
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CG SQL : %s\n",sql);
        execute_sql_callback(dsn,mutex,sql,conf_init_callback,&call->conf);
        switch_safe_free(sql);
        res = call->conf.is_init;
   }
   return res;
}

void eaves_drop(switch_core_session_t *session, const char * extension,call_details_t *      call_info,char * dsn,switch_mutex_t *mutex)
{
      char *tmp_str=NULL;
      char uuid[200] = {'\0'};
      switch_channel_t * channel = switch_core_session_get_channel(session);
      switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"callforwording statusf :%s\n",extension);

      switch_channel_answer  (channel);
      tmp_str=switch_mprintf("SELECT uuid FROM `pbx_realtime_cdr` where customer_id=%s and forward=%s\
         and current_status='CHANNEL_BRIDGE' and sip_current_application='call_queue' limit 1" ,call_info->caller.cust_id, extension);
      execute_sql2str(dsn,mutex,tmp_str,uuid,NELEMS(uuid));
      switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"dial %s  agent number not on any  call :\n",tmp_str);
      if(strcmp(uuid,""))
        switch_core_session_execute_application(session,"eavesdrop",uuid);
      else{
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"dial %s  agent number not on any  call :\n",extension);
        switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
       }
}
 

static int fd_init_callback(void *parg,int argc,char **argv,char **column_names){
    feture_details_t *fd = (feture_details_t*) parg;
    if(argc < 3){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," fd CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"INTERNAL fd %d %s %s\n",i,column_names[i],argv[i]);

    fd->is_init         = true;
    fd->bling_typ       = atoi(argv[0]);
    if(!IS_NULL(argv[1]) && strlen(argv[1])){
        fd -> blnce  = strdup(argv[1]);
    }
    if(!IS_NULL(argv[2]) && strlen(argv[2])){
        fd -> crdt_lmt  = strdup(argv[2]);
    }

    return 0;
}
 
 void feature_code(switch_core_session_t *session,const char *callee,const char *caller, call_details_t *      call_info,char * dsn,switch_mutex_t *mutex )   {   
      char* query=NULL ; 
	char* call_frwd=NULL;                                                                                                                   
      char result[128]={0};
      char *audio_file="/home/cloudconnect/pbx_new/upload/def_prompts/speech.wav";
      uint8_t min=7;
      uint8_t max=14;  
      char digit_buffer[128] ;  
      char *  ivr_pin ;
      char *tmp="/home/cloudconnect/pbx_new/upload/def_prompts/check_blnc.wav";
      char digit[128]="1" ;
      char * pin;
      char *chck_blnc;
      char * chck_invce;
      char * tmp_invce;
      char * tmp_blnce;
      char invce[20] = {'\0'};
      char status[20]={0};
      float crdt_lmt,blnce,total_blnce;
      int i; 
      bool ext=0;
    switch_channel_t * channel = switch_core_session_get_channel(session);
		
            switch_channel_answer(channel);
            switch(atoi(callee+1))
              {    
                case 12:
			 //switch_channel_set_variable(channel,"call_type","call_enable_dnd");
		         switch_channel_answer(channel);
			//switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/partita-no-3-in-e-major-bwv-1006-1-preludio.wav", NULL);
               if(call_info->caller.is_admin){
                  sleep(3); 
		 for(i=0;i<3;i++){
			 switch_play_and_get_digits(session,1 ,1 ,1,10000, "#",tmp,NULL , NULL, digit,sizeof(digit),"[0-9]+", 10000, NULL);
                         pin=strdup(digit);
		      
			switch(atoi(pin))
                         {
                          case 1:
                           ext=1;
                           chck_blnc=switch_mprintf("SELECT billing_type,balance,credit_limit FROM `customer` where id=%s",call_info->caller.cust_id);
                           switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," fd SQL : %s\n",chck_blnc);
                           execute_sql_callback(dsn,mutex,chck_blnc,fd_init_callback,&call_info->fd);
                           if(call_info->fd.bling_typ==2){
                               sscanf(call_info->fd.blnce, "%f", &blnce);
                               sscanf(call_info->fd.crdt_lmt, "%f", &crdt_lmt);
                               total_blnce=blnce+crdt_lmt;
                               tmp_blnce =switch_mprintf("en CURRENCY PRONOUNCED %f",total_blnce);
                               	       
                              }
                           else{
			       tmp_blnce =switch_mprintf("en CURRENCY PRONOUNCED %s",call_info->fd.blnce);
                               }
                             switch_core_session_execute_application(session,"say",tmp_blnce);
                             switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"pin: %s\n",pin);
                             switch_safe_free(chck_blnc);
                             //switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                              
                             break;
                          case 2:
			     ext=1;
                             chck_invce=switch_mprintf("SELECT amount_with_gst FROM `pbx_invoice` where customer_id=%s order by id desc limit 1",call_info->caller.cust_id);
                             execute_sql2str(dsn,mutex,chck_invce,invce,NELEMS(invce));
                             switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"invoice: %s\n",invce);
			     tmp_invce=switch_mprintf("en CURRENCY PRONOUNCED %s",invce );
                             switch_core_session_execute_application(session,"say",tmp_invce);
                             switch_safe_free(chck_invce); 
                             switch_safe_free(tmp_invce);                           
                             //switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);

                             break;
                          default:
                             switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/invalid_digit.wav", NULL);
                            // switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
 
                            break;
                          }
                          if(ext){
			      switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/thankyou_selfcare.wav", NULL);
				switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
				}

                        }
			 switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/thankyou_selfcare.wav", NULL);
                         switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                           
		       }else{
                        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE," No admin permission %d :\n",call_info->caller.is_admin);
			sleep(3);
			switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/not_authorized_to_dial.wav", NULL);
			switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                       
			 }
			break;

		case 33:
                     switch_channel_set_variable(channel,"call_type","call_evesdrop");
                     if(call_info->caller.is_admin){
                      ivr(session, min,   max, audio_file,  result,call_info, dsn,mutex) ;
                      switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"callforwording statusf :%s\n",result);
                        }
                     else{
                        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE," No admin permission %d :\n",call_info->caller.is_admin);
                    
			switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
			
                        }

                       break;
                  /*case 70:
                       
                       switch_channel_set_variable(channel,"call_type","call_enable_dnd");

                    if(call_info->caller.is_dnd == false){
                       	query=switch_mprintf(" update extension_master SET  dnd =1 where ext_number=%s",caller);
                        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",query);
                        execute_sql(dsn,query,mutex);   
                        switch_safe_free(query); 
                       }
			
                        switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                    break;                                                                                                                   
                 case 79:
			
                     switch_channel_set_variable(channel,"call_type","call_disable_dnd");
                     if(call_info->caller.is_dnd == true){
                      query=switch_mprintf(" update extension_master SET  dnd =0 where ext_number=%s",caller);
                      switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",query);
                       execute_sql(dsn,query,mutex);   
                       switch_safe_free(query); 
                     }
                     
                      
                        switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                    break;
                */

             case 70:
             		 if(call_info->caller.is_dnd == false){
                        query=switch_mprintf(" update extension_master SET  dnd =1 where ext_number=%s",caller);
                        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",query);
                        switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/dnd_activated.wav", NULL);
			 }else{
			 query=switch_mprintf(" update extension_master SET  dnd =0 where ext_number=%s",caller);
                         switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",query);
                         switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/dnd_deactivated.wav", NULL);
			}
			execute_sql(dsn,query,mutex);   
                        switch_safe_free(query);
                       break;

                /* case 72:
			 
                        switch_channel_set_variable(channel,"call_type","call_enable_forward");
                        query=switch_mprintf(" update pbx_call_forward SET  status ='1' where customer_id=%s and  extension_id=%d"
                        ,call_info->caller.cust_id,call_info->caller.id);
                    //  query=switch_mprintf(" update extension_master SET forward =1 where ext_number=%s",caller);
                       switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",query);
                       execute_sql(dsn,query,mutex);   
		       switch_safe_free(query); 
                   
		     
                    break;                                                                                                                   
                 case 73:
			 
		       
                       switch_channel_set_variable(channel,"call_type","call_disable_forward");

                       query=switch_mprintf(" update pbx_call_forward SET  status ='0' where customer_id=%s and  extension_id=%d"
                        ,call_info->caller.cust_id,call_info->caller.id);
                     //   query=switch_mprintf(" update extension_master SET forward =0 where ext_number=%s",caller);
                       switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",query);
                       execute_sql(dsn,query,mutex);
                       switch_safe_free(query);
                      break;
                  */
                  
                 case 72:
			
	               call_frwd = switch_mprintf("SELECT  status FROM `pbx_call_forward` \
                       WHERE `customer_id`=%s \
                       AND `extension_id`=%d" \
                        ,call_info->caller.cust_id,call_info->caller.id);
	
		       execute_sql2str(dsn,mutex,call_frwd,status,NELEMS(status));
 		       switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",status);
                        if(atoi(status)){
			switch_channel_set_variable(channel,"call_type","call_disable_forward");
			
                        query=switch_mprintf(" update pbx_call_forward SET  status ='0' where customer_id=%s and  extension_id=%d"
                        ,call_info->caller.cust_id,call_info->caller.id);
		       	
                       switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",status);
		      //execute_sql(dsn,query,mutex);
                       //switch_safe_free(query);
			 
		       execute_sql(dsn,query,mutex);
                       switch_safe_free(query);
		      switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/call_forward_deactivated.wav", NULL);
			}
			else if(atoi(status)==0 && strcmp(status,"")){
			
                         switch_channel_set_variable(channel,"call_type","call_enable_forward");
			query=switch_mprintf(" update pbx_call_forward SET  status ='1' where customer_id=%s and  extension_id=%d"
                        ,call_info->caller.cust_id,call_info->caller.id);
			
		     
			
		       execute_sql(dsn,query,mutex);
                       //switch_safe_free(query);
		      //switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/call_forward_deactivated.wav", NULL);
			switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",query);
			switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/call_forward_activated.wav", NULL);
			}
		       else{
			
		        sleep(1);
			switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/call_forward_setup_notfound.wav", NULL);
		   }
			 break;





                                                                                                                           
                case 75:
                      if(call_info->caller.is_vm_on == true){
                      voicemail(session, "check",NULL,caller);
                    }else
		    switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/voicemail_is_not_activated.wav", NULL);
                      break;
                case 76:
		    // switch_channel_set_variable(channel,"ann_pmt",call_info->caller.ann_pmt);
                     // if(call_info->caller.is_vm_on == true){
			 voicemail(session, "check",NULL,NULL);
                       // }else
			//switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/voicemail_is_not_activated.wav", NULL);

		     break;
                case 80:
                      park(session,callee); 
                      break;
                case 81:
                      park(session,callee);
                      break; 
	       	case 77:
                      //if(call_info->caller.is_vm_on == true){
                     //
                          // ivr(session, min,   max, audio_file,  result)     ;
                        switch_play_and_get_digits(session,7 ,10 , 3,15000, "#",audio_file,NULL , NULL, digit_buffer,
                                sizeof(digit_buffer),"[0-9]+", 2000, NULL);
                           ivr_pin = strdup(digit_buffer);
			   //sscanf(call_info->caller,"%s",atoi(ivr_pin));
                           get_ext_details(channel,&call_info->callee,dsn,mutex,ivr_pin);
			   if(strcmp(call_info->caller.cust_id,call_info->callee.cust_id)){
			       sleep(2);
                               switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/not_authorized_to_dial.wav", NULL);
 	
                                  break;
				}
			    if(call_info->callee.is_vm_on == true ){
  
 			

                           voicemail(session, NULL,NULL,ivr_pin);
                         }else
		            switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/voicemail_is_not_activated.wav",NULL);
                      break;   
                 default:
			sleep(2);
		      switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/invalid_digit.wav", NULL);
		      break;

              }
		
                   
              //switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
            }

void valetpark(switch_core_session_t *session,const char *callee)
{
    switch_channel_t * channel =switch_core_session_get_channel(session);
         if (atoi(callee+1)==5900)
              {
                   switch_channel_answer  (channel);
                   switch_core_session_execute_application(session,"valet_park","my_lot auto in 5901 5999");
             }

        else if(atoi(callee+1)>=5901 && atoi(callee+1)<=5999 )
              {
                  switch_channel_answer  (channel);
                  switch_core_session_execute_application(session,"valet_park","my_lot auto out 5901 5999");
              }

}

void park(switch_core_session_t *session,const char *callee)
{ 
    char * park;
    char * unpark;
    char * data;
    switch_channel_t * channel =switch_core_session_get_channel(session);
    data=switch_mprintf("slot_count=${fifo(count 80@$${domain})");    
    switch_core_session_execute_application(session,"set",data);
    switch_core_session_execute_application(session,"set","slot_count=${slot_count:-9:2}");
    /*switch_core_session_execute_application(session,"unset","fifo_chime_list");
    switch_core_session_execute_application(session,"set","fifo_chime_freq=0");
    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query:");*/
  if (atoi(callee+1)==80)
              {
                  switch_channel_answer  (channel);
		 /* switch_core_session_execute_application(session,"set","slot_count=${fifo(count 80@$${domain})");
                  switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query:");
                  switch_core_session_execute_application(session,"set","slot_count=${slot_count:-9:2}");*/
                  switch_core_session_execute_application(session,"unset","fifo_chime_list");
                  switch_core_session_execute_application(session,"set","fifo_chime_freq=0");
                  park=switch_mprintf("80@$${domain} in undef local_stream://moh");
		  switch_core_session_execute_application(session,"fifo",park);
             }

        else if(atoi(callee+1)>=81 && atoi(callee+1)<=99 )
              {
                  switch_channel_answer  (channel);
		  unpark=switch_mprintf("80@$${domain} out nowait");
		  switch_core_session_execute_application(session,"fifo",unpark);
              }

}





/*void  ivr(switch_core_session_t *session,uint8_t min,  uint8_t max, char* audio_file,const char * result)
     {
     int32_t max_tries = 3;
    uint32_t digit_timeout =2000;
    int timeout = 15000;
    char digit_buffer[128] ;
    const char *bad_input_audio_file =NULL;
    const char *var_name = NULL;
    const char *valid_terminators = "#";
    const char *digits_regex ="[0-9]+";
    const char *transfer_on_failure = NULL;
  
                      
    switch_play_and_get_digits(session, min, max, max_tries, timeout, valid_terminators,
                               audio_file, bad_input_audio_file, var_name, digit_buffer, sizeof(digit_buffer),
                                 digits_regex, digit_timeout, transfer_on_failure);
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"callforwording statusf :%s\n",digit_buffer);
       result =strdup(digit_buffer);
        if(strlen(result)==ndle
4)
          result=switch_mprintf("%s%s",call_info->caller.cust_id,result);
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"agent :%s\n",result);
        eaves_drop(session, result,call_info,dsn,mutex);  
 
     }
*/

void  ivr(switch_core_session_t *session,uint8_t min,  uint8_t max, char* audio_file,const char * result,call_details_t * call_info,char * dsn,switch_mutex_t *mutex){

    int32_t max_tries = 3;
    uint32_t digit_timeout =2000;
    int timeout = 15000;
    char digit_buffer[128] ;
    const char *bad_input_audio_file =NULL;
    const char *var_name = NULL;
    const char *valid_terminators = "#";
    const char *digits_regex ="[0-9]+";
    const char *transfer_on_failure = NULL;


         switch_play_and_get_digits(session, min, max, max_tries, timeout, valid_terminators,
                               audio_file, bad_input_audio_file, var_name, digit_buffer, sizeof(digit_buffer),
                                 digits_regex, digit_timeout, transfer_on_failure);
        result =strdup(digit_buffer);
        if(strlen(result)==4)
          result=switch_mprintf("%s%s",call_info->caller.cust_id,result);
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"agent :%s\n",result);
        eaves_drop(session, result,call_info,dsn,mutex);

     }

/*
 * Function to handle inbound did call.
 * Transfer call to mapped destination.
 * */

void handle_did_dest(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call){
        switch_core_session_t* session = switch_channel_get_session(channel);
        const char* caller = switch_channel_get_variable(channel,"sip_from_user");
        char * prompt;
	char * time;
        char * invld_pmt;
	struct stack *pt = newStack(5);
        char result[20] = {'\0'};
        char dst_id[20] = {'\0'};
        char pr_rslt[200]={'\0'};
	char * did_currnt_call=NULL;
        char did_call[20]={'\0'};
        int type;
        int  total_blance;
	switch_channel_set_variable(channel,"application","inbound");
        switch_core_session_execute_application(session,"answer",NULL);
	sprintf(dst_id,"%d",call->did.dst_id);

	did_currnt_call =switch_mprintf ("call  verify_did_max_call(%d)",call->did.id);
	execute_sql2str(dsn,mutex,did_currnt_call,did_call,NELEMS(did_call));
         
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"CALL1 : %s,%d",did_call,call->did.id);               
        if(!atoi(did_call)){
        switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/cc_limit_exhaust.wav", NULL);
        switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                return ;
               }
	if(atoi(call->did.bill_type)!= 4 ||(atoi(call->did.bill_type)== 4 && atoi(call->did.conn_charge)!=0)){
	
           if(call->did.billing_typ==1)
                type=0;
               else
                 type=1;
               total_blance=call->did.blnce+call->did.crdt_lmt*type;
              if(total_blance<=0){
                   switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/balance_exhaust.wav", NULL);
                   switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                   return;

                  }
             }
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"CALL1 : %s,%s",did_call,did_currnt_call);               
	switch_channel_set_variable(channel,"did_id",dst_id );
        

                 time=switch_mprintf ("call  verify_time_group(%d,%s)",call->did.tm_gp_id,call->did.cust_id);
                 execute_sql2str(dsn,mutex,time,result,NELEMS(result));
                 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"CALL : %s,%s",time,result);               
                 if(atoi(result)!=1){ 
                  prompt =switch_mprintf ("select p.file_path  from pbx_prompts p join  time_group t on(t.prompt_id=p.id) where t.id=%d",call->did.tm_gp_id);
                  execute_sql2str(dsn,mutex,prompt,pr_rslt,NELEMS(pr_rslt));
                  invld_pmt = switch_mprintf("/var/www/html/pbx/app/%s",pr_rslt);
                  switch_ivr_play_file(session, NULL,invld_pmt ,NULL);
                  switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                  switch_safe_free(prompt);
                  switch_safe_free(invld_pmt);
		  return;
              }
                  
                 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"active : %d",call->did.actv_ftr_id);               

                     
	switch(call->did.actv_ftr_id){
 		
               
		case 1: //SIP
			call->flags.is_inbound_sip = true;
			switch_channel_set_variable_printf(channel,"ext_id","%s",dst_id);
              	        sscanf(caller,"%s",call->caller.num);	
		        call->caller.cust_id=call->did.cust_id;
			 

                         handle_sip_call(channel,dsn,mutex,call);
			break;
		case 2: 
			
		      switch_channel_set_variable(channel,"call_type","call_ivr");
                      handle_ivr(channel, dsn,mutex,call,pt,1);

                        break;
	
		case 3: //conf
                        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"active conference : %d",call->did.actv_ftr_id);               
			if(verify_internal_exten(channel,dsn,mutex,call,dst_id)){
				if(call->conf.is_init){
					handle_conf(channel,call);
				}
			}
			break;
		case 4: 
                        //handle_ivr(dsn,mutex,call);

			handle_queue(channel,dsn,mutex,call,1);	
			break;
		case 5:
			if(verify_internal_exten(channel,dsn,mutex,call,dst_id)){
				if(call->cg.is_init){
					
              	        		sscanf(caller,"%s",call->caller.num);	
					call->caller.cust_id=call->did.cust_id;
					handle_cg(channel,call,dsn,mutex);
				}
			}

			break;
               case 10:
                        handle_queue(channel,dsn,mutex,call,2);
                        break;
		case 12:
			
 			handle_play_bck(channel,dsn,mutex,dst_id);
			break;
                case 13:
                       handle_appointment(channel,dsn,mutex,call);
                       //verify_apmt_slots(channel,&call->apmt_slots,dsn,mutex);
                       // verify_apmt(channel,&call->apmt,dsn,mutex);
                        

                        break;                                         

		default:
			break;
	}
	return;
}



//appointment method

 static int apmt_init_callback(void *parg,int argc,char **argv,char **column_names)
{
    apmt_details_t *apmt = (apmt_details_t*) parg;


     if(argc < 15){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," apmt  CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
       for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO," appointment %d %s %s\n",i,column_names[i],argv[i]);
           
            if(!IS_NULL(argv[0]) && strlen(argv[0])){
          
               apmt->apmt_id            = strdup(argv[0]);
               }
            if(!IS_NULL(argv[1]) && strlen(argv[1])){
          
               apmt->apmt_name            = strdup(argv[1]);
               }
            if(!IS_NULL(argv[2]) && strlen(argv[2])){
          
               apmt->wc_prompt         = strdup(argv[2]);
               }
            if(!IS_NULL(argv[3]) && strlen(argv[3])){
          
               apmt->invalid_prompt          = strdup(argv[3]);
               }
            if(!IS_NULL(argv[4]) && strlen(argv[4])){
          
               apmt->tm_out_prompt            = strdup(argv[4]);
               }
            if(!IS_NULL(argv[5]) && strlen(argv[5])){
          
               apmt->wc_pmt_path           = strdup(argv[5]);
               }
            if(!IS_NULL(argv[6]) && strlen(argv[6])){
          
               apmt->invalid_pmt_path           = strdup(argv[6]);
               }
            if(!IS_NULL(argv[7]) && strlen(argv[7])){
          
               apmt->tm_out_prompt           = strdup(argv[7]);
               }
            if(!IS_NULL(argv[8]) && strlen(argv[8])){
          
               apmt->phone_no1           = strdup(argv[8]);
               }
            if(!IS_NULL(argv[9]) && strlen(argv[9])){
          
               apmt->phone_no2          = strdup(argv[9]);
               }
            if(!IS_NULL(argv[10]) && strlen(argv[10])){
          
             apmt->grp_name            = strdup(argv[10]);
               }
            if(!IS_NULL(argv[11]) && strlen(argv[11])){
          
                 apmt->name         = strdup(argv[11]);
               }
            
             apmt->contact_id      = atoi(argv[12]);
             apmt->grp_contact_id  = atoi(argv[13]);
             apmt->ext_no    = atoi(argv[14]);



       
                   
return 0;

}


/*
static int apmt_slots_init_callback(void *parg,int argc,char **argv,char **column_names)
{
    apmt_slots_details_t *apmt_slots= (apmt_slots_details_t*) parg;


     if(argc < 6){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," apmtslots  CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
       for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO," appointment slots %d %s %s\n",i,column_names[i],argv[i]);

             apmt_slots->id      = atoi(argv[0]);
             apmt_slots->did  =    atoi(argv[1]);
             apmt_slots->time_slot    = atoi(argv[2]);
             apmt_slots->time_interval  = atoi(argv[3]);
            if(!IS_NULL(argv[4]) && strlen(argv[4])){
          
             apmt_slots->apmtid        = strdup(argv[4]);
               }
            if(!IS_NULL(argv[5]) && strlen(argv[5])){
          
             apmt_slots->src        = strdup(argv[5]);
               }



return 0;
}
*/


/*
void  verify_apmt(switch_channel_t *channel,apmt_details_t *apmt,char * dsn,switch_mutex_t *mutex){        
        
 
        char * sql = NULL;
    
                    
        
                
                sql = switch_mprintf("SELECT pa.id AS apmt_id, pa.name AS apmt_name, pa.welcome_prompt, pa.invalid_prompt, pa.timeout_prompt,\
           GROUP_CONCAT(DISTINCt pro.file_path) as welcome_pmt, GROUP_CONCAT(DISTINCt pro1.file_path) as invalid_pmt,\
           GROUP_CONCAT(DISTINCt pro2.file_path) as timmeout_pmt, group_concat(DISTINCT pcl.phone_number1) AS phone_no1,\
           Group_concat(DISTINCT pcl.phone_number2)AS phone_no2, GROUP_CONCAT(DISTINCT pcg.name) AS Group_Name,\
           pcg.id AS Contact_ID, pcgm.id AS Group_Contact_ID, pcl.name ,em.ext_number  FROM pbx_appointment pa LEFT JOIN pbx_prompts pro on\
           (pa.welcome_prompt = pro.id)LEFT join pbx_prompts pro1 on (pa.invalid_prompt = pro1.id) LEFT JOIN pbx_prompts pro2 on (pa.timeout_prompt = pro2.id)\
           LEFT JOIN pbx_contact_group as pcg ON (pa.group_ids = pcg.id) LEFT JOIN pbx_contact_group_mapping AS pcgm ON (pcgm.contact_group_id = pcg.id)\
           LEFT JOIN pbx_contact_list AS pcl ON pcl.id = (pcgm.contact_id ) LEFT JOIN pbx_appointment_mapping AS pam ON (pa.id = pam.appointment_id)\
           LEFT JOIN extension_master AS em ON (pam.ref_id = em.id)  WHERE pa.id = '48'");
          
          
          execute_sql_callback(dsn,mutex,sql,apmt_init_callback,apmt);
          switch_safe_free(sql);

       
        
          
       // switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," AMPT apmt_id  : %d \n",apmt->apmt_id);
        
          switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE," apmt did id : %s\n",apmt->apmt_id);
        
    
}
*/


/*void  verify_apmt_slots(switch_channel_t *channel,apmt_slots_details_t *apmt_slots,char * dsn,switch_mutex_t *mutex){        
        
 
        char * sql = NULL;
               sql = switch_mprintf("     SELECT `id`, `app_id`, `did`, `src`, `time_slot`, `time_interwal` FROM `pbx_appointment_slots`");
               execute_sql_callback(dsn,mutex,sql,apmt_slots_init_callback,apmt_slots);
               switch_safe_free(sql);

}
*/
//appointment handle



  void  handle_appointment(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call){        
            char * sql = NULL;

            char * insert_str=NULL;
            switch_time_exp_t tm;
            char date[50] = "";
            switch_size_t retsize;
            switch_time_t ts;
            const char* caller = switch_channel_get_variable(channel,"sip_from_user");

               switch_core_session_t* session = switch_channel_get_session(channel);
               const char recording_path[] = "/home/cloudconnect/pbx_new/upload";

               ts = switch_time_now();
               switch_time_exp_lt(&tm, ts);
               switch_strftime(date,&retsize,sizeof(date),"%Y-%m-%d-%T",&tm);

	 
            
            //char src[20]={0};
            //char *call_frwd=NULL;
           

                    
        
                
                sql = switch_mprintf("SELECT pa.id AS apmt_id, pa.name AS apmt_name, pa.welcome_prompt, pa.invalid_prompt, pa.timeout_prompt,\
           GROUP_CONCAT(DISTINCt pro.file_path) as welcome_pmt, GROUP_CONCAT(DISTINCt pro1.file_path) as invalid_pmt,\
           GROUP_CONCAT(DISTINCt pro2.file_path) as timmeout_pmt, group_concat(DISTINCT pcl.phone_number1) AS phone_no1,\
           Group_concat(DISTINCT pcl.phone_number2)AS phone_no2, GROUP_CONCAT(DISTINCT pcg.name) AS Group_Name,\
           pcg.id AS Contact_ID, pcgm.id AS Group_Contact_ID, pcl.name ,em.ext_number  FROM pbx_appointment pa LEFT JOIN pbx_prompts pro on\
           (pa.welcome_prompt = pro.id)LEFT join pbx_prompts pro1 on (pa.invalid_prompt = pro1.id) LEFT JOIN pbx_prompts pro2 on (pa.timeout_prompt = pro2.id)\
           LEFT JOIN pbx_contact_group as pcg ON (pa.group_ids = pcg.id) LEFT JOIN pbx_contact_group_mapping AS pcgm ON (pcgm.contact_group_id = pcg.id)\
           LEFT JOIN pbx_contact_list AS pcl ON pcl.id = (pcgm.contact_id ) LEFT JOIN pbx_appointment_mapping AS pam ON (pa.id = pam.appointment_id)\
           LEFT JOIN extension_master AS em ON (pam.ref_id = em.id)  WHERE pa.id = '48'");
          
          
          execute_sql_callback(dsn,mutex,sql,apmt_init_callback,&call->apmt);
          switch_safe_free(sql);

       
        
          
       // switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," AMPT apmt_id  : %d \n",apmt->apmt_id);
        
          switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE," apmt did id : %s\n",call->apmt.apmt_id);
          switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO," apmt did number : %s\n",call->did.num); 
          
          /*  call_frwd = switch_mprintf("SELECT `time_slot` FROM `pbx_appointment_slots`\
           WHERE `did` = 1171366697 AND `app_id` = 48" );

           execute_sql2str(dsn,mutex,call_frwd,src,NELEMS(src));
           switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Exten query: %s\n",src); */
          
             
         
             
               switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"RECORD DETAILS: DATE:%s Caller:%s\n",date,caller);    
           
               switch_core_session_execute_application(session,"set","RECORD_STEREO=true");
               switch_core_session_execute_application(session,"set","media_bug_answer_req=true");
               
               insert_str=switch_mprintf("INSERT INTO `cc_master`.`pbx_appointment_slots` (`app_id`, `did`, `src`,\
                          `time_slot`, `time_interwal`) VALUES\
                           (%s, '%s', '%s',' %s', '%s')\
                           ",call->apmt.apmt_id,call->did.num,caller,date,date);
               execute_sql(dsn,insert_str,mutex); 
               switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"insert_string : %s",insert_str);
              
               switch_channel_set_variable(channel,"set_recording","1");
               switch_channel_set_variable(channel,"recording_path",recording_path);
               switch_channel_set_variable(channel,"rec_cust_id",call->caller.cust_id);
          
               switch_channel_set_variable(channel,"rec_num",call->caller.num);
                switch_channel_set_variable(channel,"rec_caller",caller);
               switch_channel_set_variable(channel,"rec_date",date);
              
               switch_safe_free(insert_str);
           
               

         

       
      
    
}


static int tf_init_callback(void *parg,int argc,char **argv,char **column_names){
    tc_failover *tf = (tc_failover*) parg;
    if(argc <2){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," tc CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"INTERNAL tc %d %s %s\n",i,column_names[i],argv[i]);
         tf->is_init         = true;
         tf->actve_ftre      = atoi(argv[0]);
         tf->actve_val       = atoi(argv[1]);

 
      return 0;
  }


void handle_tc_failover(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,char* tc_id){
        switch_core_session_t* session = switch_channel_get_session(channel);
        char *query=NULL;
        char * vm=NULL;
        const char* caller = switch_channel_get_variable(channel,"sip_from_user");
        struct stack *pt = newStack(5);
        char dst_id[20]={'\0'};
        char result[20]={'\0'};
        switch_channel_set_variable(channel,"application","inbound");
        switch_core_session_execute_application(session,"answer",NULL);
        switch_channel_set_variable(channel,"did_id",dst_id );
        query=switch_mprintf("SELECT active_feature,active_feature_value FROM `pbx_tc` where id=%s",tc_id);   
	execute_sql_callback(dsn,mutex,query,tf_init_callback,&call->tf);
        call->did.dst_id=call->tf.actve_val;
        sprintf(dst_id,"%d",call->tf.actve_val);
        if(call->tf.actve_ftre!=0 && call->tf.actve_val!=0 ){
              switch(call->tf.actve_ftre){
	        case 1: //SIP
                        call->flags.is_inbound_sip = true;
                        switch_channel_set_variable_printf(channel,"ext_id","%s",dst_id);
                        sscanf(caller,"%s",call->caller.num);
                        call->caller.cust_id=call->did.cust_id;
	                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n%s\n%s\n%d",call->caller.cust_id,dst_id,caller,call->tf.actve_val);

                         handle_sip_call(channel,dsn,mutex,call);
                        break;
                 case 2:

                      switch_channel_set_variable(channel,"call_type","call_ivr");
                      handle_ivr(channel, dsn,mutex,call,pt,1);

                        break;

                 case 3:
                        if(verify_internal_exten(channel,dsn,mutex,call,dst_id)){
                                if(call->conf.is_init){
                                        handle_conf(channel,call);
                                }
                        }
                        break;
                  case 4:
                       handle_queue(channel,dsn,mutex,call,1);
                        break;
                  case 5:
                        if(verify_internal_exten(channel,dsn,mutex,call,dst_id)){
                                if(call->cg.is_init){

                                        sscanf(caller,"%s",call->caller.num);
                                        call->caller.cust_id=call->did.cust_id;
                                        handle_cg(channel,call, dsn,mutex);
                                }
                        }

                        break;
                   case 6:
                         vm= switch_mprintf ("select ext_number from  extension_master where  id =%s",dst_id);

                          execute_sql2str(dsn,mutex,vm,result,NELEMS(result));

                if(get_ext_details(channel,&call->caller,dsn,mutex,result)){
                  if(call->caller.is_vm_on){
                    voicemail(session,NULL,NULL,result);
                        }
                    else{
                      switch_core_session_execute_application(session,"sleep","2000");
                      switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/forward_vm_blocked.wav",NULL);
                        }
                  switch_safe_free(vm);
                        }
                  break;
                    
                   case 10:
                        handle_queue(channel,dsn,mutex,call,2);
                        break;
                  case 12:

                        handle_play_bck(channel,dsn,mutex,dst_id);
                        break;
  
                  default:
                        break;
        }
       }
      
        return;
        }                               
                  
static int cq_init_callback(void *parg,int argc,char **argv,char **column_names){
    queue_details_t *cq = (queue_details_t*) parg;
    if(argc < 8){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CG CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"Call QUEUE: %d %s %s\n",i,column_names[i],argv[i]);

	cq->is_init         = true;
     if(!IS_NULL(argv[0]) && strlen(argv[0])){
        strcpy(cq -> q_name,argv[0]);
    }
    cq->max_wait_call   = atoi(argv[1]);
    //cq->welcome_pmt     = atoi(argv[2]);
    cq->p_anc		= atoi(argv[2]);
    cq->p_anc_time      = atoi(argv[3]);
   // cq->p_anc_pmt       = atoi(argv[5]);
    cq->play_pos_on_call = atoi(argv[4]);
    cq->play_pos_prdcly  = atoi(argv[5]);
     if(!IS_NULL(argv[6]) && strlen(argv[6])){
          cq->welcome_pmt =strdup(argv[6]);
              }
       if(!IS_NULL(argv[7]) && strlen(argv[7])){
          cq->ann_pmt =strdup(argv[7]);
              }
           cq->unauth_usr=atoi(argv[8]);
	 

    return 0;
}

static int tc_init_callback(void *parg,int argc,char **argv,char **column_names){
    tc_details_t *tc = (tc_details_t*) parg;
    if(argc <6){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," tc CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"INTERNAL tc %d %s %s\n",i,column_names[i],argv[i]);

    tc->is_init         = true;
    if(!IS_NULL(argv[0]) && strlen(argv[0])){
     tc->id              = strdup(argv[0]);
     }
  if(!IS_NULL(argv[1]) && strlen(argv[1])){
     tc->tc_id              = strdup(argv[1]);
     }

    if(!IS_NULL(argv[2]) && strlen(argv[2])){
      tc->cntct1        = strdup(argv[2]);
     }
    if(!IS_NULL(argv[3]) && strlen(argv[3])){
       tc->cntct2 =strdup(argv[3]);
       }
  
       tc->mnt = atoi(argv[4]);
       if(!IS_NULL(argv[5]) && strlen(argv[5])){
          tc->mnt_id =strdup(argv[5]);
       }
       //tc->unauth_usr=atoi(argv[6]);


    return 0;
}
static int sta_init_callback(void *parg,int argc,char **argv,char **column_names){
    sta_details_t *sta = (sta_details_t*) parg;
    if(argc <3){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," STA  CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"INTERNAL STA %d %s %s\n",i,column_names[i],argv[i]);

    sta->is_init = true;
   
    if(!IS_NULL(argv[0]) && strlen(argv[0])){
     sta->id             = strdup(argv[0]);
     }
 
  if(!IS_NULL(argv[1]) && strlen(argv[1])){
     sta->agnt              = strdup(argv[1]);
     }
     sta->rcd  =atoi(argv[2]);
     sta->sat  =atoi(argv[3]);

    return 0;
}

void handle_play_bck(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex, char* id){

switch_core_session_t* session = switch_channel_get_session(channel);
	char* tmp=NULL;
	char* sql=NULL;
	char result[1000] = {'\0'};

	sql=switch_mprintf("select file_path from pbx_prompts where id=%s",id );
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n",sql);

	execute_sql2str(dsn,mutex,sql,result,NELEMS(result));
 	tmp = switch_mprintf("/var/www/html/pbx/app%s",result);
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n",result);
	switch_ivr_play_file(session,NULL,tmp,NULL);
	switch_safe_free(sql);
	switch_safe_free(tmp);
	return;



}
/*void handle_test(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex){
     switch_core_session_t* session = switch_channel_get_session(channel);
     char *audio_file="/usr/src/bhati/freeswitch-1.6.20/src/mod/applications/mod_cc_pbx/speech.wav";
     char digit_buffer[128] ;
     char *  ivr_pin ;
     switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string :\n");
	
     switch_play_and_get_digits(session,7 ,10 , 3,15000, "#",audio_file,NULL , NULL, digit_buffer,
                                sizeof(digit_buffer),"[0-9]+", 2000, NULL);
     ivr_pin = strdup(digit_buffer);
     voicemail(session, NULL,NULL,ivr_pin);

}*/
int  handle_stcky_agnt(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call){

        
	  switch_core_session_t* session = switch_channel_get_session(channel);
	const char* caller = switch_channel_get_variable(channel,"sip_from_user");
        
	const char* opsp_port = switch_channel_get_variable(channel,"sip_network_port");
	const char* dialstatus;
	bool init=1;
	char *contact=NULL;
	char *tmp_str;
	char *pin;
        char *tmp="/home/cloudconnect/pbx_new/upload/def_prompts/adv_sticky_agent_with_vm.wav";	
        char * bs_ivr="/home/cloudconnect/pbx_new/upload/def_prompts/basic_sticky_agent.wav";
	 char digit_buffer[128]="1" ;
       	char sticky_buffer[12];
     
        contact=switch_mprintf ("SELECT que.id,fd.forward,que.recording ,que.sticky_agent_type FROM `pbx_feedback` as fd, `pbx_queue` as que where fd.ref_id=que.id and fd.ref_id=%d and fd.customer_id=%s and que.agent like concat('%%',fd.forward,'%%') and fd.src=%s and que.sticky_agent=1    ORDER BY fd.hangup_time desc limit 1 ",call->did.dst_id,call->did.cust_id,caller);
       
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n",contact);
	execute_sql_callback(dsn,mutex,contact,sta_init_callback,&call->sta);
        
        if(!IS_NULL(call->sta.agnt)){
	switch_play_and_get_digits(session,1,1,1,15000,"#",bs_ivr,NULL,NULL,sticky_buffer,sizeof(sticky_buffer),"[0-9]+",2000,NULL);
                               switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n",sticky_buffer); 
        if(!strcmp(sticky_buffer,"1")){
       
		for(int i=0;i<3;i++){
			switch_channel_set_variable(channel,"call_type","call_sticky_agent");
			switch_channel_set_variable(channel,"ref_id",call->sta.id);
		

			tmp_str = switch_mprintf("{originate_timeout=40}sofia/internal/%s@119.18.55.154:%s",call->sta.agnt,opsp_port);
			switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n",tmp_str);
			switch_core_session_execute_application(session,"bridge",tmp_str);
   			switch_safe_free(tmp_str);
			if(call->sta.rcd){
                          call->caller.cust_id=strdup(call->did.cust_id);
                          strcpy(call->caller.num,caller); 
                          
			  switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n",tmp_str);
                          switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_strin");
                           set_recording(channel,"call_queue",call,dsn,mutex);
                         }
			
			dialstatus =switch_channel_get_variable(channel,"DIALSTATUS");
			switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n",dialstatus);
			/*if(call->sta.sat==1){
		       	if(  (dialstatus!=NULL) && !strcmp(dialstatus,"BUSY")  ){
             			switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cause1: %d\n",!strcmp(dialstatus,"BUSY") );
             			switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/ivr-call_rejected.wav", NULL);
				sleep(10);
				continue;
        		}else if( dialstatus!=NULL  && !strcmp(dialstatus,"NOANSWER") ){
                 		switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/ivr-no_user_response.wav", NULL);
                               	sleep(10);
				continue;
                               }	
			     }*/
			 if(call->sta.sat==2){
				if((dialstatus!=NULL) &&( !strcmp(dialstatus,"BUSY")||!strcmp(dialstatus,"NOANSWER") ) ){
                                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cause1: %d\n",call->sta.sat );
                                switch_channel_answer(channel);
				get_ext_details(channel,&call->caller,dsn,mutex,call->sta.agnt);
                                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %d\n",call->caller.is_vm_on);
                                if(call->caller.is_vm_on){

				switch_play_and_get_digits(session,1 ,1 ,1,10000, "#",tmp,NULL , NULL, digit_buffer,sizeof(digit_buffer),"[0-9]+", 10000, NULL);
				pin=strdup(digit_buffer);
				switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_INFO,"Dial_string : %s\n",digit_buffer);
				if(init){
				if(!strcmp(pin,"1")){
					voicemail(session,NULL,NULL,call->sta.agnt);
					 switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
					return 1;
						}
				 else{
				   init=0;
					}
					}
				 }          
                
                               
                        }
			}
			 if(  (dialstatus!=NULL) && !strcmp(dialstatus,"BUSY")  ){
                                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cause1: %d\n",!strcmp(dialstatus,"BUSY") );
                                //switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/call_rejected.wav", NULL);
                                //sleep(10);
                                break;
                        }else if( dialstatus!=NULL  && !strcmp(dialstatus,"NOANSWER") ){
                                //switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/ivr-no_user_response.wav", NULL);
                                sleep(10);
                                continue;
                               }

				
        		 if( dialstatus!=NULL  &&!strcmp(dialstatus,"UNALLOCATED_NUMBER")  ){
                   		 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cause: %s\n",dialstatus);
                     		// switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/ivr-unallocated_number.wav", NULL);
				 break;
				}
			else if( dialstatus!=NULL  &&!strcmp(dialstatus,"SUCCESS")  ){
                                 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," causefefefe: %s\n",dialstatus);
                                 switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
				
		
      			       return 1;
                                 
                                }

  			 }
		       } 
		    }
		return 0;
	      }           

void handle_queue(switch_channel_t *channel,char * dsn,switch_mutex_t *mutex,call_details_t *call,int i){
	
        char *wel_mpt=NULL;
	char *sql = NULL;
	char *tmp = NULL;
        char * tc=NULL;
        char tc_mnt[20];

	switch_core_session_t* session = switch_channel_get_session(channel);
	switch_stream_handle_t stream = {0};
        const char* src = switch_channel_get_variable(channel,"sip_from_user");
	
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"QUEUE DETAILS : %d",call->did.dst_id);
        /*sql = switch_mprintf("SELECT name,max_waiting_call,periodic_announcement,periodic_announcement_time,play_position_on_call,\
          play_position_periodically,  pro.file_path as file_path FROM `pbx_queue` as queue LEFT JOIN `pbx_prompts`\
          as pro on (queue.welcome_prompt = pro.id ) \
          where queue.status ='1' and queue.id =%d and queue.customer_id=%s GROUP BY queue.id",call->did.dst_id,call->did.cust_id)*/
        if(i==1){
		if(handle_stcky_agnt(channel,dsn,mutex,call)){
			return ;
		}
		
		
        sql = switch_mprintf("SELECT queue.id,max_waiting_call,periodic_announcement,periodic_announcement_time,play_position_on_call,\
          play_position_periodically, GROUP_CONCAT( distinct(pro.file_path)) as file_path,GROUP_CONCAT(distinct(pro1.file_path)) as periodic_prompti,queue.status FROM `pbx_queue` as queue LEFT JOIN `pbx_prompts`\
          as pro on (queue.welcome_prompt = pro.id )left join pbx_prompts as pro1 on (queue.periodic_announcement_prompt=pro1.id  )  \
          where queue.status ='1' and queue.id =%d and queue.customer_id=%s GROUP BY queue.id",call->did.dst_id,call->did.cust_id);
          switch_channel_set_variable(channel,"call_type","call_queue");

        }
      else if(i==2){
    sql = switch_mprintf("SELECT concat('tc_',queue.id)as id ,max_waiting_call,periodic_announcement,periodic_announcement_time,play_position_on_call,\
          play_position_periodically, GROUP_CONCAT( distinct(pro.file_path)) as file_path,GROUP_CONCAT(distinct(pro1.file_path))\
           as periodic_prompt ,queue.unauthorized_fail FROM `pbx_tc` as queue LEFT JOIN `pbx_prompts`\
           as pro on (queue.welcome_prompt = pro.id )left join pbx_prompts as pro1 on (queue.periodic_announcement_prompt=pro1.id  )  \
          where  queue.id =%d and queue.customer_id=%s GROUP BY queue.id",call->did.dst_id,call->did.cust_id);

         }
    if (i==2){
      tc=switch_mprintf(" select  concat('tc_', tc.id) as id,tc.id ,GROUP_CONCAT(cnt_map.phone_number1) as contact1,GROUP_CONCAT(cnt_map.phone_number2) as  \
                  contact2, (tpm.minutes-tpm.used_minute)as minute,tpm.id  from pbx_tc as tc  LEFT JOIN `pbx_tc_mapping` as tc_map  on (tc.id =tc_map.tc_id) LEFT JOIN \
                    `pbx_contact_list` as cnt_map   on (tc_map.ref_id=cnt_map.id) LEFT JOIN `extension_master` as ext_map   on (tc_map.ref_id=ext_map.id) \
                    left join pbx_tc_plan_mapping as tpm on(tpm.user_id=cnt_map.id) where  (cnt_map.phone_number1 like concat('%%','%s','%%') or   \
                      cnt_map.phone_number2 like concat('%%','%s','%%'))  and  ( tc_map.type='U' ) and  tc.id=%d  and tc.customer_id=%s  GROUP BY tc.id",
                 src,src,call->did.dst_id ,call->did.cust_id);
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," tc SQL : %s\n",tc);
        execute_sql_callback(dsn,mutex,tc,tc_init_callback,&call->tc);
        switch_safe_free(tc);
        switch_channel_set_variable(channel,"call_type","call_tc");
        sprintf(tc_mnt,"%d",call->tc.mnt);
        switch_channel_set_variable(channel,"tc_mnt",tc_mnt);
        switch_channel_set_variable(channel,"tc_mnt_id",call->tc.mnt_id);
        switch_channel_set_variable(channel,"ref_id",call->tc.tc_id);

       }
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ SQL : %d\n",call->tc.mnt);
        execute_sql_callback(dsn,mutex,sql,cq_init_callback,&call->cq);
        switch_safe_free(sql);
        switch_channel_set_variable(channel,"ref_id",call->cq.q_name);

        //switch_channel_set_variable(channel,"call_type","call_queue");
	switch_channel_answer(channel);
       if(i==1){//i=1 for queue 
        if(call->cq.is_init == false ){
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"CALL1");//queue is not activated now
                switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/invalid_queue.wav", NULL);
                switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                return ;
           }
        }

       if(i==2){ //i=2 for tc
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ SQL : %d\n",call->tc.mnt);
	if(call->tc.is_init == false ){
		if(!call->cq.unauth_usr){
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"CALL1");  //tc is not activated now             
 		switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/tc_unauthorize.wav", NULL);
		}
		else{
                   switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/tc_unauthorize_failover.wav", NULL);
		   handle_tc_failover(channel, dsn,mutex,call,"268");
                   switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                   return; 
                    }
                 switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                return ;	
	}
         else if(call->tc.mnt<=0 ){
               switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"CALL2");
                switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/balance_exhaust.wav", NULL);
                switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                return ;
          }
		
	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," tc SQL : %d\n",call->tc.mnt);
	}
	SWITCH_STANDARD_STREAM(stream);
        tmp = switch_mprintf("queue count members %s",call->cq.q_name);
//         tmp = switch_mprintf("queue count members %s",call->cq.q_name);

        switch_api_execute("callcenter_config",tmp,NULL,&stream);
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ SQL : %d\n",atoi((char *)stream.data));
        switch_safe_free(tmp);

        if(atoi((char *)stream.data) >= call->cq.max_wait_call ){
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," Call QUEUE %s full.\n",call->cq.q_name);
                switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
                return ;
        }
        switch_safe_free(stream.data);
         wel_mpt = switch_mprintf("/var/www/html/pbx/app%s",call->cq.welcome_pmt);
     
      switch_ivr_play_file(session, NULL,wel_mpt,NULL);

        if(call->cq.p_anc  ){
                char *uuid = switch_channel_get_uuid(channel);
              tmp = switch_mprintf("result=${luarun(/usr/local/freeswitch/scripts/callcenter-anouncement-position.lua %s %s@default %d %s %d)}",uuid,call->cq.q_name,(call->cq.p_anc_time * 1000),(call->cq.ann_pmt),call->cq.play_pos_on_call);
               //tmp = switch_mprintf("result=${luarun(/usr/local/freeswitch/scripts/callcenter-anouncement-position.lua %s %s@default %d )}",uuid,call->cq.q_name,(call->cq.p_anc_time * 1000));

                 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"CALL : %d %s.\n",call->cq.p_anc_time,tmp);
                switch_core_session_execute_application(session,"set",tmp);
                switch_safe_free(tmp);
        }

        tmp = switch_mprintf("%s@default",call->cq.q_name);
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ NAME : %s\n",tmp);
        switch_core_session_execute_application(session,"callcenter",tmp);
        switch_safe_free(tmp);

	return;
}

    
static int cv_init_callback(void *parg,int argc,char **argv,char **column_names){
            ivr_details_t  *cv = (ivr_details_t *) parg;
    if(argc < 11){
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CG CALLBACK ERROR : NO DATA %d\n",argc);
        return -1;
    }
    for(int i = 0 ; i<argc;i++)
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"Call ivr: %d %s %s\n",i,column_names[i],argv[i]);

        cv->is_init         = true;
     if(!IS_NULL(argv[0]) && strlen(argv[0])){
        strcpy(cv -> ivr_name,argv[0]);
       }
      cv ->timeout=atoi(argv[1]);
      cv ->dgt_timeout=atoi(argv[2]);
      cv ->mx_timeout=atoi(argv[3]);
      cv ->invld_count=atoi(argv[4]);
      cv ->drct_ext_call=atoi(argv[5]);
     if(!IS_NULL(argv[6]) && strlen(argv[6])){
        strcpy(cv -> ivr_dgt ,argv[6]);
    }
   if(!IS_NULL(argv[7]) && strlen(argv[7])){
       strcpy( cv -> ivr_prm,  argv[7]);
    }
    if(!IS_NULL(argv[8]) && strlen(argv[8])){
        cv -> welcome_pmt  = strdup(argv[8]);
    }
    if(!IS_NULL(argv[9]) && strlen(argv[9])){
        cv ->repeat_pmt  = strdup(argv[9]);
    }
    if(!IS_NULL(argv[10]) && strlen(argv[10])){
        cv ->invalid_pmt  = strdup(argv[10]);
    }
    if(!IS_NULL(argv[11]) && strlen(argv[11])){
        cv -> timeout_pmt  = strdup(argv[11]);
    }
    cv ->is_extn_dtmf=atoi(argv[12]);

    return 0;
    }

void handle_ivr(switch_channel_t *channel ,char * dsn,switch_mutex_t *mutex,call_details_t *call,struct stack *pt,int ivr){
   char * tmp=NULL;
   char *ask_extn="/home/cloudconnect/pbx_new/upload/def_prompts/extn_provide.wav";
   char *wrng_extn="/home/cloudconnect/pbx_new/upload/def_prompts/wrng_extn.wav";
   char * contact=NULL;
   char *sql = NULL;
   char * vm=NULL;
   struct node *head = NULL;
   struct node *a=NULL; 
   //struct stack *pt = newStack(5);
  
   const char* dialstatus;
   char * ivr_pin;  
   int i,k,n,num,list;
   char ** tokens=NULL;
   char **tok=NULL;
   char digit_buffer[128] = "";
   
   char **tokl=NULL;
   const char *number=NULL;
   const char *callee=NULL; 
   char result[20] = {'\0'};
   switch_core_session_t* session = switch_channel_get_session(channel);
   const char* caller = switch_channel_get_variable(channel,"sip_from_user");
       // switch_channel_set_variable(channel,"call_type","call_ivr");
    // switch_play_and_get_digits(session,1 ,1 , 1,5000, "#","/usr/src/bhati/freeswitch-1.6.20/src/mod/applications/mod_cc_pbx/speech.wav",NULL , NULL, digit_buffer,
      //                           sizeof(digit_buffer),"[0-9]+",5000, NULL);

      //switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"ivr DETAILS : %s",digit_buffer);
      sql = switch_mprintf("select pbx_mstr.name,pbx_mstr.timeout,pbx_mstr.digit_timeout,pbx_mstr.max_timeout,pbx_mstr.invalid_count,\
            pbx_mstr.direct_ext_call , GROUP_CONCAT(pbx_dtl.ivr_digit) as ivr_digit  ,GROUP_CONCAT(pbx_dtl.ivr_param)as ivr_param,\
            GROUP_CONCAT(distinct(pro.file_path)) as welcome_prompt ,GROUP_CONCAT(distinct(pro1.file_path)) as repeat_prompt ,\
            GROUP_CONCAT(distinct(pro2.file_path))  as invalid_sound ,GROUP_CONCAT(distinct(pro3.file_path))  as timeout_prompt ,\
	    pbx_mstr.is_direct_ext_dial from pbx_ivr_detail as pbx_dtl left join  pbx_ivr_master as pbx_mstr \
	    on (pbx_dtl.ivr_id=pbx_mstr.id)  left join pbx_prompts as pro on(pbx_mstr.welcome_prompt=pro.id  )\
	    left join pbx_prompts as pro1 on (pbx_mstr.repeat_prompt=pro1.id  ) left join pbx_prompts as pro2\
            on (pbx_mstr.invalid_sound=pro2.id)  left join pbx_prompts as pro3 on (pbx_mstr.timeout_prompt=pro3.id)  where pbx_mstr.id=%d",call->did.dst_id);
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ SQL : %s\n",sql);
        execute_sql_callback(dsn,mutex,sql,cv_init_callback,&call->cv);
        switch_safe_free(sql);
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ SQL : %zu\n", (strlen(call->cv.ivr_dgt)+1)/2);
        list=((strlen(call->cv.ivr_dgt)+1)/2) ;
        tokens = split(call->cv.ivr_dgt , &num, ",");
        tok = split( call->cv.ivr_prm, &n, ",");
        for(k=0;k<list;k++)
         {    
        addLast(&head,tokens[k],tok[k]);
         }  
         printList(head);
	if((strcmp(call->cv.welcome_pmt,call->cv.repeat_pmt))&& ivr==1){
           tmp = switch_mprintf("/var/www/html/pbx/app/%s",  call->cv.welcome_pmt);    
           switch_ivr_play_file(session, NULL, tmp, NULL);
           switch_safe_free(tmp);
        }
     
       for(i=0;i<= call->cv.invld_count;i++)
         {

          if(call->cv.is_extn_dtmf){
           //switch_channel_answer(channel);
           tmp = switch_mprintf("/var/www/html/pbx/app%s!%s",call->cv.repeat_pmt,ask_extn);
	   switch_channel_set_variable(channel,"playback_delimiter","!");

           switch_play_and_get_digits(session,1,7 , 1,(call->cv.timeout)*1000, "#",tmp,NULL , NULL, digit_buffer,
                                 sizeof(digit_buffer),"[0-9]+", (call->cv.dgt_timeout)*1000, NULL);
           
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"%s",tmp);
               if(strlen(digit_buffer)>3){
		 if(strlen(digit_buffer)==4){
                  	 callee=switch_mprintf("%s%s",call->did.cust_id,digit_buffer);
			}
		else{
                   callee=switch_mprintf("%s",digit_buffer);
			}
                   switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"CALL TYPE :%s\n",callee);
                   switch_channel_set_variable(channel,"call_type","sip_extn");
                   switch_channel_set_variable(channel,"cust_id",call->did.cust_id);
		   switch_channel_set_variable(channel,"sip_req_user",callee);
                 
             if(!get_ext_details(channel,&call->callee,dsn,mutex,callee)){
		 switch_ivr_play_file(session, NULL,wrng_extn, NULL);
 
		continue;
             	}
		else{
		     	
		        call->caller.cust_id=call->did.cust_id;
			handle_sip_call(channel,dsn,mutex,call);
			 switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
               		break;
			}
	
            }
	   }
	   else {
		 tmp = switch_mprintf("/var/www/html/pbx/app%s",call->cv.repeat_pmt);
		  switch_play_and_get_digits(session,1,1 , 1,(call->cv.timeout)*1000, "#",tmp,NULL , NULL, digit_buffer,
                                 sizeof(digit_buffer),"[0-9]+", (call->cv.dgt_timeout)*1000, NULL);
		}
           switch_safe_free(tmp);
           ivr_pin =strdup(digit_buffer);
	    
           if(!strcmp(ivr_pin,""))
             { 
		switch_channel_set_variable(channel,"call_dtmf","no_input"); 
                switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CidwdwQ : %s\n",ivr_pin);
                if(strcmp(call->cv.invalid_pmt,""))
		{
              	 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ NAME : %s\n",call->cv.timeout_pmt);
              	 tmp = switch_mprintf("/var/www/html/pbx/app/%s",  call->cv.timeout_pmt);
            	 }
             	 else
             	 {
                 tmp= switch_mprintf("/home/cloudconnect/pbx_new/upload/def_prompts/no_input.wav");
              	 }
              switch_ivr_play_file(session,NULL,tmp, NULL);
              switch_safe_free(tmp);
                continue ;
             }     
           a=search(head,ivr_pin );  
	 if(IS_NULL(a)){
		switch_channel_set_variable(channel,"call_dtmf","invalid_input");
              if(strcmp(call->cv.invalid_pmt,"")){
            	 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ NAME : %s\n",call->cv.invalid_pmt);
              	 tmp = switch_mprintf("/var/www/html/pbx/app/%s",  call->cv.invalid_pmt);
             }
              else
             {
            	 tmp= switch_mprintf("/home/cloudconnect/pbx_new/upload/def_prompts/invalid_digit.wav");
               }
             switch_ivr_play_file(session,NULL,tmp, NULL);
              switch_safe_free(tmp);

             continue ;
           }
           switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ _SQL : %s,%s\n",a->data,a->key);
           tokl = split( a->key, &n, "_");
           switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ _SQL : %s\n",tokl[0]);
           switch_channel_set_variable(channel,"call_dtmf",ivr_pin);
	   switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," pin_SQL : %s\n",ivr_pin);
          /*if(call->cv.is_extn_dtmf){
          	switch_channel_answer(channel);
                switch_play_and_get_digits(session,1 ,1 , 1,(call->cv.timeout)*1000, "#",ask_extn,NULL , NULL, digit_buffer,
                                 sizeof(digit_buffer),"[0-9]+", (call->cv.dgt_timeout)*1000, NULL);
		switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR,"%s",digit_buffer);
		if(strlen(digit_buffer)==4){
                   callee=switch_mprintf("%s%s",call->did.cust_id,digit_buffer);
                   switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"CALL TYPE :%s\n",callee);
                   switch_channel_set_variable(channel,"call_type","sip_extn");
                   switch_channel_set_variable(channel,"cust_id",call->did.cust_id);
		
		 }
	       }	

                if(!handle_sip_call(channel,dsn,mutex,call_info))
                        status = SWITCH_STATUS_FALSE;
                 
   		}*/
          //if((strcmp(tokl[0],""))||(strcmp(tokl[1],"")))
            //     switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);
         if(!strcmp(tokl[0],"conf")){
			switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cnof1:\n");
                   call->did.dst_id=atoi(tokl[1]);
                  if(verify_internal_exten(channel,dsn,mutex,call,tokl[1])){
			switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cnof2:\n");

                           if(call->conf.is_init){
                                        handle_conf(channel,call);
                                }
                    //    }
	          //else if(call->conf.is_init){

		//	switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cnof2:\n");
                  //      handle_conf(channel,call);
           
				}
                       break;
                            }
        else if(!strcmp(tokl[0],"queue")){
                   call->did.dst_id=atoi(tokl[1]);     
                   handle_queue(channel,dsn,mutex,call,1);
                 break;
                }
        else if(!strcmp(tokl[0],"cg")){
                 call->did.dst_id=atoi(tokl[1]);
                 if(verify_internal_exten(channel,dsn,mutex,call,tokl[1])){
                     if(call->cg.is_init){
					call->caller.cust_id=call->did.cust_id;
					sscanf(caller,"%s",call->caller.num);

                                        handle_cg(channel,call, dsn,mutex);
                                }
                        }
                  break;
                   }
        else if(!strcmp(tokl[0],"sip")){
                     call->flags.is_inbound_sip = true;

                     switch_channel_set_variable_printf(channel,"ext_id","%s",tokl[1]);
                       
                     if(get_ext_details(channel,&call->caller,dsn,mutex,tokl[1])){;
		     bridge_call(session,call,call->caller.num);
			
                    dialstatus =switch_channel_get_variable(channel,"DIALSTATUS");
			 switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ _SQL %s:",dialstatus);
		     handle_prompt(channel,dialstatus);
			}
                     //handle_sip_call(channel,dsn,mutex,call);
                       break;
                       }
         else if(!strcmp(tokl[0],"custom")){
              if(call->did.is_outbound_on ==true){
                    call->flags.is_call_outbound=true; 
		    call->caller.cust_id=call->did.cust_id;
                    number=tokl[2]; 
                    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ _SQL : %s\n",number);           
                    outbound(session,dsn,mutex,call,number);
                    bridge_call(session,call,number);
                    
                  }
                     break;         
                  }
         else if(!strcmp(tokl[0],"vm")){

                  vm= switch_mprintf ("select ext_number from  extension_master where  id =%s",tokl[1]);
                  execute_sql2str(dsn,mutex,vm,result,NELEMS(result));
                 
                if(get_ext_details(channel,&call->caller,dsn,mutex,result)){
		  if(call->caller.is_vm_on){
                    voicemail(session,NULL,NULL,result);
	        	}
		    else{
                      switch_core_session_execute_application(session,"sleep","2000");
                      switch_ivr_play_file(session,NULL,"/home/cloudconnect/pbx_new/upload/def_prompts/forward_vm_blocked.wav",NULL);
                        }
                  switch_safe_free(vm);
	        	}
 	          break;
                    }
        else if(!strcmp(tokl[0],"contact")){
		switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : ");
             if(call->did.is_outbound_on ==true){ 
		  call->caller.cust_id=call->did.cust_id; 
                  call->flags.is_call_outbound=true;
	          switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"Outbound : ");
                  contact=switch_mprintf ("select phone_number1 from pbx_contact_list where id=%s",tokl[2]);
                  execute_sql2str(dsn,mutex,contact,result,NELEMS(result));
                  outbound(session,dsn,mutex,call,result);
                  bridge_call(session,call,result);
                  switch_safe_free(contact); 
                                  
                       }
                  break;
                  }
        else if(!strcmp(tokl[0],"playback")){
	       handle_play_bck(channel,dsn,mutex,tokl[1]);
               break;	
		}
	else if(!strcmp(tokl[0],"ivr")){
	push(pt,call->did.dst_id);
	call->did.dst_id=atoi(tokl[1]);
	//push(pt,call->did.dst_id);
       switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_NOTICE,"The top element is %d\n", peek(pt));

	handle_ivr(channel, dsn,mutex,call,pt,1);
	break;
	}
	else if(!strcmp(tokl[0],"back2ivr")){
	  if (!isEmpty(pt)){
        	call->did.dst_id=pop(pt);
          	handle_ivr(channel, dsn,mutex,call,pt,0);
               
        switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," CQ _SQL : %d\n",call->did.dst_id);       
		}    
         break;
	}

        else if(!strcmp(tokl[0],"0")){ 
              switch_channel_hangup(channel,SWITCH_CAUSE_CALL_REJECTED);

                 break;
                  }
        else if(!strcmp(tokl[0],"-1")){
                continue; 
             }              
       }
       
    
    switch_safe_free(tokens); 
    switch_safe_free(tok);
   switch_safe_free(tokl);  
   return;

}

void handle_prompt(switch_channel_t *channel ,const char* dialstatus){
 
    switch_core_session_t* session = switch_channel_get_session(channel);
    if(  (dialstatus!=NULL) && !strcmp(dialstatus,"BUSY")  ){
             switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cause1: %d\n",!strcmp(dialstatus,"BUSY") );
             switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/call_rejected.wav", NULL);
        }else if( dialstatus!=NULL  && !strcmp(dialstatus,"NOANSWER") ){
                 switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/ivr-no_user_response.wav", NULL);
        }else if( dialstatus!=NULL  &&!strcmp(dialstatus,"UNALLOCATED_NUMBER")  ){
                    switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR," cause: %s\n",dialstatus);
                      switch_ivr_play_file(session, NULL, "/home/cloudconnect/pbx_new/upload/def_prompts/ivr-unallocated_number.wav", NULL);
        }

}

//split function
char  ** split(char string[], int * num, char * sep) {
    char * pch;
    char ** out = 0;
        int i = 0;
    pch = strtok (string, sep );
        
    while (pch != 0 ) {
                out = realloc(out, (  i + 1 ) * sizeof( char * ));
        out[i] = malloc( strlen(pch ) + 1 );
                strcpy( out[i], pch );
                ++i;
        pch = strtok (NULL, sep);
    }
        *num = i;
    return out;
}
//add  element
void addLast(struct node **head, char * val,char * key){
    
    struct node *newNode = malloc(sizeof(struct node));
    newNode->data = val;
    newNode->key=key;
    newNode->next = NULL;

 
    if(*head == NULL)
         *head = newNode;
  
    else
    {
        struct node *lastNode = *head;

        while(lastNode->next != NULL)
        {
            lastNode = lastNode->next;
        }

        lastNode->next = newNode;
    }

}
struct node * search(struct  node *head, char *key){  
    
    struct node *curNode;
    curNode = head;
   
    while (curNode != NULL){
	  
	 if( !strcmp(curNode->data, key))
           {
	return curNode ;
         
 	  }
        else
          curNode = curNode->next;
   }
    return NULL;
}




void printList(struct node *head){
    struct node *temp = head;

    //iterate the entire linked list and print the data
        while(temp != NULL)
           {
	    //switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR   ,"%s->", temp->data);
            //switch_log_printf(SWITCH_CHANNEL_LOG,SWITCH_LOG_ERROR       , "%s->",temp->key);
            temp = temp->next;
             }
                                              
       }


//stack database

struct stack* newStack(int capacity)
{
        struct stack *pt = (struct stack *) malloc(sizeof(struct stack));

        pt->maxsize = capacity;
        pt->top = -1;
        pt->items = (int*) malloc(sizeof(int) *capacity);

        return pt;
}


int size(struct stack *pt)
{
        return pt->top + 1;
}


int isEmpty(struct stack *pt)
{
        return pt->top == -1;
}


int isFull(struct stack *pt)
{
        return pt->top == pt->maxsize - 1;
}


void push(struct stack *pt, int x)
{

        if (isFull(pt))
        {
                printf("Overflow\nProgram Terminated\n");
                exit(EXIT_FAILURE);
        }

        printf("Inserting %d\n", x);


        pt->items[++pt->top] = x;
}


int peek(struct stack *pt)
{

        if (!isEmpty(pt))
        {
                return pt->items[pt->top];
        }
        else
        {
                exit(EXIT_FAILURE);
        }
}

int pop(struct stack *pt)
{

        if (isEmpty(pt))
        {
                printf("Underflow\nProgram Terminated\n");
                exit(EXIT_FAILURE);
        }

        printf("Removing %d\n", peek(pt));

        return pt->items[pt->top--];
}


