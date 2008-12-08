/*****************************************************************************
 *                                                                           *
 *       Projet       :  Bibiotheque GANIL_TAPE                              *
 *                                                                           *
 *       Programme    :  ACQ_MT_MES_FONCTIONS.h                              *
 *                                                                           *
 *       Auteur       :  OPDEBECK                                            *
 *       Date         :  16 juillet 1996                                     *
 *                      				                     *
 *       Objet        :   Ensemble de fonctions modifiables a volonte par    *
 *			l'utilisateur permettant de traiter les differents   *
 *                      buffers --> Prototypes.                              *
 *                      Necessite d'inclure GEN_TYPE.H et gan_acq_buf.h      *
 *
 *       Modifications :
 *         B. Raine 20 janvier 2003
 *         B. Raine 13 septembre 2004 acq_get_parbr_label
 *                                                                           *
 *****************************************************************************/

#ifndef __MES_FONCTIONS_H
#define __MES_FONCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif
#define PAR_NAME_LEN 20
/* Definition d'une entree parametre pour statistiques tres simples */
typedef struct Param {
  char ParName[PAR_NAME_LEN];
  int  ParRang;           /* Rang du parametre dans la liste */
  int  ParNbBits;         /* Nb de bits du parametres */
  int  ParNbCoups;        /* Nb de coups dans ce parametre */
} STAT_PARAM;

/* Pointeur sur le tableau des statistiques de paramtres cree par 
   traite_params et mis a jour par mon_traitement */
#ifndef __MES_FONCTIONS_C
extern STAT_PARAM *StatParamTable;
#else
STAT_PARAM *StatParamTable=0; 
#endif

#ifndef __MES_FONCTIONS_C
extern
#endif
int  NumParMax;  /* Numero maximum de parametre */


/* Prototypes */
int mon_traitement 	( short int * );
void AfficheStat        (void);
int traite_params	( in2p3_buffer_struct * , int);
int traite_echelles	( in2p3_buffer_struct * ); 
int acq_get_parbr_label (char *ParName, int *Label, int *NbBits);
int acq_get_parbr_label_for_ (char *ParName, int *Label, int *NbBits);



#ifdef __cplusplus
}
#endif
#endif                   
