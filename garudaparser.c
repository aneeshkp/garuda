/* Example code: yaml file parser using libyaml
 * author: smavros
 * 25 Jan 2016
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <yaml.h>
#define ENTITY_COUNT 3
#define RELATIONSHIP_COUNT 3
#define RULE_COUNT 2
#define ACTION_COUNT 2
#define MESSAGE_COUNT 2

/* types  ------------------------------------------------------------- */

typedef 
    struct entity{
        char *category;
        char *type;
        char *id;
        char *name;
    } Entity;


typedef 
    struct relationship{
        char *source;
        char *relationship_type;
        char *id;
        
    } Relationship;

typedef 
    struct message{
        char *condition;
        char *source;
        char *target;
        char *resource;
    } Message;



typedef 
    struct action{
        char *action_type;
        char *resource_name;
        char *resource_value;
        char *target;
    } Action;

typedef 
    struct rule{
        char *condition;
        Action actions[ACTION_COUNT];
        
    } Rule;

//main struct 
typedef struct config{
  Entity entities[ENTITY_COUNT];
  Relationship realationships[RELATIONSHIP_COUNT];
  Rule rules[RULE_COUNT];
  Message messages[MESSAGE_COUNT];
  int rule_lastindex;
}Config;

/* prototypes --------------------------------------------------------- */

/* init util */
//void init_data(Coil_t *coil, char **argv);
void init_data(Config *config);
/* global parser */
unsigned int parser(Config *config);

/* parser util */
void init_prs(FILE **fh, yaml_parser_t *parser);
void parse_next(yaml_parser_t *parser, yaml_event_t *event);
void clean_prs(FILE **fh, yaml_parser_t *parser, yaml_event_t *event);

/* parser actions */
void event_switch(bool *seq_status, unsigned int *map_seq, Config *config,\
    yaml_parser_t *parser, yaml_event_t *event);
void to_data(bool *seq_status, unsigned int *map_seq, Config *config,\
    yaml_parser_t *parser, yaml_event_t *event);
void to_data_from_map(char *buf, unsigned int *map_seq, Config *config,\
    yaml_parser_t *parser, yaml_event_t *event);
const char *getJsonString(char * condition, Config * config);
Config * loadMonitoringConfig();
void printConfig(Config * config);

/* printing */

void freeConfig(Config * config);

/* main --------------------------------------------------------------- */

/*int main(int argc, char **argv)
{ 
  //template.yaml
  

    
   
   //read struct and get

    // check for condition interface up /interface down
    // constrcu json msh 
  
    Config *config=loadMonitoringConfig();
    printConfig(config);
   const char * returnValue =getJsonString("interface_up",config);
    printf("JSON STRING***:%s\n",returnValue);
    free((char*)returnValue);

    free(config);
    // freeConfig(&config);

    return 0;
}*/

//"meta":{"condition":"interface_down","entity":"eno2","source":"nfvha-comp-04.oot.lab.eng.bos.redhat.com","clock":"1497538751427"}
void performAction(){

}


Config * loadMonitoringConfig(){
 Config *config;                    /* create my coil */
    //init_data(config);
    config = (Config *) malloc(sizeof(Config));
    int i=0;
    //config->rule_lastindex=(int *)malloc(sizeof(int));
    config->rule_lastindex=0;
    parser(config);
    return config;
 
  
}

char * getEntityNameById(Config *config,char *entity_id){
 for(int e=0;e<ENTITY_COUNT;e++){
  if(!strcmp(config->entities[e].id,entity_id)){
    return config->entities[e].name;
  }
 }
 return "";
}
const char * getJsonString(char * condition, Config * config){ 
  /*   {
  "host1": {
    "action": "add_vip",
    "resource_value": "12233",
    "resource_name":"eno2"
  },
  "host2": {
    "action": "add_vip",
     "resource_value": "12233",
     "resource_name":"eno2"
   }
  }*/
   //char json[255];
   char *json=malloc(255 * sizeof(char));
   json[0]='\0';
   for(int k=0;k<RULE_COUNT;k++){
    if(!strcmp(condition,config->rules[k].condition)){
      strcat(json,"{");
      for (int j=0;j<ACTION_COUNT;j++){
        //construct json here
        strcat(json,"\"");
        strcat(json, config->rules[k].actions[j].target);
        strcat(json,"\":{");
        strcat(json,"\"action\":\"");
        strcat(json, config->rules[k].actions[j].action_type);
        strcat(json,"\",");
        strcat(json,"\"resource_name\":\"");
        strcat(json, config->rules[k].actions[j].resource_name);
        strcat(json,"\",");
        strcat(json,"\"resource_value\":\"");
        strcat(json, config->rules[k].actions[j].resource_value);
        strcat(json,"\"}");
        if(j!=ACTION_COUNT-1)strcat(json,",");
      }
     strcat(json,"}");
    }
   }

   return json;
}

void printConfig(Config * config){
    printf("FINAL Printing Data\n");

    printf("*******ENTITY*****\n");
    for(int i=0;i<ENTITY_COUNT;i++){

      printf("\tEntities[%d].category:%s\n",i,config->entities[i].category);
      printf("\tEntities[%d].type:%s\n",i,config->entities[i].type);
      printf("\tEntities[%d].name:%s\n",i,config->entities[i].name);
      printf("\tEntities[%d].id:%s\n",i,config->entities[i].id);
      printf("\n");
    }
    printf("******Relation\n");
    for(int i=0;i<RELATIONSHIP_COUNT;i++){
      printf("\tsource:%s\n",config->realationships[i].source);
      printf("\trelation_type:%s\n",config->realationships[i].relationship_type);
      printf("\tid:%s\n",config->realationships[i].id);
    }
    printf("*****message ****\n");
    for(int i=0;i<MESSAGE_COUNT;i++){
      printf("\tconditon:%s\n",config->messages[i].condition);
      printf("\tsource:%s\n",config->messages[i].source);
      printf("\ttarget:%s\n",config->messages[i].target);
      printf("\tresource:%s\n",config->messages[i].resource);
    }

    printf("*****Rules****\n");
      for(int k=0;k<RULE_COUNT;k++){
      printf("rules[%d].conidtion=%s\n",k,config->rules[k].condition);
      for (int j=0;j<ACTION_COUNT;j++){
        printf("\trules[%d].action[%d].action_type:%s\n",k,j,config->rules[k].actions[j].action_type);
        printf("\trules[%d].action[%d].resource_name:%s\n",k,j,config->rules[k].actions[j].resource_name);
        printf("\trules[%d].action[%d].resource_value:%s\n",k,j,config->rules[k].actions[j].resource_value);
        printf("\trules[%d].action[%d].target:%s\n",k,j,config->rules[k].actions[j].target);
      }
    }
}
void freeConfig(Config *config){
     free( config->entities );
     free( config->realationships );
     for(int i=0;i<RULE_COUNT;i++){
        free( config->rules[i].actions);
      }
     free( config->rules );
     free( config->messages );
     //free(config->rule_lastindex);
}


void
init_data(Config *config)
{
  config = (Config *) malloc(sizeof(Config));

    //config->rule_lastindex=0;
//    config->entities=calloc(ENTITY_COUNT,sizeof(Entity));
  //  config->realationships=calloc(RELATIONSHIP_COUNT,sizeof(Relationship));
  //  config->rules=calloc(RULE_COUNT,sizeof(Rule));
   // for(int i=0;i<RULE_COUNT;i++){
    //   config->rules[i].actions=calloc(ACTION_COUNT,sizeof(Action));
    // }
   // config->messages=calloc(MESSAGE_COUNT,sizeof(Message));
   // config->rule_lastindex=0;
}

unsigned int
parser(Config *config)
{
    /* open file & declare libyaml types */
    FILE *fh = fopen("template.yaml","r");
    yaml_parser_t parser;
    yaml_event_t event;
    
    bool seq_status = 0;  /* IN or OUT of sequence index, init to OUT */
    unsigned int map_seq = 0;  /* index of mapping inside sequence */

    init_prs(&fh,&parser);  /* initiliaze parser & open file */
    unsigned int block_seq=0;
    
    do {
        
        parse_next(&parser,&event);  /* parse new event */
        
        /* decide what to do with each event */
        event_switch(&seq_status,&map_seq,config,&parser,&event);   
        
        if ( event.type != YAML_STREAM_END_EVENT ){
            yaml_event_delete(&event);
        }
        
    } while( event.type != YAML_STREAM_END_EVENT );

    clean_prs(&fh,&parser,&event);  /* clean parser & close file */
   
    return map_seq;
}

void
event_switch(bool *seq_status, unsigned int *map_seq, Config *config,\
    yaml_parser_t *parser, yaml_event_t *event)
{
    switch(event->type){
        case YAML_STREAM_START_EVENT: break;
        case YAML_STREAM_END_EVENT: break;
        case YAML_DOCUMENT_START_EVENT: break;
        case YAML_DOCUMENT_END_EVENT: break;
        case YAML_SEQUENCE_START_EVENT:
             //printf("SEQUENC STARTED\n");
            (*seq_status) = true;
            break;
        case YAML_SEQUENCE_END_EVENT:
            (*seq_status) = false;
            (*map_seq)=0;
            //printf("SEQUENC ENDED\n");
            break;
        case YAML_MAPPING_START_EVENT:
            //printf("MAPPING STARTED\n");
            if( *seq_status == 1 ){
                (*map_seq)++;
            }
            break;
        case YAML_MAPPING_END_EVENT: 
            //printf("MAPPING ENDED\n");
             break;
        case YAML_ALIAS_EVENT:
            printf(" ERROR: Got alias (anchor %s)\n", \
                event->data.alias.anchor);
            exit(EXIT_FAILURE);
            break;
        case YAML_SCALAR_EVENT:
            to_data(seq_status,map_seq,config,parser,event);
            break;
        case YAML_NO_EVENT:
            puts(" ERROR: No event!");
            exit(EXIT_FAILURE);
            break;
    }
}

void
to_data(bool *seq_status, unsigned int *map_seq, Config *config,\
    yaml_parser_t *parser, yaml_event_t *event)
{
    char *buf = (char *) event->data.scalar.value;
    
    //printf("------------------------------R%d\n",config->rule_lastindex);
    //printf("%s = scalar %s \n", buf,(char *)event->data.scalar.value);
    
     //if ( (*seq_status) == true ){

      if(!strcmp(buf, "entity")){
          parse_next(parser,event);
          parse_next(parser,event);
          parse_next(parser,event);
          config->entities[(*map_seq)-1].category=strdup((char *)event->data.scalar.value);
          parse_next(parser,event);
          parse_next(parser,event);
          config->entities[(*map_seq)-1].type=strdup((char *)event->data.scalar.value);
          parse_next(parser,event);
          parse_next(parser,event);
          config->entities[(*map_seq)-1].name=strdup((char *)event->data.scalar.value);
          parse_next(parser,event);
          parse_next(parser,event);
          config->entities[(*map_seq)-1].id=strdup((char *)event->data.scalar.value);
         /* printf("Printing Entity---- as collected \n");
          printf("Entity[%d].category=%s  \n", (*map_seq)-1,config->entities[(*map_seq)-1].category);
          printf("Entity[%d].type=%s  \n", (*map_seq)-1,config->entities[(*map_seq)-1].type);
          printf("Entity[%d].name=%s  \n", (*map_seq)-1,config->entities[(*map_seq)-1].name);
          printf("Entity[%d].id=%s  \n", (*map_seq)-1,config->entities[(*map_seq)-1].id);*/



      }else if(!strcmp(buf, "relationship")){
          parse_next(parser,event);
          parse_next(parser,event);
          parse_next(parser,event);
          //printf("\t%s :1 %s \n", buf,(char *)event->data.scalar.value);
         config->realationships[(*map_seq)-1].source=strdup((char *)event->data.scalar.value);
        //printf("%s===<\n",(char*)config->entities[(*map_seq)-1].category);
          parse_next(parser,event);
          parse_next(parser,event);
          
          config->realationships[(*map_seq)-1].relationship_type=strdup((char *)event->data.scalar.value);
          parse_next(parser,event);
          parse_next(parser,event);
          
          config->realationships[(*map_seq)-1].id=strdup((char *)event->data.scalar.value);
          
     
          
         
      }else if(!strcmp(buf, "message")){
          //printf("%s*******************Block \n", buf);
          parse_next(parser,event);
          parse_next(parser,event);
          parse_next(parser,event);
          //printf("\t%s :1 %s \n", buf,(char *)event->data.scalar.value);
         config->messages[(*map_seq)-1].condition=strdup((char *)event->data.scalar.value);
        //printf("%s===<\n",(char*)config->entities[(*map_seq)-1].category);
          parse_next(parser,event);
          parse_next(parser,event);
          
          //printf("\t%s :2 %s \n", buf,(char *)event->data.scalar.value);
          config->messages[(*map_seq)-1].source=strdup((char *)event->data.scalar.value);
          parse_next(parser,event);
          parse_next(parser,event);
          
          //printf("\t%s :3 %s \n", buf,(char *)event->data.scalar.value);
          config->messages[(*map_seq)-1].target=strdup((char *)event->data.scalar.value);
          parse_next(parser,event);
          parse_next(parser,event);
          //printf("\t%s :3 %s \n", buf,(char *)event->data.scalar.value);
          config->messages[(*map_seq)-1].resource=strdup((char *)event->data.scalar.value);
          
      
      }else if(!strcmp(buf, "rule")){
         // printf("%s*******************Block \n", buf);
          //printf("%s*******************Block \n", buf);
          parse_next(parser,event);
          parse_next(parser,event);
          parse_next(parser,event);
          
          
          //printf("------------------------------Rule Idec%d\n",config->rule_lastindex);
          config->rules[config->rule_lastindex].condition=strdup((char *)event->data.scalar.value);
          //config->rules[config->rule_lastindex+1].condition=strdup((char *)event->data.scalar.value);
          config->rule_lastindex=config->rule_lastindex+1;
          //printf("Rule Idec%d\n",config->rule_lastindex);
          (*map_seq)=0;
          
        //printf("%s===<\n",(char*)config->entities[(*map_seq)-1].category);
         
       }else if(!strcmp(buf, "action")){
         //printf("%s*******************Block \n", buf);

         int index=config->rule_lastindex-1;
         
          parse_next(parser,event);
          parse_next(parser,event);
          parse_next(parser,event);
          //printf("INSIDE ACTION ===%d\n",config->rule_lastindex);
          //printf("INSIDE ACTION 2==%d\n",(*map_seq)-1);
          //printf("\tRule[%d].actions[%d].actiontype=%s  \n", index,(*map_seq)-1,(char *)event->data.scalar.value);
          config->rules[index].actions[(*map_seq)-1].action_type=strdup((char *)event->data.scalar.value);
          parse_next(parser,event);
          parse_next(parser,event);
          //printf("\tRule[%d].actions[%d].param=%s  \n", index,(*map_seq)-1,(char *)event->data.scalar.value);
          
          config->rules[index].actions[(*map_seq)-1].resource_name=strdup(getEntityNameById(config,(char *)event->data.scalar.value));
          parse_next(parser,event);
          parse_next(parser,event);
          config->rules[index].actions[(*map_seq)-1].resource_value=strdup((char *)event->data.scalar.value);
          parse_next(parser,event);
          parse_next(parser,event);
          //printf("\tRule[%d].actions[%d].target=%s  \n", index,(*map_seq)-1,(char *)event->data.scalar.value);
          config->rules[index].actions[(*map_seq)-1].target=strdup(getEntityNameById(config,(char *)event->data.scalar.value));
          
        }
         
    
}

void
to_data_from_map(char *buf, unsigned int *map_seq, Config *config,\
    yaml_parser_t *parser, yaml_event_t *event)
{
    char *cb; /* char part buffer for strtod() */
   
    /* dictionary */
    char *rad = "radius";
    char *xcen = "x_center";
    char *ycen = "y_center";
    parse_next(parser,event);
   
}

void
parse_next(yaml_parser_t *parser, yaml_event_t *event)
{
    /* parse next scalar. if wrong exit with error */
    if ( !yaml_parser_parse(parser, event) ){
        printf("Parser error %d\n", parser->error);
        exit(EXIT_FAILURE);
    }
}

void
init_prs(FILE ** fh, yaml_parser_t * parser)
{
    /* parser initilization */
    if( !yaml_parser_initialize(parser) ){
        fputs("Failed to initialize parser!\n",stderr);
    }
    if( *fh == NULL ){
        fputs("Failed to open file!\n",stderr);
    }

    yaml_parser_set_input_file(parser, *fh); /* set input file */
}

void
clean_prs(FILE ** fh, yaml_parser_t * parser, yaml_event_t * event)
{
    yaml_event_delete(event);    /* delete event */
    yaml_parser_delete(parser);  /* delete parser */
    fclose(*fh);           /* close file */
}



/* end */
