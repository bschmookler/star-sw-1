/*:Copyright 1996, Lawrence Berkeley National Laboratory
*:>---------------------------------------------------------------------
*:FILE:         stafGen.c
*:DESCRIPTION:  Program to generate STAF mainline.
*:AUTHOR:       cet - Craig E. Tull, cetull@lbl.gov
*:BUGS:         -- STILL IN DEVELOPMENT --
*:HISTORY:      19mar96-v000a-cet- creation
*:<---------------------------------------------------------------------
*/

/*-------------------------------------------- MACROS               --*/
#define TRUE 1
#define FALSE 0
/*-------------------------------------------- INCLUDES             --*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*-------------------------------------------- TYPEDEFS             --*/
/*-------------------------------------------- GLOBALS              --*/
int nasp=0;
char *asp[64];
char *ASP[64];
int npam=0;
char *pam[64];
char *PAM[64];

int ami=FALSE;			/* AMI is not defined */

unsigned char paw=FALSE;	/* PAW */

/*-------------------------------------------- PROTOTYPES           --*/
char* locase(char* s);
char* hicase(char* s);
void usage(char* prog);
char* asp_includes();
char* pam_includes();
char* ami_load_proto();
char* asp_starts();
char* asp_stops();
char* ami_load_func();
char* cern_init_call();

/*
*:>---------------------------------------------------------------------
*:ROUTINE:      main
*:DESCRIPTION:  program mainline
*:ARGUMENTS:    -- NONE --
*:RETURN VALUE: -- NONE --
*:<---------------------------------------------------------------------
*/
int main(int argc, char **argv)
{
   time_t it;
   int i;
   char ap='a';			/* ASP or PAM */
   unsigned char asu=FALSE;	/* core ASP */
   unsigned char soc=FALSE;	/* core ASP */
   unsigned char eml=FALSE;	/* core ASP */

   if (argc < 1) usage(argv[0]);
/*- Process arguments. -*/
   for( i=1;i<argc;i++ ){
      switch (argv[i][0]) {
      case '-':
	 switch (argv[i][1]) {
	 case 'a': case 'A':
	 case 'p': case 'P':
	    ap = argv[i][1];
	    break;
	 case 'w': case 'W':
	    paw = TRUE;
	    break;
	 default:
	    usage(argv[0]);
	    break;
	 }
	 break;
      default:
	 switch (ap) {
	 case 'a': case 'A':
	    asp[nasp] = (char*)locase(argv[i]);
	    if(0 == strcmp(asp[nasp],"ami"))ami=TRUE;
	    ASP[nasp++] = (char*)hicase(argv[i]);
	    break;
	 case 'p': case 'P':
	    pam[npam] = (char*)locase(argv[i]);
	    PAM[npam++] = (char*)hicase(argv[i]);
	    break;
	 }
	 break;
      }
   }

/*- Add core ASPs. -*/
   for( i=0;i<nasp;i++ ){
      if( 0 == strcmp("eml",asp[i]) )eml = TRUE;
      if( 0 == strcmp("soc",asp[i]) )soc = TRUE;
      if( 0 == strcmp("asu",asp[i]) )asu = TRUE;
   }
   if(!eml){asp[nasp]="eml";ASP[nasp++]="EML";}
   if(!soc){asp[nasp]="soc";ASP[nasp++]="SOC";}
   if(!asu){asp[nasp]="asu";ASP[nasp++]="ASU";}

/*= PRINT ============================================================*/

   printf(
"/*********************************** \n"
"** %s"
"** Automatically generated by %s \n"
"** DO NOT EDIT THIS FILE \n"
"***********************************/ \n"
" \n"
"/*------------------ INCLUDES ----*/ \n"
"#include <stream.h> \n"
"#include <stdlib.h> \n"
"#include <string.h> \n"
"#include \"kuip.h\" \n"
" \n"
"/*------------------ ASPS --------*/ \n"
"%s"						/* asp_includes() */
" \n"
"/*------------------ PAMS --------*/ \n"
"%s"						/* pam_includes() */
" \n"
"/*------------------ PROTOTYPES --*/ \n"
"%s"						/* ami_load_proto() */
"extern CC_P void unknown(); \n"
"extern CC_P int stafArgs(int argc, char **argv); \n"
"#ifdef TNT \n"
"extern CC_P void staf_paw_init_();  \n"
"#else \n"
"extern CC_P void staf_kuip_init_();  \n"
"#endif \n"
"extern CC_P void staf_banner(FILE* stream); \n"
" \n"
"/*================== MAIN ========*/ \n"
"int main(int argc, char** argv) \n"
"{ \n"
"   stafArgs(argc,argv); \n"
"#ifdef TNT \n"
"   staf_paw_init_();  \n"
"#else \n"
"   staf_kuip_init_();  \n"
"#endif \n"
" \n"
"/*------------------ START -------*/ \n"
"%s"						/* asp_starts() */
" \n"
"/*------------------ INPUT -------*/ \n"
"   staf_banner(stdout); \n"
"   printf(\"STAF - ready for user input.\\n\"); \n"
"   ku_what(unknown); \n"
"   printf(\"STAF - done with user input.\\n\"); \n"
" \n"
"/*------------------ STOP --------*/ \n"
"%s"						/* asp_stops() */
" \n"
"   return 1; \n"
"} \n"
" \n"
"/*================== UNKNOWN =====*/ \n"
"void unknown(){} \n"
" \n"
"/*================== AMI_LOAD ====*/ \n"
"%s"						/* ami_load_func() */
" \n"
	,ctime(&(it = time(0)))
	,argv[0]
	,asp_includes()
	,pam_includes()
	,ami_load_proto()
	,asp_starts()
	,asp_stops()
	,ami_load_func()
   );

/*====================================================================*/

   return 1;
}

/*
*:>---------------------------------------------------------------------
*:ROUTINE:      void usage
*:DESCRIPTION:  Shows usage of main.
*:ARGUMENTS:    char* prog      = Program name.
*:RETURN VALUE: *** KILLS PROGRAM ***
*:<---------------------------------------------------------------------
*/

void usage(char* prog)
{
   printf("Usage: %s [-a asps] [-p pams] \n",prog);
   printf("Example: %s -a ami tbr dio dui tdm spx soc asu -p pam \n"
		,prog);
   exit(0);
}

/*====================================================================*/
char* asp_includes()
{
   int i;
   char *s=(char*)malloc(1024);
   s[0]=NULL;
   for( i=nasp-1;i>=0;i-- ){
      sprintf(s+strlen(s),
"#include \"%sLib.h\" \n"
      ,asp[i]);
   }
   s[strlen(s)]=NULL;
   return s;
}
/*--------------------------------------------------------------------*/
char* pam_includes()
{
   int i;
   char *s=(char*)malloc(1024);
   s[0]=NULL;
   for( i=0;i<npam;i++ ){
      sprintf(s+strlen(s),
"extern \"C\" int %s_init(), %s_start(), %s_stop(); \n"
      ,pam[i] ,pam[i] ,pam[i]);
   }
   s[strlen(s)]=NULL;
   return s;
}

/*--------------------------------------------------------------------*/
char* ami_load_proto()
{
   if(ami){
      return "extern CC_P int ami_load(amiBroker *broker); \n";
   }
   else {
      return "//- ami ASP not included \n";
   }
}

/*--------------------------------------------------------------------*/
char* asp_starts()
{
   int i;
   char *s=(char*)malloc(1024);
   s[0]=NULL;
   for( i=nasp-1;i>=0;i-- ){
      sprintf(s+strlen(s),
"#ifdef %s \n"
"   %s_init(); %s_start(); \n"
"#endif /* %s */ \n"
" \n"
      ,ASP[i],asp[i],asp[i],ASP[i]);
   }
   s[strlen(s)]=NULL;
   return s;
}

/*--------------------------------------------------------------------*/
char* asp_stops()
{
   int i;
   char *s=(char*)malloc(1024);
   s[0]=NULL;
   for( i=0;i<nasp;i++ ){
      sprintf(s+strlen(s),
"#ifdef %s \n"
"   %s_stop(); \n"
"#endif /* %s */ \n"
" \n"
      ,ASP[i],asp[i],ASP[i]);
   }
   s[strlen(s)]=NULL;
   return s;
}

/*--------------------------------------------------------------------*/
char* ami_load_func()
{
   int i;
   char *s=(char*)malloc(1024);
   if(!ami)return "//- ami ASP not included \n";
   s[0]=NULL;
   sprintf(s,
"int ami_load(amiBroker *broker) \n"
"{ \n"
   );
   for( i=npam-1;i>=0;i-- ){
      sprintf(s+strlen(s),
"\t%s_init(); %s_start(); \n"
      ,pam[i] ,pam[i]);
   }
   sprintf(s+strlen(s),
" \n"
"   return TRUE; \n"
"} \n"
   );
   s[strlen(s)]=NULL;
   return s;
}

/*--------------------------------------------------------------------*/
char* locase(char *s)
{
   char *ss=(char*)malloc(strlen(s)+1);
   for(int i=0;i<strlen(s);i++){
      if('A'<=s[i] && s[i]<='Z'){
	 ss[i] = s[i] + 'a'-'A';
      }
      else {
	 ss[i] = s[i];
      }
   }
   ss[strlen(s)]=NULL;
   return ss;
}

/*--------------------------------------------------------------------*/
char* hicase(char *s)
{
   char *ss=(char*)malloc(strlen(s)+1);
   for(int i=0;i<strlen(s);i++){
      if('a'<=s[i] && s[i]<='z'){
	 ss[i] = s[i] + 'A'-'a';
      }
      else {
	 ss[i] = s[i];
      }
   }
   ss[strlen(s)]=NULL;
   return ss;
}

/*--------------------------------------------------------------------*/
char* cern_init_call()
{
   if(paw){
      return "staf_paw_init_()";
   }
   else {
      return "staf_kuip_init_()";
   }
}

/*====================================================================*/

