/**********************************************************/
/* Title   : ACQ_SBUF package for GANIL Acquisition System*/
/*                                                        */
/* Name    : acq_sbuf.c                                   */
/*                                                        */
/* Author  : Frederic Saillant - GANIL                    */
/*                                                        */
/* Last update : 12 mai 1997                              */
/*                                                        */
/**********************************************************/

#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#if defined(SUN) && !defined(__VMS)
#include <sys/time.h>
#else
#include <time.h>
#endif

#if defined (VMS) || defined (__VMS) /* only on VMS */
#include <unixio.h>
#ifndef SS$_NORMAL
#include ssdef
#endif
#ifdef GR_OK
#undef GR_OK
#endif
#define GR_OK SS$_NORMAL
#endif

#include <file.h>
#include "tcp_errors.h"
#include "acq_sbuf_errors.h"
#include "vmecom_def.h"
#include "acq_codes_erreur.h"

#define TCP_READ_TIMEOUT 150        /* 150 x 200 ms = 30 secondes                  */
#define MAXEVENTBUFSIZE  16384      /* Taille max d'un buffer evenement acquisition*/
#define MSGSIZE          12         /* Taille en octets d'un message (cmd ou repon)*/

#ifndef FCODE
#define FCODE            0x02
#endif

#ifndef GCODE
#define GCODE            0x05
#endif

#define SBUFCODE         0x10       /* Code de commande config serveur buffers     */

#define OldSbufProtocol   0
#define AsyncSbufProtocol 1
#define BurstSbufProtocol 2

#define COD_OK      0
#define COD_INFO    1
#define COD_WARNING 2
#define COD_ERROR   3

/*********************/
/* Codes de commande */
/*********************/

#define CMD_HELLO 0          /* Hello : pour se dire Bonjour ...       */
#define CMD_INIT  1          /* Init  : initialiser la session         */
#define CMD_READ  2          /* Read  : demande de lecture d'un buffer */
#define CMD_BYE   0xFFFF     /* Bye   : fermer la session et Au revoir */
#define CMD_KILL  0xFFFFFFFF /* Kill  : arret du serveur, exit         */

/********************/
/* Codes de reponse */
/********************/

#define REPLY_OK     0 /* OK, commande bien traitee ...           */
#define REPLY_FAIL   1 /* Echec dans le traitement de la commande */
#define REPLY_NOREAD 2 /* Bien compris mais pas de donnees lues   */

#ifdef __STDC__

extern void lib$wait(float*);

extern int TcpOpen(int*    sid_pt,
                   char*   ip_adrs,
                   int    SourcePort,
                   int    DestPort,
                   long   timeout);

extern int TcpRead(int*    sid_pt,
                   char*   buffer,
                   int         lg_buffer,
                   int*    lg_recu_pt,
                   long   timeout);

extern int TcpWrite(int*    sid_pt,
                    char*   buffer,
                    int    lg_buffer,
                    int*    lg_ecrit_pt,
                    long   timeout);

extern int TcpClose(int*    sid_pt);

extern void SwapBytesInWords(char* Source, char* Dest, int Length);
extern void SwapBytes(char* Source, char* Dest, int Length);
extern void SwapWords(short* Source, short* Dest, int Length);

static int SendMsg(int* Canal);
static int RecvMsg(int* Canal);
static int acq_sbuf_open(int* Canal , char* Host , int Port);

int acq_sbuf_reset(int* TimeOut);
int acq_sbuf_select(char* TargetName , int* BufSize);
int acq_sbuf_read(char* Buffer , int* ReqLen , int* GotLen);
int acq_sbuf_release(int* LastBuffer);

#else

extern void lib$wait();
extern int  TcpOpen();
extern int  TcpRead();
extern int  TcpWrite();
extern int  TcpClose();
extern void SwapBytesInWords();
extern void SwapBytes();
extern void SwapWords();

static int SendMsg();
static int RecvMsg();
static int acq_sbuf_open();

int acq_sbuf_reset();
int acq_sbuf_select();
int acq_sbuf_read();
int acq_sbuf_release();

#endif

static long TcpReadTimeOut = TCP_READ_TIMEOUT;
static int  ClientSocket1 = -1;
static int  ClientSocket2 = -1;
static char Message[MSGSIZE];
static char EventBuffer[MAXEVENTBUFSIZE];
static char ServerName[30];
static int  Protocol = -1;
static int  LastBuffer_ = 0;
static int  CurrentBuffer = 0;
static int  Session = -1;
static long AdrBoiteDial;

extern int TcpWind;

#ifdef __STDC__
static int acq_sbuf_open(int* Canal , char* Host , int Port)
#else
static int acq_sbuf_open(Canal , Host , Port)
int*   Canal;
char* Host;
int    Port;
#endif
{
   int Status;

   TcpWind = 32768;

   if (*Canal != -1) TcpClose(Canal);

   *Canal = 0;

   Status = TcpOpen(Canal, Host, Port, Port, 50);

   if (Status != GR_OK) {
      printf("Erreur acq_sbuf_open - TcpOpen(%d) : $%x\n", Port, Status);

      if (Status == R_TCP_HOST) {
         printf("        --> Host '%s' inconnu\n", Host);
      }

      if ((Status == R_TCP_BIND) || (Status == R_TCP_CONNECT)) {
         if (Status == R_TCP_BIND) {
            printf("        --> bind()\n");
         } else {
            printf("        --> connect()\n");
         }

         TcpClose(Canal);
      }

      *Canal = -1;
   }

   return (Status);
}

#ifdef __STDC__
static int SendMsg(int* Canal)
#else
static int SendMsg(Canal)
int* Canal;
#endif
{
   int err;
   int Count;
   char* tmp;

#if defined(VMS) || defined(__VMS)
   char SwapBuf[MSGSIZE];

   SwapBytes(Message, SwapBuf, MSGSIZE);

   err = TcpWrite(Canal, SwapBuf, MSGSIZE, &Count, 200);
#else
   err = TcpWrite(Canal, Message, MSGSIZE, &Count, 200);
#endif

   if (err != GR_OK) {
      printf("Erreur SendMsg - TcpWrite() : $%x\n", err);
      TcpClose(Canal);
      *Canal = -1;
      return (-1);
   } else {
      return (0);
   }
}

#ifdef __STDC__
static int RecvMsg(int* Canal)
#else
static int RecvMsg(Canal)
int* Canal;
#endif
{
   int err;
   int Count;

#if defined(VMS) || defined(__VMS)
   char SwapBuf[MSGSIZE];

   err = TcpRead(Canal, SwapBuf, MSGSIZE, &Count, TcpReadTimeOut);

   SwapBytes(SwapBuf, Message, MSGSIZE);
#else
   err = TcpRead(Canal, Message, MSGSIZE, &Count, TcpReadTimeOut);
#endif

   if (err != GR_OK) {
      printf("Erreur RecvMsg - TcpRead() : $%x\n", err);
      TcpClose(Canal);
      *Canal = -1;
      return (-1);
   } else {
      return (0);
   }
}

#ifdef __STDC__
int acq_sbuf_reset(int* TimeOut)
#else
int acq_sbuf_reset(TimeOut)
int* TimeOut;
#endif
{
   TcpReadTimeOut = TCP_READ_TIMEOUT;
   TcpWind = 32768;

   if (*TimeOut > 0) TcpReadTimeOut = *TimeOut * 5;

   ClientSocket1 = -1;
   ClientSocket2 = -1;
   Session       = -1;
   LastBuffer_   =  0;

   return (GR_OK);
}

#ifdef __STDC__
int acq_sbuf_select(char* TargetName , int* BufSize)
#else
int acq_sbuf_select(TargetName , BufSize)
char* TargetName;
int*   BufSize;
#endif
{
   int   i;
   int   err;
   int   ReplyCode;
   int   Canal = -1;
   int   Count;
   int   LastBuffer;
   float delay;
   char  VMEname[20];
   char  CmdBuf[24];
   char  RepBuf[80];
   char  Commande[80];
   char* pChar;

   for (i = 0; i < 24; i++) CmdBuf[i] = 0;

   for (i = 0; i < 80; i++) RepBuf[i] = 0;

   strcpy(VMEname, TargetName);

   err = acq_sbuf_open(&Canal, VMEname, 3000);

   if (err != GR_OK) {
      return (R_SBUF_READ);
   }

   /***********************************************/
   /* On envoie au VME une commande pour demander */
   /* quel est le type de Serveur de Buffers      */
   /***********************************************/
   CmdBuf[0]  = 0x01;
   CmdBuf[1]  = 0x9E;
   CmdBuf[2]  = 0xC6;
   CmdBuf[3]  = 0xE2;

   CmdBuf[15] = SBUFCODE;

   err = TcpWrite(&Canal, CmdBuf, 24, &Count, 0);

   if (err != GR_OK) {
      printf("Erreur acq_sbuf_select - TcpWrite(3000) : $%x\n", err);
      TcpClose(&Canal);
      return (R_SBUF_READ);
   }

   delay = 0.01;
   lib$wait(&delay);

   err = TcpRead(&Canal, CmdBuf, 24, &Count, TcpReadTimeOut);

   if (err != GR_OK) {
      printf("Erreur acq_sbuf_select - TcpRead(3000) : $%x\n", err);
      TcpClose(&Canal);
      return (R_SBUF_READ);
   }

   err = TcpRead(&Canal, RepBuf, 80, &Count, TcpReadTimeOut);

   if (err != GR_OK) {
      printf("Erreur acq_sbuf_select - TcpRead(3000) : $%x\n", err);
      TcpClose(&Canal);
      return (R_SBUF_READ);
   }

   sscanf(RepBuf, "%s %d", ServerName, &Protocol);

   if (CmdBuf[15] == COD_ERROR) {
      printf("Erreur acq_sbuf_select : %s\n", RepBuf);
      TcpClose(&Canal);
      return (R_SBUF_READ);
   }

   for (i = 0; i < 24; i++) CmdBuf[i] = 0;

   for (i = 0; i < 80; i++) RepBuf[i] = 0;

   /*********************************************/
   /* On envoie au VME la commande de fermeture */
   /* du canal TCP/IP                           */
   /*********************************************/
   CmdBuf[0]  = 0x01;
   CmdBuf[1]  = 0x9E;
   CmdBuf[2]  = 0xC6;
   CmdBuf[3]  = 0xE2;

   CmdBuf[15] = FCODE;

   err = TcpWrite(&Canal, CmdBuf, 24, &Count, 0);

   if (err != GR_OK) {
      printf("Erreur acq_sbuf_select - TcpWrite(3000) : $%x\n", err);
      TcpClose(&Canal);
      return (R_SBUF_READ);
   }

   delay = 0.01;
   lib$wait(&delay);

   err = TcpRead(&Canal, CmdBuf, 24, &Count, TcpReadTimeOut);

   if (err != GR_OK) {
      printf("Erreur acq_sbuf_select - TcpRead(3000) : $%x\n", err);
      TcpClose(&Canal);
      return (R_SBUF_READ);
   }

   TcpClose(&Canal);

   if (Protocol == OldSbufProtocol) {
      err = acq_sbuf_open(&ClientSocket1, ServerName, 3000);

      if (err != GR_OK) {
         return (R_SBUF_READ);
      }

      Commande[79] = 0;

      sprintf(Commande, "SBUF RESETOPEN TCPIP");

      err = vmecom_cmd_SVCC(ClientSocket1, Commande, RepBuf, 80, &Count);

      if (err != ACQ_NEWSFROMVME) {
         TcpClose(&ClientSocket1);
         ClientSocket1 = -1;
         return (R_SBUF_READ);
      }

      Session = 0;

      if ((pChar = strstr(RepBuf, "A:")) == NULL) {
         TcpClose(&ClientSocket1);
         ClientSocket1 = -1;
         return (R_SBUF_READ);
      }

      sscanf(pChar + 2, "%lx", &AdrBoiteDial);

      sprintf(Commande, "SBUF SETUP ALL ALL %d", *BufSize);

      err = vmecom_cmd_SVCC(ClientSocket1, Commande, RepBuf, 80, &Count);

      if (err != GR_OK) {
         TcpClose(&ClientSocket1);
         ClientSocket1 = -1;
         return (R_SBUF_READ);
      }

      sprintf(Commande, "START %lx %d", AdrBoiteDial, *BufSize);

      err = vmecom(ClientSocket1, GCODE, Commande, strlen(Commande) + 1, RepBuf, &Count);

      if (err != GR_OK) {
         TcpClose(&ClientSocket1);

         ClientSocket1 = -1;

         sprintf(Commande, "SBUF CLOSE");

         ClientSocket2 = -1;

         err = acq_sbuf_open(&ClientSocket2, ServerName, 3000);

         if (err != GR_OK) {
            return (R_SBUF_READ);
         }

         err = vmecom_cmd_SVCC(ClientSocket2, Commande, RepBuf, 80, &Count);

         TcpClose(&ClientSocket2);

         ClientSocket2 = -1;

         return (R_SBUF_READ);
      }
   } else {
      err = acq_sbuf_open(&ClientSocket1, ServerName, 3001);

      if (err != GR_OK) {
         return (R_SBUF_READ);
      }

      *(unsigned int*)Message       = CMD_INIT;
      *(unsigned int*)(Message + 4) = *BufSize;

      if (SendMsg(&ClientSocket1) == -1) {
         return (R_SBUF_READ);
      }

      delay = 0.01;
      lib$wait(&delay);

      if (RecvMsg(&ClientSocket1) == -1) {
         return (R_SBUF_READ);
      }

      ReplyCode = *(unsigned int*)Message;

      if (ReplyCode != REPLY_OK) {
         TcpClose(&ClientSocket1);
         ClientSocket1 = -1;
         return (R_SBUF_READ);
      }

      if (Protocol == BurstSbufProtocol) {
         *(unsigned int*)Message = CMD_READ;

         if (SendMsg(&ClientSocket1) == -1) {
            return (R_SBUF_READ);
         }
      }
   }

   delay = 15.0;

   lib$wait(&delay);

   return (GR_OK);
}

#ifdef __STDC__
int acq_sbuf_read(char* Buffer , int* ReqLen , int* GotLen)
#else
int acq_sbuf_read(Buffer , ReqLen , GotLen)
char* Buffer;
int*   ReqLen;
int*   GotLen;
#endif
{
   int   err;
   int   Count;
   int   ReplyCode;
   float delay;
   char  Commande[80];
   char  RepBuf[80];

   *GotLen = 0;

   if (ClientSocket1 == -1) {
      printf("Appel de acq_sbuf_read() alors que le canal est ferme !\n");
      return (R_SBUF_READ);
   }

   if (Protocol == OldSbufProtocol) {
      err = TcpRead(&ClientSocket1, EventBuffer, *ReqLen, GotLen, TcpReadTimeOut);

      if (err == GR_OK) {
         if (EventBuffer[0] == '$') {
            return (R_SBUF_NOBUF);
         } else {
#if defined(VMS) || defined(__VMS)
            SwapBytesInWords(EventBuffer, Buffer, *GotLen);
#else
            SwapWords((short*)EventBuffer, (short*)Buffer, *GotLen);
#endif
            CurrentBuffer = *((int*)(Buffer + 8));

            if ((Session == -1) && (LastBuffer_ != 0) && (CurrentBuffer == LastBuffer_)) {
               vmecom(ClientSocket1, GCODE, NULL, 0, RepBuf, &Count);

               vmecom(ClientSocket1, FCODE, NULL, 0, RepBuf, &Count);

               TcpClose(&ClientSocket1);

               ClientSocket1 = -1;
            }
         }
      } else {
         *GotLen = 0;

         printf("Erreur acq_sbuf_read - TcpRead() : $%x\n", err);

         if (err == R_TCP_TIMEOUT) {
            printf("     --> time-out sur lecture du buffer de donnees (R_TCP_TIMEOUT)\n");
         } else if (err == R_TCP_READ) {
            printf("     --> erreur de lecture du buffer de donnees (R_TCP_READ)\n");
         }

         TcpClose(&ClientSocket1);

         ClientSocket1 = -1;

         Commande[79] = 0;

         sprintf(Commande, "SBUF CLOSE");

         acq_sbuf_open(&ClientSocket2, ServerName, 3000);

         vmecom_cmd_SVCC(ClientSocket2, Commande, RepBuf, 80, &Count);

         sprintf(Commande, "SBUF PURGE");

         vmecom_cmd_SVCC(ClientSocket2, Commande, RepBuf, 80, &Count);

         vmecom(ClientSocket2, FCODE, NULL, 0, RepBuf, &Count);

         TcpClose(&ClientSocket2);

         ClientSocket2 = -1;

         Session = -1;

         LastBuffer_ = 0;

         return (R_SBUF_READ);
      }
   } else {
      if (Protocol == AsyncSbufProtocol) {
         *(unsigned int*)Message = CMD_READ;

         if (SendMsg(&ClientSocket1) == -1) {
            return (R_SBUF_READ);
         }

         delay = 0.01;
         lib$wait(&delay);
      }

      if (RecvMsg(&ClientSocket1) == -1) {
         return (R_SBUF_READ);
      }

      ReplyCode = *(unsigned int*)Message;

      if (ReplyCode == REPLY_OK) {
         err = TcpRead(&ClientSocket1, EventBuffer, *ReqLen - MSGSIZE, GotLen, TcpReadTimeOut);

         if (err != GR_OK) {
            *GotLen = 0;
            printf("Erreur acq_sbuf_read - TcpRead() : $%x\n", err);
            if (err == R_TCP_TIMEOUT) {
               printf("     --> time-out sur lecture du buffer de donnees (R_TCP_TIMEOUT)\n");
            } else if (err == R_TCP_READ) {
               printf("     --> erreur de lecture du buffer de donnees (R_TCP_READ)\n");
            }
            TcpClose(&ClientSocket1);
            ClientSocket1 = -1;
            return (R_SBUF_READ);
         }
      } else if (ReplyCode == REPLY_NOREAD) {
         return (R_SBUF_NOBUF);
      } else { /* NE DEVRAIT JAMAIS ARRIVER ! */
         printf("Erreur du Serveur de Buffers du frontal !\n");
         TcpClose(&ClientSocket1);
         ClientSocket1 = -1;
         return (R_SBUF_READ);
      }

#if defined(VMS) || defined(__VMS)
      SwapBytesInWords(EventBuffer, Buffer, *GotLen);
#else
      SwapWords((short*)EventBuffer, (short*)Buffer, *GotLen);
#endif

      *GotLen = *GotLen + MSGSIZE;
   }

   return (GR_OK);
}

#ifdef __STDC__
int acq_sbuf_release(int* LastBuffer)
#else
int acq_sbuf_release(LastBuffer)
int* LastBuffer;
#endif
{
   int    err;
   int    Count;
   char* pChar;
   float  delay;
   char   Commande[80];
   char   RepBuf[80];

   *LastBuffer = 0;

   if (ClientSocket1 == -1) {
      return (GR_OK);
   }

   if (Protocol == OldSbufProtocol) {
      if (Session == -1) {
         return (GR_OK);
      }

      sprintf(Commande, "SBUF CLOSE");

      ClientSocket2 = -1;

      err = acq_sbuf_open(&ClientSocket2, ServerName, 3000);

      if (err == GR_OK) {
         err = vmecom_cmd_SVCC(ClientSocket2, Commande, RepBuf, 80, &Count);

         vmecom(ClientSocket2, FCODE, NULL, 0, RepBuf, &Count);

         TcpClose(&ClientSocket2);

         ClientSocket2 = -1;

         if (err == ACQ_NEWSFROMVME) {
            if ((pChar = strstr(RepBuf, "N:")) == NULL) {
               err = R_SBUF_READ;
            } else {
               err = GR_OK;
               sscanf(pChar + 2, "%x", LastBuffer);
               Session = -1;
            }
         } else {
            err = R_SBUF_READ;
         }
      }

      LastBuffer_ = *LastBuffer;

      if ((err != GR_OK) || (*LastBuffer == CurrentBuffer)) {
         vmecom(ClientSocket1, GCODE, NULL, 0, RepBuf, &Count);

         vmecom(ClientSocket1, FCODE, NULL, 0, RepBuf, &Count);

         TcpClose(&ClientSocket1);

         ClientSocket1 = -1;

         if (err != GR_OK) {
            return (R_SBUF_READ);
         }
      }
   } else if (Protocol == BurstSbufProtocol) {
      err = acq_sbuf_open(&ClientSocket2, ServerName, 3001);

      if (err != GR_OK) {
         delay = 1.0;
         lib$wait(&delay);
         TcpClose(&ClientSocket1);
         ClientSocket1 = -1;
         return (R_SBUF_READ);
      }

      *(unsigned int*)Message = CMD_BYE;

      if (SendMsg(&ClientSocket2) == -1) {
         return (R_SBUF_READ);
      }

      delay = 0.01;
      lib$wait(&delay);

      if (RecvMsg(&ClientSocket2) == -1) {
         return (R_SBUF_READ);
      }

      TcpClose(&ClientSocket2);

      ClientSocket2 = -1;

      delay = 1.0;

      lib$wait(&delay);

      TcpClose(&ClientSocket1);

      ClientSocket1 = -1;
   } else { /* if ( Protocol == AsyncSbufProtocol ) */
      *(unsigned int*)Message = CMD_BYE;

      if (SendMsg(&ClientSocket1) == -1) {
         return (R_SBUF_READ);
      }

      delay = 0.01;
      lib$wait(&delay);

      if (RecvMsg(&ClientSocket1) == -1) {
         return (R_SBUF_READ);
      }

      TcpClose(&ClientSocket1);

      ClientSocket1 = -1;
   }

   return (GR_OK);
}
