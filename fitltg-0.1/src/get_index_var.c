#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#endif
/*
 *          *******************************************************
 *          *     OBTENTION DE L'ADRESSE D'UNE VARIABLE OU        *
 *          *  TABLEAU D'ENTIERS OU DE REELS OU D'UNE ROUTINE     *
 *          *  SOUS FORME D'UN INDEX D'UN TABLEAU DE REFERENCE    *
 *          *******************************************************
 *
 *      call get_index_var(var,ref,isize_ref,index)
 *
 *   ENTREE :
 *    var : [I] ou [R] ou [External] variable ou tableau ou routine
 *                  dont on veut l'index
 *    ref : [I*(*)] ou [R*(*)] : tableau servant de reference
 * isize_ref : [I] taille en octets de chaque element de ref
 * 
 *   RETOUR :
 *    index : [I] index : ref(index) a la meme adresse que var
 *                        index=0 si defaut d'alignement
 */
void F77_FUNC_(get_index_var,GET_INDEX_VAR)(char *var, char *ref, int *size,
                                          int *index)
{
  int64_t  i, imin, imax ;
  
  i = (int64_t)(var-ref) ;
  imin = INT_MIN ;
  imax = INT_MAX ;
  
  if ((i % *size) != 0 || i < imin || i > imax) *index = 0 ; 
  else *index = i/ *size+1 ;
  return ;
}
