#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <locale.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include "my402list.h"
#include "cs402.h"

pthread_t packet_arrv_thread;
pthread_t token_arrv_thread;
pthread_t server_arrv_thread1;
pthread_t server_arrv_thread2;
pthread_t cntrlc_thread;
 
char* filename;
int server_1=1;
int server_2=2;
int *S1=&server_1;
int *S2=&server_2;
int check=0;

typedef struct thread_param
{
    double lambda;
    double mu;
    double r;
    int b;
    int p;
    int num;
}param;

param *a;

typedef struct pkt_str
{
    int pkt_num;
    int token_num;
    struct timeval server_stop_service;
    struct timeval pkt_strt_time;
    struct timeval pkt_appended_q1;
    struct timeval pkt_unlinked_q1;
    struct timeval pkt_unlink_q1;
    struct timeval pkt_appended_q2;
    struct timeval pkt_unlinked_q2;
    struct timeval pkt_beg_ser;
    //struct timeval token_start_time;
}pktstr;

struct timeval start,end;
struct timeval token_start_time;
struct timeval last_pkt_time;

useconds_t service_time;

double time_s;
int bucket_size;
double time_end;
double time_start;

void* pkt_arrv(void *emu_beg);
void* token_arrv(void *emu_beg);
void *server_thrd(void *emu_beg);
void stats();
void *monitor();
void emu_param();

My402List *q1;
My402List *q2;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

int drop_pkts_num=0;
long drop_token_num=0;
int total_pkt=1;
int token_count=0;
int pkts_in_server=0;
double pr_onebylambda;
double pr_onebymu;
double pr_onebyr;
sigset_t set;


unsigned long tot_inter_arrv_time=0;
unsigned long tot_pkt_serv_time=0;
unsigned long tot_time_in_q1 = 0;
unsigned long tot_time_in_q2=0;
unsigned long tot_time_in_sys=0;
unsigned long sqr_of_time_in_sys=0;

double mu[1000];


int pakt_arrv_time=0;
double act_pakt_arrv_time=0;


void Usage()
{
    fprintf(stderr,"Usage: warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]\n");
    exit(EXIT_FAILURE);
}  

void* pkt_arrv(void *emu_beg)
{
      int l=1;
      last_pkt_time.tv_sec = start.tv_sec;
      last_pkt_time.tv_usec = start.tv_usec;
      char *tot_pkts = malloc(1024*sizeof(char));
      char *oneoverlambda = malloc(4*sizeof(char));
      char *tot_token = malloc(4*sizeof(char));
      char *oneovermu = malloc(4*sizeof(char));
      char buf[5000];
      int ret = 0;

      My402ListElem *q1_first_elem;
      q1_first_elem=(My402ListElem*)malloc(sizeof(My402ListElem));

      pktstr *q1_obj;
      q1_obj = (pktstr*)malloc(sizeof(pktstr));

      double act_one_by_lambda =(1/a->lambda)*1000;
      pr_onebylambda = act_one_by_lambda*1000;
              
      if(pr_onebylambda == 0 || pr_onebylambda < 0 )
      {fprintf(stderr,"The value of 'lambda' is not proper\n");
         //printf("after Calc\n");
         exit(EXIT_FAILURE);
      }
      else if(pr_onebylambda > 10000000)
      {pr_onebylambda = 10000000; 
      }
      else
      {pr_onebylambda = pr_onebylambda;
      }

           
      FILE *fp;
      if(emu_beg != NULL)
      {
  
                char* fname = NULL;
                fname = malloc(1024*(sizeof(char)));
                fname = (char*) emu_beg;
                fp=fopen(fname,"r");
                //printf("file\n");

            if(fp==NULL) 
            {          fprintf(stderr, "Input file does not exists\n");
                      exit(-1);
            }
        
            fgets(tot_pkts,2000,fp);

            if(atoi(tot_pkts) == 0 || atoi(tot_pkts)<0 || atoi(tot_pkts)> 2147483647)
            {         fprintf(stderr,"The value of 'packet' is not proper\n");
                      exit(EXIT_FAILURE);
            } 
             a->num = atoi(tot_pkts);
     
      } 
sigprocmask(SIG_BLOCK,&set,0);
double act_pakt_arrv_time;
double act_inter_arrv_time;
 while(1)
 {
   
        
        if((fp!= NULL) && (emu_beg != NULL))
        {

            fgets(buf,2000,fp);
            sscanf(buf,"%s %s %s",oneoverlambda,tot_token,oneovermu);
      
            if(atof(oneoverlambda)== 0 || atof(oneoverlambda) <0 /*|| atof(oneoverlambda) < 2147483647*/)
            {  fprintf(stderr,"The value of 'lambda' is not proper\n");
                printf("here file\n");
                exit(EXIT_FAILURE);
            }

            a->lambda=atof(oneoverlambda);
            pr_onebylambda = (a->lambda)*1000;
                 
            if(atoi(tot_token)== 0 || atoi(tot_token)<0 /*|| atoi(tot_token)> 2147483647*/)
            { fprintf(stderr,"The value of 'token' is not proper\n");
              exit(EXIT_FAILURE);
            }
            a->p = atoi(tot_token);
               
            if(atof(oneovermu)== 0 || (atof)(oneovermu)<0 /*|| (atof)(oneovermu)> 2147483647*/)
            {  fprintf(stderr,"The value of 'mu' is not proper\n");
               exit(EXIT_FAILURE);
            }
            a->mu = atof(oneovermu);
            pr_onebymu = (a->mu)*1000;
        }
              
            pktstr *arrv_pkt;
            arrv_pkt = (pktstr*)malloc(sizeof(pktstr));

            arrv_pkt->pkt_num = total_pkt;
            arrv_pkt->token_num = a->p;
            
            //printf("sleep time in pkt thrd %lf \n",pr_onebylambda);
            usleep(pr_onebylambda);
            gettimeofday(&arrv_pkt->pkt_strt_time,NULL);

            if(pr_onebylambda > 10000000)
              {pr_onebylambda = 10000000;
              }

            pakt_arrv_time=(arrv_pkt->pkt_strt_time.tv_sec - start.tv_sec) * 1000000 + ( arrv_pkt->pkt_strt_time.tv_usec - start.tv_usec);
            act_pakt_arrv_time = (double)pakt_arrv_time/1000;

            double inter_arrv_time= (arrv_pkt->pkt_strt_time.tv_sec * 1000000 + arrv_pkt->pkt_strt_time.tv_usec) - (last_pkt_time.tv_sec*1000000 + last_pkt_time.tv_usec);
            last_pkt_time.tv_sec = arrv_pkt->pkt_strt_time.tv_sec;
            last_pkt_time.tv_usec = arrv_pkt->pkt_strt_time.tv_usec;
            act_inter_arrv_time = inter_arrv_time/1000;

                      
        if((arrv_pkt->pkt_num > a->num && My402ListEmpty(q2)) || check ==1)
        {     
          pthread_exit(NULL);
        }
        
        if((arrv_pkt->pkt_num <= a->num) && (check != 1) && arrv_pkt->token_num <= a->b)
        {
          
            pthread_mutex_lock(&mutex);
           
            mu[l]=pr_onebymu;
           
            l++;

            My402ListAppend(q1,arrv_pkt);  
            total_pkt++;
         
            tot_inter_arrv_time=tot_inter_arrv_time+inter_arrv_time;

            printf("%012.3fms: p%d arrives, needs %d tokens, inter-arrival time = %.3fms\n",act_pakt_arrv_time,arrv_pkt->pkt_num,arrv_pkt->token_num,/*pr_onebylambda/1000*/act_inter_arrv_time);

            gettimeofday(&arrv_pkt->pkt_appended_q1,NULL);
            int q1_len=My402ListLength(q1);
            
            if(q1_len != 0) 
            {
            q1_first_elem = My402ListFirst(q1);
            q1_obj = (pktstr *)(q1_first_elem->obj);
            }
 
            double pkt_arrv_q1=(arrv_pkt->pkt_appended_q1.tv_sec*1000000+arrv_pkt->pkt_appended_q1.tv_usec)-time_s;
            double act_pkt_arrv_q1=pkt_arrv_q1/1000;
            printf("%012.3lfms: p%d enters Q1\n",act_pkt_arrv_q1,arrv_pkt->pkt_num);

            while(bucket_size >= q1_obj->token_num && q1_len!=0 )
            {

                
                bucket_size=bucket_size-q1_obj->token_num;
 
                My402ListUnlink(q1,q1_first_elem);
                gettimeofday(&q1_obj->pkt_unlinked_q1,NULL);

                double time_in_q1=(q1_obj->pkt_unlinked_q1.tv_sec * 1000000 + q1_obj->pkt_unlinked_q1.tv_usec)- (q1_obj->pkt_appended_q1.tv_sec * 1000000 + q1_obj->pkt_appended_q1.tv_usec);
                double act_time_in_q1=time_in_q1/1000;

                //tot_time_in_q1 = tot_time_in_q1 + time_in_q1;

                double pakt_leaves_q1=(q1_obj->pkt_unlinked_q1.tv_sec * 1000000 + q1_obj->pkt_unlinked_q1.tv_usec)- time_s;
                double act_pakt_leaves_q1=pakt_leaves_q1/1000;

                printf("%012.3lfms: p%d leaves Q1, time in Q1 = %07.3lfms, token bucket now has %d token\n",act_pakt_leaves_q1,q1_obj->pkt_num,act_time_in_q1,bucket_size);

                ret = My402ListAppend(q2,q1_obj);
                             
                gettimeofday(&q1_obj->pkt_appended_q2,NULL);

                double pakt_arrv_q2=(q1_obj->pkt_appended_q2.tv_sec * 1000000 + q1_obj->pkt_appended_q2.tv_usec)- time_s;
                double act_pakt_arrv_q2=pakt_arrv_q2/1000;
                printf("%012.3lfms: p%d enters Q2\n",act_pakt_arrv_q2,q1_obj->pkt_num);
                
                q1_len=My402ListLength(q1);
                if(q1_len != 0)
                  {
                      q1_first_elem=My402ListFirst(q1);
                      q1_obj = (pktstr *)q1_first_elem->obj;
                  }

                  int q2_len=My402ListLength(q2);
                  if((q2_len==1) && (ret == 1))
                 {
                  pthread_cond_broadcast(&cv);
                 }
            }              
         }
         else
         {

              printf("%012.3lfms: p%d arrives, needs %d tokens, inter-arrival time = %.3lfms, dropped\n",act_pakt_arrv_time,arrv_pkt->pkt_num,arrv_pkt->token_num,/*pr_onebylambda/1000*/act_inter_arrv_time);
              total_pkt++;
              arrv_pkt->pkt_num=total_pkt;
              drop_pkts_num++;
              pthread_cond_broadcast(&cv);
              if(arrv_pkt->pkt_num > a->num || check ==1)
                  {     
                    pthread_exit(NULL);
                  }
         }
//---------------------------mutex unlock-------------------------
     pthread_mutex_unlock(&mutex);
     gettimeofday(&end, NULL);
     
  }
fclose(fp);
 return 0;
}

void* token_arrv(void *emu_beg)
{

int ret = 0;
int pkt_num_after=0;

double act_one_by_r =(1/a->r)*1000;
        pr_onebyr = act_one_by_r*1000;
        if(pr_onebyr == 0 || pr_onebyr <0 )
        {
          fprintf(stderr,"The value of 'r' is not proper\n");
          exit(EXIT_FAILURE);
        }
        else if(pr_onebyr > 10000000)
        {
          pr_onebyr = 10000000; 
        }
        else
        {
          pr_onebyr = pr_onebyr;
        }

while(1)
 {
  

      sigprocmask(SIG_BLOCK,&set,0);

      if(pr_onebyr>10000000)
      {
          pr_onebyr=10000000;
      }

      // pthread_mutex_lock(&mutex);
      //printf("pr_onebyr %lf\n",pr_onebyr );
      usleep(pr_onebyr);

      pthread_mutex_lock(&mutex);

      gettimeofday(&token_start_time,NULL);
      //--------------------mutex lock---------------------
      
      token_count++;
      int token_arrv_tim=(token_start_time.tv_sec * 1000000 + token_start_time.tv_usec)- time_s;
      double act_token_arrv_time=(double)token_arrv_tim/1000;

     if(bucket_size < a->b)
     {

         bucket_size=bucket_size+1;
         printf("%012.3lfms: token t%d arrives, token bucket now has %d tokens\n",act_token_arrv_time,token_count,bucket_size);
     }
     else
     {
        //token_count++;
         drop_token_num++;
         printf("%012.3lfms: token t%d arrives, dropped\n",act_token_arrv_time,token_count);
     }
     //pthread_mutex_lock(&mutex);


     int q1_len = My402ListLength(q1);
     if(q1_len!=0)
     {

      My402ListElem *q1_first_elem;

      q1_first_elem=(My402ListElem*)malloc(sizeof(My402ListElem));
      q1_first_elem = My402ListFirst(q1);
      pktstr *q1_obj;
      q1_obj = (pktstr*)malloc(sizeof(pktstr));
      q1_obj = (pktstr *)(q1_first_elem->obj);
      pkt_num_after=q1_obj->pkt_num;
    
     while(bucket_size >= q1_obj->token_num /*&& q1_len!=0*/)
          {
    
              bucket_size=bucket_size - q1_obj->token_num;
              My402ListUnlink(q1,q1_first_elem);
              gettimeofday(&q1_obj->pkt_unlink_q1,NULL);

              double time_in_q1=(q1_obj->pkt_unlink_q1.tv_sec * 1000000 + q1_obj->pkt_unlink_q1.tv_usec)- (q1_obj->pkt_appended_q1.tv_sec * 1000000 + q1_obj->pkt_appended_q1.tv_usec);
              double act_time_in_q1=time_in_q1/1000;

              tot_time_in_q1 = tot_time_in_q1 + time_in_q1;
 
              double pkt_leaves_q1=(q1_obj->pkt_unlink_q1.tv_sec * 1000000 + q1_obj->pkt_unlink_q1.tv_usec)- time_s;
              double act_pkt_leaves_q1=pkt_leaves_q1/1000;

              printf("%012.3lfms: p%d leaves Q1, time in Q1 = %07.3lfms, token bucket now has %d token\n",act_pkt_leaves_q1,q1_obj->pkt_num,act_time_in_q1,bucket_size);

              ret = My402ListAppend(q2,q1_obj);
              gettimeofday(&q1_obj->pkt_appended_q2,NULL);

              double pakt_arrv_q2=(q1_obj->pkt_appended_q2.tv_sec * 1000000 + q1_obj->pkt_appended_q2.tv_usec)- time_s;
              double act_pakt_arrv_q2=pakt_arrv_q2/1000;

               printf("%012.3lfms: p%d enters Q2\n",act_pakt_arrv_q2,q1_obj->pkt_num);

               int q2_len=My402ListLength(q2);
               
              if((q2_len==1) && (ret == 1))
             {
                pthread_cond_broadcast(&cv);
             }

             
          }
      }
      pthread_mutex_unlock(&mutex);
      gettimeofday(&end, NULL);
      
      if(total_pkt > a->num && My402ListEmpty(q1))
      {
          pthread_exit(NULL);
      }

      } 
       
    return 0;
}

void *server_thrd(void *emu_beg)
{

My402ListElem *q2_first_elem;
q2_first_elem =(My402ListElem*)malloc(sizeof(My402ListElem));


pktstr* q2_obj;
q2_obj = (pktstr*)malloc(sizeof(pktstr));

int flag = 0;
int* srvno = NULL;
srvno = (int*) emu_beg;


while (1)
 {       
        sigprocmask(SIG_BLOCK,&set,0);
        pthread_mutex_lock(&mutex);
      
        if(My402ListEmpty(q2) && (pkts_in_server != (a->num - drop_pkts_num)))
        {
          pthread_cond_wait(&cv,&mutex);
        }

        else if(!My402ListEmpty(q2) && (pkts_in_server != (a->num - drop_pkts_num)))
                {           
                      
                  q2_first_elem = My402ListFirst(q2);
                  q2_obj = (pktstr *)(q2_first_elem->obj);         
                                                 
                  pkts_in_server++;
                                    
                  My402ListUnlink(q2,q2_first_elem);
                  flag = 1;
                  gettimeofday(&q2_obj->pkt_unlinked_q2,NULL);

                  double pkt_leaves_q2=(q2_obj->pkt_unlinked_q2.tv_sec * 1000000 + q2_obj->pkt_unlinked_q2.tv_usec)- time_s;
                  double act_pakt_leaves_q2 = pkt_leaves_q2/1000;

                  double time_in_q2 = (q2_obj->pkt_unlinked_q2.tv_sec-q2_obj->pkt_appended_q2.tv_sec)*1000000+(q2_obj->pkt_unlinked_q2.tv_usec-q2_obj->pkt_appended_q2.tv_usec);
                  double act_time_in_q2 = (double)time_in_q2/1000;

                  tot_time_in_q2 = tot_time_in_q2 + time_in_q2;
                           
                  printf("%012.3lfms: p%d leaves Q2, time in Q2 = %.3fms\n",act_pakt_leaves_q2,q2_obj->pkt_num,act_time_in_q2);
                  
                  gettimeofday(&q2_obj->pkt_beg_ser,NULL);
                  double pkt_beg_serv=(q2_obj->pkt_beg_ser.tv_sec * 1000000 + q2_obj->pkt_beg_ser.tv_usec)- time_s;
                  double act_pkt_beg_serv = pkt_beg_serv/1000;

                  printf("%012.3lfms: p%d begins service at S%d,requesting %.3fms of service\n",act_pkt_beg_serv/*,act_pakt_srv_q2*/,q2_obj->pkt_num,*srvno,mu[pkts_in_server]/1000);
            

                  }
      pthread_mutex_unlock(&mutex);                  

      if(flag == 1) 
        {         

            if(mu[pkts_in_server]>10000000)
            {
                  mu[pkts_in_server]=10000000;
            }

            // pthread_mutex_unlock(&mutex);
            // gettimeofday(&q2_obj->pkt_beg_ser,NULL);
            usleep(mu[pkts_in_server]);

            gettimeofday(&q2_obj->server_stop_service,NULL);

            // double pkt_beg_serv=(q2_obj->pkt_beg_ser.tv_sec * 1000000 + q2_obj->pkt_beg_ser.tv_usec)- time_s;
            // double act_pkt_beg_serv = pkt_beg_serv/1000;

            double pkt_departs_server=(q2_obj->server_stop_service.tv_sec * 1000000 + q2_obj->server_stop_service.tv_usec)- time_s;
            double act_pkt_departs_server = pkt_departs_server/1000;

            // printf("%012.3lfms: p%d begins service at S%d,requesting %.3fms of service\n",act_pkt_beg_serv/*,act_pakt_srv_q2*/,q2_obj->pkt_num,*srvno,mu[pkts_in_server]/1000);
            
            // pthread_mutex_unlock(&mutex);

            double pkt_service_time=(q2_obj->server_stop_service.tv_sec * 1000000 + q2_obj->server_stop_service.tv_usec)- (q2_obj->pkt_unlinked_q2.tv_sec * 1000000 + q2_obj->pkt_unlinked_q2.tv_usec);
            double act_pkt_service_time = pkt_service_time/1000;

            tot_pkt_serv_time = tot_pkt_serv_time+pkt_service_time;
            
            double pkt_in_system=(q2_obj->server_stop_service.tv_sec * 1000000 + q2_obj->server_stop_service.tv_usec)- (q2_obj->pkt_strt_time.tv_sec * 1000000 + q2_obj->pkt_strt_time.tv_usec);
            double act_pkt_in_system = pkt_in_system/1000;

            tot_time_in_sys = tot_time_in_sys + pkt_in_system;
            sqr_of_time_in_sys = sqr_of_time_in_sys + (pkt_in_system*pkt_in_system);


            printf("%012.3lfms: p%d departs from S%d, service time = %.3fms, time in system = %.3fms\n",act_pkt_departs_server,q2_obj->pkt_num,*srvno,act_pkt_service_time,act_pkt_in_system);
            flag = 0;
        }
 
            // pthread_mutex_unlock(&mutex);
            if(((My402ListEmpty(q2)) && (pkts_in_server == (a->num - drop_pkts_num))) || (check == 1))
            {
              pthread_cond_broadcast(&cv);
              pthread_exit(NULL);
            }
             
  }
    return 0;
}

void* thread_create()
    { 

              if(pthread_create(&packet_arrv_thread, NULL, pkt_arrv, 0))
              {
              fprintf(stderr, "Error creating thread\n");
              return (void*)1;
              }

              if(pthread_create(&token_arrv_thread, NULL, token_arrv, 0))
              {
              fprintf(stderr, "Error creating thread\n");
              return (void*)1;
              }

              if(pthread_create(&server_arrv_thread1, NULL, server_thrd, (void*) S1))
              {
              fprintf(stderr, "Error creating thread\n");
              return (void*)1;
              }

              if(pthread_create(&server_arrv_thread2, NULL, server_thrd, (void*) S2))
              {
              fprintf(stderr, "Error creating thread\n");
              return (void*)1;
              }

              if(pthread_create(&cntrlc_thread, NULL, monitor, NULL))
              {
              fprintf(stderr, "Error creating thread\n");
              return (void*)1;
              }

      return (void*)0;
    }

 void *monitor()
    {
      //int sig;
      while(1)
      {
      sigwait(&set);

      check=1;
      pthread_mutex_lock(&mutex);
      pthread_cancel(packet_arrv_thread);
      pthread_cancel(token_arrv_thread);

      struct timeval p_rem_q1;
      struct timeval p_rem_q2;

      while(!My402ListEmpty(q1))
      { 
        My402ListElem *q1_first;
        q1_first=(My402ListElem*)malloc(sizeof(My402ListElem));
        q1_first = My402ListFirst(q1);
        pktstr* obj_q1=(pktstr*)q1_first->obj;
        My402ListUnlink(q1,q1_first);
        
        gettimeofday(&p_rem_q1,NULL);
        double rem_pkt_q1 = (p_rem_q1.tv_sec * 1000000 + p_rem_q1.tv_usec)- time_s;
        double act_rem_pkt_q1= rem_pkt_q1/1000;
        printf("%012.3lfms:  p%d removed from Q1\n",act_rem_pkt_q1,obj_q1->pkt_num);
      }


      while(!My402ListEmpty(q2))
      { 
        My402ListElem *q2_first;
        q2_first=(My402ListElem*)malloc(sizeof(My402ListElem));
        q2_first = My402ListFirst(q2);
        pktstr* obj_q2=(pktstr*)q2_first->obj;
        My402ListUnlink(q2,q2_first);
        
        gettimeofday(&p_rem_q2,NULL);
        double rem_pkt_q2 = (p_rem_q2.tv_sec * 1000000 + p_rem_q2.tv_usec)- time_s;
        double act_rem_pkt_q2 = rem_pkt_q2/1000;
        printf("%012.3lfms:  p%d removed from Q1\n",act_rem_pkt_q2,obj_q2->pkt_num);
      }

      pthread_cond_broadcast(&cv);
      pthread_mutex_unlock(&mutex);
    }
      return 0;
    }

void stats()
{
     
        char str[10]="N/A";
        double act_avg_time_in_sys;
        
        printf("Statistics:\n");
             
        if((total_pkt-1)==0)
        {
        printf("    average packet inter-arrival time =%s\n",str);
        printf("    No packets arrived at this facility\n");
        }
        else
        {
        double avg_pkt_intr_time = tot_inter_arrv_time;
        double act_avg_pkt_intr_time = avg_pkt_intr_time/((total_pkt-1)-drop_pkts_num);
        printf("    average packet inter-arrival time =%.6gs\n",act_avg_pkt_intr_time/1000000);
        }
        if(pkts_in_server==0)
        {
        printf("    average packet service time =%s\n\n",str);
        printf("    No packets arrived at this facility\n");
        }
        else
        {
        double avg_pkt_srv_time = tot_pkt_serv_time;
        double act_avg_pkt_srv_time = avg_pkt_srv_time/pkts_in_server;
        printf("    average packet service time =%.6gs\n\n",act_avg_pkt_srv_time/1000000);
        }
        if(time_end==(double)0)
        {
        printf("    average number of packets in Q1 =%s\n",str);
        printf("    No packets arrived at this facility\n");
        }
        else
        {
        double avg_pkt_q1 = tot_time_in_q1;
        double act_avg_pkt_q1 = avg_pkt_q1/time_end;
        printf("    average number of packets in Q1 =%.6g\n",act_avg_pkt_q1);
        }
        if(time_end==(double)0)
        {
        printf("    average number of packets in Q2 =%s\n",str);
        printf("    No packets arrived at this facility\n");
        }
        else
        {
        double avg_pkt_q2=tot_time_in_q2;
        double act_avg_pkt_q2 = avg_pkt_q2/time_end;
        printf("    average number of packets in Q2 =%.6g\n",act_avg_pkt_q2); 
        }
        if(time_end==(double)0)
        {
        printf("    average number of packets at S1 =%s\n",str);
        printf("    No packets arrived at this facility\n");
        }
        else
        {
        double avg_pkt_S1= tot_pkt_serv_time;
        double act_avg_pkt_S1=avg_pkt_S1/time_end;
        printf("    average number of packets at S1 =%.6g\n",act_avg_pkt_S1);
        }
        if(time_end==(double)0)
        {
        printf("    average number of packets at S2 =%s\n\n",str);
        printf("    No packets arrived at this facility\n");
        }
        else
        {
        double avg_pkt_S2= tot_pkt_serv_time;
        double act_avg_pkt_S2 = avg_pkt_S2/time_end; 
        printf("    average number of packets at S2 =%.6g\n\n",act_avg_pkt_S2);
        }
        if(time_end==(double)0)
        {
        printf("    average time a packet spent in system =%s\n",str);
        printf("    No packets arrived at this facility\n");
        }
        else
        {
        double avg_time_in_sys = tot_time_in_sys;
        act_avg_time_in_sys = avg_time_in_sys /(total_pkt-1);
        printf("    average time a packet spent in system =%.6g\n",act_avg_time_in_sys/1000000);
        }
        if((total_pkt-1)==0)
        {
        printf("    standard deviation for time spent in system =%s\n",str);
        printf("    No packets arrived at this facility\n");
        }
        else
        {
        double avg_of_sqr = sqr_of_time_in_sys/(total_pkt-1);
        double sqr_of_avg = (act_avg_time_in_sys*act_avg_time_in_sys)/1000000;
        double var = avg_of_sqr + sqr_of_avg;
        double std_dev = sqrt(fabs(var));
        printf("    standard deviation for time spent in system =%.6g\n\n",std_dev/1000000);
        }
        if(token_count==0)
        {
        printf("    token drop probability  =%s\n",str);
        printf("    No tokens arrived at this facility\n");
        }
        else
        {
          double token_drop_prob = drop_token_num/token_count;
          printf("    token drop probability =%.6g\n",token_drop_prob);

        }
        if((total_pkt-1)==0)
        {
        printf("    packet drop probability =%s\n",str);
        printf("    No packets arrived at this facility\n");
        }
        else
        {
          double pkt_drop_prob = drop_pkts_num/(total_pkt-1);
          //printf("dropped packts %d\n",drop_pkts_num );
          printf("    packet drop probability =%.6g\n",pkt_drop_prob);
        }

  exit(0);
      
}

void emu_param()
{

    fprintf(stdout,"Emulation Parameters:\n");
    fprintf(stdout,"    number to arrive=%d\n",a->num);
    fprintf(stdout,"    lambda=%lf\n",a->lambda);
    fprintf(stdout,"    mu=%lf\n",a->mu);
    fprintf(stdout,"    r=%lf\n",a->r);
    fprintf(stdout,"    B=%d\n",a->b);
    fprintf(stdout,"    P=%d\n",a->p);
    fprintf(stdout,"    tsfile=%s\n",filename);
    printf("%012.3lfms: emulation begins\n",time_start);

}

int main(int argc, char *argv[])
{ 
    int i=1;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, 0);

    filename=(char*)malloc(1000*sizeof(char));
    
     a = (param *)malloc(sizeof(param));
     a -> lambda = 1;
     a -> mu=0.35;
     a -> r=1.5;
     a -> b=10;
     a -> p=3;
     a -> num=20;

    gettimeofday(&start, NULL);
    time_s=(start.tv_sec * 1000000 + start.tv_usec);
    time_start=(start.tv_sec * 1000000 + start.tv_usec)- (start.tv_sec * 1000000 + start.tv_usec);
    
    q1 = (My402List *)malloc(sizeof(My402List));
    (void)My402ListInit(q1);

    q2 = (My402List *)malloc(sizeof(My402List));
    (void)My402ListInit(q2);
//----------------------Error handling in input--------------------

if(argc==2 || argc==4 || (strncmp(argv[1],"sort",(50*sizeof(char))==0)) )
  {
   
     printf("Malformed Input\n");
      Usage();
}

/*int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}*/
//printf("here in main \n");

//-------------------For argc = 1-----------------------------------
  if(argc==1)
    {     emu_param();
          thread_create();
    }
//------------------For i<argc---------------------------------------
  while(i < argc)
  {
            if((strncmp(argv[i],"-lambda",(8*sizeof(char))))==0)
            {
              a->lambda=atof(argv[i+1]);
              //printf("lambda is %lf\n",a->lambda );
              if(argv[i+1]==NULL)
              {
                fprintf(stderr,"The value of lambda is not proper\n");
                exit(EXIT_FAILURE);
              }
             
           }
           else if((strncmp(argv[i],"-mu",(8*sizeof(char))))==0)
           {
              a->mu=atof(argv[i+1]);
              if(argv[i+1] == NULL)
              {
                fprintf(stderr,"The value of mu is not proper\n");
                exit(EXIT_FAILURE);
              }
          
           }
           else if((strncmp(argv[i],"-r",(8*sizeof(char))))==0)
             {
              a->r=atof(argv[i+1]);
              //printf("r is %lf\n",a->r);
              if(argv[i+1] == NULL)
              {
                fprintf(stderr,"The value of r is not proper\n");
                exit(EXIT_FAILURE);
              }
          
             }
           else if((strncmp(argv[i],"-B",(8*sizeof(char))))==0)
           {
            //printf("B=%d\n",a->b);
            a->b=atof(argv[i+1]);
            if(argv[i+1] == NULL)
            {
              fprintf(stderr,"The value of B is not proper\n");
              exit(EXIT_FAILURE);
            }
           }

           else if((strncmp(argv[i],"-P",(8*sizeof(char))))==0)
           {
            a->p=atof(argv[i+1]);
            if(argv[i+1] == NULL)
            {
              fprintf(stderr,"The value of P is not proper\n");
              exit(EXIT_FAILURE);
            }
    
           // break;
           }
           else if((strncmp(argv[i],"-n",(8*sizeof(char))))==0)
           {
             a->num=atof(argv[i+1]);
            if(argv[i+1] == NULL )
            {
              fprintf(stderr,"The value of n is not proper\n");
              exit(EXIT_FAILURE);
            }
               
           }  

           else if((strncmp(argv[i],"-t",(8*sizeof(char))))==0)
           {
              filename = argv[i+1];
              struct stat directory;
              stat(argv[i+1],&directory);
              if(errno==EACCES)
              {
                fprintf(stderr,"file %s cannot be opened\n",argv[i+1]);
              }
              stat(argv[i+1],&directory);
              if(S_ISDIR(directory.st_mode)){
                fprintf(stderr, "input file %s is a directory\n", argv[i+1]);
              }

              if(pthread_create(&packet_arrv_thread, NULL, pkt_arrv, (void*) filename))
              {
                fprintf(stderr, "Error creating thread\n");
                return 1;
              }

              if(pthread_create(&token_arrv_thread, NULL, token_arrv, NULL))
              {
                fprintf(stderr, "Error creating thread\n");
                return 1;
              }

              if(pthread_create(&server_arrv_thread1, NULL, server_thrd, (void*) S1))
              {
                fprintf(stderr, "Error creating thread\n");
                return 1;
              }

              if(pthread_create(&server_arrv_thread2, NULL, server_thrd, (void*) S2))
              {
                fprintf(stderr, "Error creating thread\n");
                return 1;
              }

              if(pthread_create(&cntrlc_thread, NULL, monitor, NULL))
              {
                fprintf(stderr, "Error creating thread\n");
                return 1;
              }
                         
            //break;

           }

             else 
            {
             fprintf(stderr,"Malformed Input\n");
             Usage();
            }
       
           i=i+2;
           if(i > 14)
            { 
                  Usage();
                  break;
            }      
 } 

/*if(a->num > 2147483647 || a->num <0)
{printf("n is not proper\n");
   exit(-1);}*/

/*if(a->b > 2147483647 || a->b <0)
{printf("B is not proper\n");
   exit(-1);}*/

/*if(a->p > 2147483647 || a->p <0)
{printf("P is not proper\n");
   exit(-1);} */
//printf("BEFORE CALC\n");
//----------------Calc 1/lambda--------------------------------------------
     /*   double act_one_by_lambda =(1/a->lambda)*1000;
        printf("act_one_by_lambda is %lf\n", act_one_by_lambda);

        pr_onebylambda = act_one_by_lambda*1000;
        printf("pr_onebylambda is %lf\n",pr_onebylambda );
        
        if(pr_onebylambda == 0 || pr_onebylambda < 0 )
        {fprintf(stderr,"The value of 'lambda' is not proper\n");
          printf("after Calc\n");
          exit(EXIT_FAILURE);
        }
        else if(pr_onebylambda > 10000000)
        {pr_onebylambda = 10000000; 
        }
        else
        {pr_onebylambda = pr_onebylambda;
        }  
*/
//---------------------Calc 1/mu------------------------------------

        double act_one_by_mu =(1/a->mu)*1000;

        pr_onebymu = act_one_by_mu*1000;
        if(pr_onebymu == 0 || pr_onebymu <0 )
        {
          fprintf(stderr,"The value of 'mu' is not proper\n");
          exit(EXIT_FAILURE);
        }
        else if(pr_onebymu > 10000000)
        {
          pr_onebymu = 10000000; 
        }
        else
        {
          pr_onebymu = pr_onebymu;
        }
//-----------------------------------------------------------------

if(filename[0] == '\0' && argc >1)
  {emu_param();
  thread_create();}

        if(pthread_join(packet_arrv_thread, NULL))
            {
            fprintf(stderr, "Error joining thread\n");
            return 2;
            }

        if(pthread_join(token_arrv_thread, NULL))
            {
            fprintf(stderr, "Error joining thread\n");
            return 2;
            }

        if(pthread_join(server_arrv_thread1, NULL))
            {
            fprintf(stderr, "Error joining thread\n");
            return 2;
            }
        if(pthread_join(server_arrv_thread2, NULL))
            {
            fprintf(stderr, "Error joining thread\n");
            return 2;
            }
    gettimeofday(&end, NULL);
    time_end = (end.tv_sec - start.tv_sec ) * 1000000 + ( end.tv_usec - start.tv_usec);
    printf("%012.3lfms: emulation ends\n",time_end/1000);
    stats();
    return 0;
}

