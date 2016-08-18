/******************************************************************************/

/* Important CSCI 402 usage information:                                      */

/*                                                                            */

/* This fils is part of CSCI 402 warmup programming assignments at USC.       */

/* Please understand that you are NOT permitted to distribute or publically   */

/*         display a copy of this file (or ANY PART of it) for any reason.    */

/* If anyone (including your prospective employer) asks you to post the code, */

/*         you must inform them that you do NOT have permissions to do so.    */

/* You are also NOT permitted to remove or alter this comment block.          */

/* If this comment block is removed or altered in a submitted file, 20 points */

/*         will be deducted.                                                  */

/******************************************************************************/



#include <stdio.h>

#include <math.h>

#include <stdlib.h>

#include <ctype.h>

#include <time.h>

#include <string.h>

#include <locale.h>

#include <unistd.h>

#include <sys/stat.h>

#include <sys/types.h>

#include "my402list.h"

#include "cs402.h"



typedef struct tfile_list_t

{



   char *tran_type;

   char *tran_time;

   char *tran_amount;

   char *tran_desc;

  

}tfile_list;


static

void Usage()

{

    fprintf(stderr,"Usage: warmup1 sort [tfile]\n");

    exit(-1);

}



int parse_line(char* line, My402List* list);

void BubbleSortForwardList(My402List *pList, int num_items);

void printlist(My402List *list);

void output_time(char * tran_time,char *buf);

void BubbleForward(My402List *pList, My402ListElem **pp_elem1, My402ListElem **pp_elem2);

char * tran_desc_trim(char *str);

char* comma_dots(float i);



int main(int argc,char *argv[])

{

    FILE *fp;

    char line[2000];

    int num_mem;

    My402List list;

    memset(&list, 0, sizeof(My402List));

    (void)My402ListInit(&list); 
    

    if(argc==1)

    {

        printf("malformed command\n");

                exit(EXIT_FAILURE);

    }   



    if(argc==4)

    {

        printf("malformed command\n");

                exit(EXIT_FAILURE);

    }

    if(strlen(line)>1024)
    {
        printf("Length of line is more than 1024\n");
        exit(EXIT_FAILURE);

    }



    if(argc == 3 && (strncmp(argv[1],"sort",strlen(argv[1]))==0))

    {

    fp = fopen(argv[2], "r");

       

    if (fp==NULL)

    {

        fprintf(stderr, "Unable to open '%s'\n",argv[3]);

                exit(EXIT_FAILURE);

    }

    while (fgets(line,2000, fp)!=NULL)

    {
        //printf("%s\n",line);
        parse_line(line, &list);

    } 

     fclose(fp);

    }
    else if(argc == 2 && (strncmp(argv[1],"sort",strlen(argv[1]))==0))

    {

         fp =stdin;

    if (fp==NULL)

        {

        fprintf(stderr, "Unable to open '%s'\n",argv[3]);

                exit(EXIT_FAILURE);

        }

   

    while (fgets(line,2000, fp)!=NULL)

    {

      
         parse_line(line, &list);

               

    }
  fclose(fp);
}
    
else 
  {

    printf("Malformed Command");
    Usage();

  }  

    num_mem=My402ListLength(&list);

     BubbleSortForwardList(&list,num_mem);

   

    printlist(&list);

   

    return 0;

}       



int parse_line(char* line, My402List* list)

{



   char *tran_type_str;

   char *tran_time_str;

   char *tran_amount_str;

   char *tran_desc_str;

   char *tab_ptr;

   

   const char s[2] = "\t";

   char *token;
   

   int len;

   char* ptr;
   ptr=(char*)malloc(11*sizeof(char));

   char *str;
   str=(char*)malloc(25*sizeof(char));
   


   tfile_list *q = (tfile_list *)malloc(sizeof(tfile_list));

  
  
   token=(char*)malloc(strlen(line));
   token = strtok(line, s);

  

      /*Checking transaction type format*/

      tran_type_str=token;

      tran_type_str = (char *)malloc(strlen(token));

      strcpy(tran_type_str, token);

     

      {

              //printf("trsn typr = %smmmmmmmmmmmmmmmmmmmm", tran_type_str);

              if ((strcmp(tran_type_str,"-")==0) || (strcmp(tran_type_str,"+")==0))

                  {

                q -> tran_type = (char*) malloc(strlen(tran_type_str));

                strncpy(q -> tran_type,tran_type_str,2);

                      //printf( "%s\n", tran_type_str );

                  }

              else

                  {

                      printf("Transaction type is not in proper format\n");
                      printf("error in line %s\n",line);
                      exit(EXIT_FAILURE);

                  }

      }

     

      /*Checking transaction time format*/

      token = strtok(NULL, s);

      tran_time_str = (char *)malloc(strlen(token));

      strcpy(tran_time_str, token);

      {

              len=strlen(tran_time_str);

              //printf("Size of time field=%d\n",len);

              if (len<11)

                  {

                q -> tran_time = (char*) malloc(strlen(tran_time_str));

                //output_time(tran_time_str);

                strncpy(q -> tran_time,tran_time_str,11);

                //printf("Time:%s\n",tran_time_str);

                //output_time(tran_time_str);

               

               

                  }

              else

                  {

                printf("Transaction time is not in proper format\n");

                exit(EXIT_FAILURE);

                  }



     

      }

      

      /*Checking transaction amount format*/

      token = strtok(NULL, s);

      tran_amount_str = (char *)malloc(strlen(token));

      strcpy(tran_amount_str, token);

     

      // {

     

              //char* pl = strchr(tran_amount_str, '.');

              // double right_num = strtod(pl+1, &ptr);

              double left_num = strtod(tran_amount_str, &ptr); 

       

        if ((int)left_num<10000000)

        {

       

            q -> tran_amount = (char*) malloc(strlen(tran_amount_str));

            strncpy(q -> tran_amount,tran_amount_str,13);

           

               

        }

        else

        {

            printf("Transaction amount is not in proper format\n");

            exit(EXIT_FAILURE);



        }



        

      /*Checking transaction description format*/

      token = strtok(NULL, s);

      tran_desc_str = (char *)malloc(strlen(token));

      strcpy(tran_desc_str, token);



     if (strlen(tran_desc_str) == 0)

           {

        printf("Transaction description cannot be empty\n");

        exit(EXIT_FAILURE);

          }

     else {

        q -> tran_desc = (char*) malloc(strlen(tran_desc_str));
        /*if((tmp1!=My402ListLast(list)))
        {*/

        
        /*}*/

        str=tran_desc_trim(tran_desc_str);

        //printf("Trimmed string=%s\n",str);

        //tran_desc_trim(tran_desc_str);
        tab_ptr = (char *)malloc(strlen(tran_desc_str));   
        tab_ptr = strchr(tran_desc_str,'\n');
        if(tab_ptr != NULL)
        *tab_ptr++= '\0';

        strncpy(q -> tran_desc,tran_desc_str,24);
        //printf("tttttt=%s\n",q -> tran_desc);
              

    }
 

     My402ListAppend(list,(void*)q);

 

     return(0);

}



void printlist(My402List *list)

{

    My402ListElem* tmp1;

    tfile_list* tmp2;

    char *buf;    

    tmp1=My402ListFirst(list);

    tmp2=(tfile_list*)tmp1->obj;

    //char* t;

    buf = (char *)malloc(20*sizeof(char));

    char flaw[]="?,???,???.??";

    int total=0;

    double f_total;
    double f_tot;

    char *str;
    str=(char*)malloc(sizeof(str));

   

    //t = (char*) malloc(20*sizeof(char));



       

    printf("12345678901234567890123456789012345678901234567890123456789012345678901234567890\n");
    printf("+-----------------+--------------------------+----------------+----------------+\n");
    printf("|       Date      | Description              |         Amount |        Balance |\n");
    printf("+-----------------+--------------------------+----------------+----------------+\n");

    
    while ((tmp1!=My402ListLast(list)) | (tmp1==My402ListLast(list)))

    {



   output_time(tmp2->tran_time,buf);

   //float i=atof(tmp2->tran_amount);
   float i=atof(tmp2->tran_amount);
   //printf("i value is   =%lf\n",i);

    double convert_factor=100.00;

    int cent=round(i*convert_factor);
    //printf("cent is=%d\n",cent);

    if (strncmp(tmp2->tran_type,"-",2)==0)

    {

    total=total-cent;

    //printf("Balance=%s\n",total);

    }

    else

    {

    total=total+cent;

    //printf("Balance=%s\n",total);

    }

   

    //printf("Total=%d\n",total);   

    f_total=(double)total/100;
    if(f_total<0)
    {
      f_tot=-f_total;
      //printf("f_total=%f\n",f_total);

    }

    //printf("Final_total=%lf\n",f_total);


    str=comma_dots(i);



    if ((strncmp(tmp2->tran_type,"-",2)==0) && (atof)(tmp2->tran_amount)<10000000 && (-10000000<f_total) && (f_total<0))
    {
    
            fprintf(stdout,"| %14s | %-24s | (%11s)  | (%12.2f) |\n",buf,tmp2->tran_desc,str,f_tot);
    }
    
    else if ((strncmp(tmp2->tran_type,"-",2)==0) && (atof)(tmp2->tran_amount)<10000000 && (0<f_total) && (f_total<10000000))
    {
            fprintf(stdout,"| %14s | %-24s | (%11s)  | %14.2f |\n",buf,tmp2->tran_desc,str,f_total);
    }

    else if ((strncmp(tmp2->tran_type,"-",2)==0) && (atof)(tmp2->tran_amount)<10000000 && (f_total>10000000))
    {
            fprintf(stdout,"| %14s | %-24s | (%11s)  | %13.2s |\n",buf,tmp2->tran_desc,str,flaw);
    }

    else if ((strncmp(tmp2->tran_type,"-",2)==0) && (atof)(tmp2->tran_amount)<10000000 && (f_total<=-10000000))
    {
            fprintf(stdout,"| %14s | %-24s | (%11s)  | (%12.2s) |\n",buf,tmp2->tran_desc,str,flaw);
    }
    
    else if ((strncmp(tmp2->tran_type,"-",2)==0) && (atof)(tmp2->tran_amount)>=10000000 && (0<f_total) && (f_total<10000000))
    {

    fprintf(stdout,"| %14s | %-24s | (%11s)  | %14.2f |\n",buf,tmp2->tran_desc,flaw,f_total);

    }

    else if ((strncmp(tmp2->tran_type,"-",2)==0) && (atof)(tmp2->tran_amount)>=10000000 && (f_total>=10000000))
    {

    fprintf(stdout,"| %14s | %-24s | (%11s)  | %13.2s |\n",buf,tmp2->tran_desc,flaw,flaw);

    }

    else if ((strncmp(tmp2->tran_type,"-",2)==0) && (atof)(tmp2->tran_amount)>=10000000 && (-10000000<f_total)&& (f_total<0))
    {

    fprintf(stdout,"| %14s | %-24s | (%11s)  | (%12.2f) |\n",buf,tmp2->tran_desc,flaw,f_tot);

    }

    else if ((strncmp(tmp2->tran_type,"-",2)==0) && (atof)(tmp2->tran_amount)>=10000000 && (f_total<-10000000))
    {

    fprintf(stdout,"| %14s | %-24s | (%11s)  | (%12.2s) |\n",buf,tmp2->tran_desc,flaw,flaw);

    }

    else if ((strncmp(tmp2->tran_type,"+",2)==0) && (atof)(tmp2->tran_amount)<10000000 && (0<f_total) && (f_total<10000000))

    {

    fprintf(stdout,"| %14s | %-24s | %13s  | %14.2f |\n",buf,tmp2->tran_desc,str,f_total);

    }

    else if ((strncmp(tmp2->tran_type,"+",2)==0) && (atof)(tmp2->tran_amount)<10000000 && (f_total>=10000000))

    {

    fprintf(stdout,"| %14s | %-24s | %13s  | %13.2s |\n",buf,tmp2->tran_desc,str,flaw);

    }

     else if ((strncmp(tmp2->tran_type,"+",2)==0) && (atof)(tmp2->tran_amount)<10000000 && (-10000000<f_total) && (f_total<0))

    {

    fprintf(stdout,"| %14s | %-24s | %13s  | (%12.2f) |\n",buf,tmp2->tran_desc,str,f_tot);

    }

     else if ((strncmp(tmp2->tran_type,"+",2)==0) && (atof)(tmp2->tran_amount)<10000000 && (f_total<=-10000000))

    {

    fprintf(stdout,"| %14s | %-24s | %13s  | (%12.2s) |\n",buf,tmp2->tran_desc,str,flaw);

    }
 
    else if ((strncmp(tmp2->tran_type,"+",2)==0) && (atof)(tmp2->tran_amount)>10000000 && (0<f_total) && (f_total<10000000))

    {

    fprintf(stdout,"| %14s | %-24s | %13s  | %14.2f |\n",buf,tmp2->tran_desc,flaw,f_total);
    }

    else if ((strncmp(tmp2->tran_type,"+",2)==0) && (atof)(tmp2->tran_amount)>10000000 && (f_total>=10000000))

    {

    fprintf(stdout,"| %14s | %-24s | %13s  | %13.2s |\n",buf,tmp2->tran_desc,flaw,flaw);

    }

    else if ((strncmp(tmp2->tran_type,"+",2)==0) && (atof)(tmp2->tran_amount)>10000000 && (-1000000<f_total) && (f_total<0))

    {

    fprintf(stdout,"| %14s | %-24s | %13s  | (%12.2f) |\n",buf,tmp2->tran_desc,flaw,f_tot);

    }

    else 

    {

    fprintf(stdout,"| %14s | %-24s  | %13s  | (%12.2s) |\n",buf,tmp2->tran_desc,flaw,flaw);

    }

   
    if (tmp1!=My402ListLast(list)){

    tmp1 = My402ListNext(list,tmp1);

    tmp2 = (tfile_list*)tmp1->obj;}

    else

    break;

       



    }

     //printf("Total=%d\n",total);

    f_total=(float)total/100;

    //printf("Final_total=%f\n",f_total);

    //f_t=f_total/1000000;

   


 

      printf("+-----------------+--------------------------+----------------+----------------+\n");



 

}



void BubbleSortForwardList(My402List *pList, int num_items)

{

        My402ListElem *elem=NULL;   
        int i=0;  
        tfile_list* tmp1;

        if (My402ListLength(pList) != num_items) {
            fprintf(stderr, "List length is not %1d in BubbleSortForwardList().\n", num_items);
            exit(1);

        }

        for (i=0; i < num_items; i++) {
            int j=0, something_swapped=FALSE;
            My402ListElem *next_elem=NULL;

            for (elem=My402ListFirst(pList), j=0; j < num_items-i-1; elem=next_elem, j++) {

                tmp1 = (tfile_list*) elem -> obj;
                long cur_val=/*(long)*/atol(tmp1->tran_time);
                long next_val=0;

                next_elem=My402ListNext(pList, elem);
                tfile_list* tmp2 = (tfile_list*) next_elem -> obj;



                next_val = /*(long)*/atol(tmp2->tran_time);





                if (cur_val > next_val) {

                    BubbleForward(pList, &elem, &next_elem);

                    something_swapped = TRUE;

                }

            }

            if (!something_swapped) break;

        }

}



void BubbleForward(My402List *pList, My402ListElem **pp_elem1, My402ListElem **pp_elem2)

    /* (*pp_elem1) must be closer to First() than (*pp_elem2) */

{

    My402ListElem *elem1=(*pp_elem1), *elem2=(*pp_elem2);

    void *obj1=elem1->obj, *obj2=elem2->obj;

    My402ListElem *elem1prev=My402ListPrev(pList, elem1);

/*  My402ListElem *elem1next=My402ListNext(pList, elem1); */

/*  My402ListElem *elem2prev=My402ListPrev(pList, elem2); */

    My402ListElem *elem2next=My402ListNext(pList, elem2);



    My402ListUnlink(pList, elem1);

    My402ListUnlink(pList, elem2);

    if (elem1prev == NULL) {

        (void)My402ListPrepend(pList, obj2);

        *pp_elem1 = My402ListFirst(pList);

    } else {

        (void)My402ListInsertAfter(pList, obj2, elem1prev);

        *pp_elem1 = My402ListNext(pList, elem1prev);

    }

    if (elem2next == NULL) {

        (void)My402ListAppend(pList, obj1);

        *pp_elem2 = My402ListLast(pList);

    } else {

        (void)My402ListInsertBefore(pList, obj1, elem2next);

        *pp_elem2 = My402ListPrev(pList, elem2next);

    }

}







void output_time(char * tran_time, char *buf)

{

    setlocale(LC_ALL,"en_CA.UTF-8");

    time_t sec = atol(tran_time);

    struct tm *timeinfo;

    timeinfo = localtime(&sec);

   

    strftime(buf,25,"%a %b %e %Y",timeinfo);



}





char * tran_desc_trim(char *str)

{

while (isspace(*str))
    str++;

  if(*str =='\0')
    printf("Transaction Description not in proper format\n");

  //printf("%s\n",str);

  return str;
  
}



char* comma_dots(float i)
{

char *str;
str=(char*)malloc(sizeof(str));
//double f_total;
                          
/* This is the amount value*/


//for right hand side of the decimal point

double convert_factor=100.00;
int cent = round(i*convert_factor);
//printf("cent value=%d\n", cent);
int rt_prt = cent%100; 
//printf("rt_prt value=%d\n", rt_prt);

int amt_int;
amt_int = cent/100;           /* Has the digits after decimal point*/
//printf("amt_int=%d\n", amt_int);

// for left hand side of the decimal point
if(amt_int<1000)
{

sprintf(str,"%d%c%.2d",amt_int,'.',rt_prt);

}

else if (amt_int<1000000)
{
 
int a_t=amt_int/1000;
//printf("a_t=%d\n", a_t);
int b_t=amt_int%1000;
//printf("b_t=%d\n", b_t);


sprintf(str,"%d%c%.3d%c%.2d",a_t,',',b_t,'.',rt_prt);

}

else /*if(amt_int<10000000)*/
{

int c_amt=amt_int/1000000;
amt_int = amt_int%1000000;
int d_amt=amt_int/1000;
int e_amt=amt_int%1000;

sprintf(str,"%d%c%.3d%c%.3d%c%.2d",c_amt,',',d_amt,',',e_amt,'.',rt_prt);

}

return str;


}