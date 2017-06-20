#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <yajl/yajl_tree.h>
#include "garudaparser.c"

typedef struct{
	char *condition;
	char *entity;
	char *source;
        char *dest;
        uint32_t sec;
        uint32_t usec;
}GarudaMessage;


typedef struct{
	char *action;
	char *resource_name;
	char *resource_value;
        char *source;
        struct timeval *source_time;
        struct timeval *dest_time;
}Actiondata;

// methods
Config * getConfig();
Actiondata *  parseCollectdMessage(Config * config,char * jsonMessage);



/*int main(){
	Config *config =getConfig();
	unsigned char message[65536]="{\"values\":[1],\"dstypes\":[\"gauge\"],\"dsnames\":[\"value\"],\"time\":1497538754.422, \
\"interval\":0.005,\"host\":\"nfvha-comp-04.oot.lab.eng.bos.redhat.com\",\"plugin\":\"connectivity\",\"/plugin_instance\":\"\", \
\"type\":\"gauge\",\"type_instance\":\"eno2\",\"meta\":{\"condition\":\"interface_up\", \
\"entity\":\"eno2\",\"source\":\"aputtur.fedora23\",\"clock\":\"1497538754422\"}}";


	Actiondata * actiondata=parseCollectdMessage(config,message);
         //printf("ACTION%s\n",actiondata->action);
         //printf("RESOURCE_NAME%s\n",actiondata->resource_name);
	//printf("RESOURCE_VALUE%s\n",actiondata->resource_value);


	free(config);
	free(actiondata);
}*/

// main config function
Config * getConfig(){
	Config *config=loadMonitoringConfig();
	return config;
}

/*
Parses CollectMessage and return struct to read action
*/

Actiondata *  parseCollectdMessage(Config * config,char * jsonMessage){
	char errbuf[1024];
	yajl_val node;
       printf("\nJSON MESSAGE%s\n",jsonMessage);
	/* we have the whole config file in memory.  let's parse it ... */
	node = yajl_tree_parse((const char *) jsonMessage, errbuf, sizeof(errbuf));
	GarudaMessage *message;
	if (node != NULL) {
            message=(GarudaMessage *)malloc(sizeof(GarudaMessage));
	    const char * path1[] = { "meta", "condition" ,(const char *) 0 };
	     message->condition=YAJL_GET_STRING(yajl_tree_get(node, path1, yajl_t_string));
	     const char * path2[] = { "meta", "entity" ,(const char *) 0 };
	     message->entity= YAJL_GET_STRING(yajl_tree_get(node, path2, yajl_t_string));
	     const char * path3[] = { "meta", "source" ,(const char *) 0 };
	     message->source= YAJL_GET_STRING(yajl_tree_get(node, path3, yajl_t_string));
             const char * path4[] = { "meta", "dest" ,(const char *) 0 };
             message->dest= YAJL_GET_STRING(yajl_tree_get(node, path4, yajl_t_string));
             const char * path5[] = { "meta", "sec" ,(const char *) 0 };
             char *sec=YAJL_GET_STRING(yajl_tree_get(node, path5, yajl_t_string));
             const char * path6[] = { "meta", "usec" ,(const char *) 0 };
             char *usec=YAJL_GET_STRING(yajl_tree_get(node, path6, yajl_t_string));
             //char *ptr;
             message->sec  = strtoul (sec, NULL, 0);
             message->usec=strtoul(usec,NULL,0);
               
	}else{
			printf("Message node is null");
			return 0;

		}
       /* printf("Message:Condition%s\n",message->condition);
	printf("Message:Entity%s\n",message->entity);
	printf("Message:Source%s\n",message->source);
       printf("Message:dest%s\n",message->dest);
          printf("Message:dest%s\n",message->clock);*/





	//printConfig(config);
	//printf("CONDITION sending %s\n",(char *)message->condition);
        char hostname[1024];
        hostname[1023] = '\0';
        gethostname(hostname, 1023);
        const char * returnValue;
        
        if(!strcmp(hostname,message->source)){
                returnValue =getJsonString((char *)message->condition,config);
        }else{  
                 returnValue =getJsonString((char *)message->dest,config);

         }

   

  //	const char * returnValue =getJsonString((char *)message->condition,config);
	//printf("JSON STRING***:%s\n",returnValue);
	//{"aputtur.fedora23":{"action":"add_vip","resource_name":"eno2","resource_value":"192.168.1.100"},"nfvha-comp-03.oot.lab.eng.bos.redhat.com":{"action":"delete_vip","resource_name":"eno2","resource_value":"192.168.1.100"}}
	//printf("hostname:%s\n",hostname);
	node = yajl_tree_parse((const char *) returnValue, errbuf, sizeof(errbuf));
	Actiondata *actiondata;
  	actiondata=(Actiondata *)malloc(sizeof(Actiondata));
        //struct timeval sourcetime;
        //sourcetime=(timeval *)malloc(sizeof(timeval));
        struct timeval tv;
        gettimeofday(&tv, NULL);
        actiondata->source_time=(struct timeval * )malloc(sizeof(struct timeval));
        actiondata->source_time->tv_sec=message->sec;
        actiondata->source_time->tv_usec=message->usec;
        actiondata->source=strdup(message->source);
        actiondata->dest_time=(struct timeval* )malloc(sizeof(struct timeval));
        actiondata->dest_time->tv_sec=tv.tv_sec;
        actiondata->dest_time->tv_usec=tv.tv_usec;


	if(node!=NULL){
                                    
                /*  if(!strcmp(hostname,message->source)){ //if hostname and message source is same
                      const char * action_path[] = { hostname, "action" ,(const char *) 0 };
                       actiondata->action=strdup(YAJL_GET_STRING(yajl_tree_get(node, action_path, yajl_t_string)));
                  }else{
                      const char * action_path[] = {message->source , "action" ,(const char *) 0 };
                      actiondata->action=strdup(YAJL_GET_STRING(yajl_tree_get(node, action_path, yajl_t_string)));
                  }*/
                  const char * action_path[] = {hostname , "action" ,(const char *) 0 };
                   actiondata->action=strdup(YAJL_GET_STRING(yajl_tree_get(node, action_path, yajl_t_string)));


		  const char * resource_name_path[] = { hostname, "resource_name" ,(const char *) 0 };
		  actiondata->resource_name=strdup(YAJL_GET_STRING(yajl_tree_get(node, resource_name_path, yajl_t_string)));
		  const char * resource_value_path[] = { hostname, "resource_value" ,(const char *) 0 };
		  actiondata->resource_value=strdup(YAJL_GET_STRING(yajl_tree_get(node, resource_value_path, yajl_t_string)));
		  
	}else{
		printf("No action is configured for this action");
	}

	yajl_tree_free(node);	
	free(message);

	free((char*)returnValue);
	//"meta":{"condition":"interface_down","entity":"eno2","source":"nfvha-comp-04.oot.lab.eng.bos.redhat.com","clock":"1497538751427"}
	return actiondata;

}
