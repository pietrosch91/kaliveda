/*****************************************************************************
 *                                                                           *
 *       Projet       :  Bibiotheque GANIL_TAPE                              *
 *                                                                           *
 *       Programme    :  ACQ_MT_CTRL_EVENT.c                                    *
 *                                                                           *
 *       Auteur       :  OPDEBECK                                            *
 *       Date         :  16 juillet 1996                                     *
 *                                                       *
 *                                                                           *
 *       Objet        :  Fonctions de lecture de buffers sur bande pour      *
 *       l'utilitaire EXEMPLE_RELECTURE sous VMS et sous UNIX *
 *                                                                           *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "acq_mt_ctrl_event.h"


#if defined ( __VMS ) || defined ( VMS ) /*******************************/

#include "acq_codes_erreur.h"

#endif  /****************************************************************/


#if defined ( __unix__ ) || (__unix)  /************************************/

#include "def_err.h"

#endif /*****************************************************************/



/******************************************************************************
   Fonctions de traitement ....
 ******************************************************************************/



