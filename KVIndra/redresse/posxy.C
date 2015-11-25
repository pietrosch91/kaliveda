// En entrée: le numéro du télescope de blocking touché (1, 2 ou 3)   (int)
//            le paramètre énergie en canaux (Double_t)
//              les 4 paramètres de position (Double_t)
// En sortie: les positions x et y en mm    (Double_t)

//            les positions x et y de valeurs absolues sup à 19mm doivent être
//            utilisées avec modération et circonspection
// x ou y = -999. en cas de rejet



// Fonction de redressement pour E416a
void red(int tel, Double_t xpos, Double_t ypos, Double_t& xred, Double_t& yred)
{
   Double_t static matcor_X[3] [512] [512];
   Double_t static matcor_Y[3] [512] [512];
   int static prem;
   if (prem != 5) {
      cout << "initialisation" << endl;
      cout << "ATTENTION:" <<
           " le numero du telescope pour posxy doit etre entre 1 et3"
           << endl;
      for (int itel = 1; itel < 4; itel++) {
         ifstream inmatX;
         inmatX.open(Form("matrice3_X%d.dat", itel));
         if (! inmatX.is_open()) {
            cout << " !!!! " << Form("matrice3_X%d.dat", itel) << "non trouvé:" << endl;
            return;
         }
         ifstream inmatY;
         inmatY.open(Form("matrice3_Y%d.dat", itel));
         if (! inmatY.is_open()) {
            cout << " !!!! " << Form("matrice3_Y%d.dat", itel) << "non trouvé:" << endl;
            return;
         }
         for (int i = 0; i < 512; i++) {
            for (int j = 0; j < 512; j++) {
               inmatX >> matcor_X[itel - 1][i][j];
               inmatY >> matcor_Y[itel - 1][i][j];
            }
         }
         inmatX.close();
         inmatY.close();
      }
      prem = 5;
   }
   int indix = int ((xpos + 1.) * 256. + 0.5);
   int indiy = int ((ypos + 1.) * 256. + 0.5);
   if (indix < 0 || indix > 511 || indiy < 0 || indiy > 511) {
      cout << "!!!!!!!!!!! Indice matrice de redressement anormal "
           << indix << " " << indiy << " " <<  xpos << " " <<  ypos << endl;
      xred = -999.;
      yred = -999.;
      return;
   }
   xred = matcor_X[tel - 1][indix][indiy];
   yred = matcor_Y[tel - 1][indix][indiy];
   return;
}

////////////////////////////////////////////////////////////////////////////////

// tel =1, 2, ou 3 en entrée
void posxy(int tel, Double_t E, Double_t p1, Double_t p2,
           Double_t p3, Double_t p4, Double_t& xred, Double_t& yred)
{


// coefficients de calibration des 4 parametres de position
   static Double_t ap1_sib[3] , bp1_sib[3] , ap2_sib[3] , bp2_sib[3] , ap3_sib[3]
   , bp3_sib[3] , ap4_sib[3] , bp4_sib[3];

   static int initial;

// limites des correlations sommepositions-vs-energiemesureeencanaux
// utilisées pour nettoyer les images
   static Double_t alim_max[3] = {999999. , 4831.52 , 4623.40};
   static Double_t blim_max[3] = {0. , 458.51 , 271.5};
   static Double_t alim_min[3] = {0. , 4696.84 , 4549.74};
   static Double_t blim_min[3] = {0. , -306.09 , -233.42};

// 1 seuil commun pour les 4 voies position et 1 seuil pour l'énergie (en canaux sur 16384)
   static const double seuil_pos = 100;
   static const double seuil_E = 1000;

   if (initial != 999) {
      cout << " INITIALISATION POUR CALIBRATION EN LOCALISATION" << endl;


      ifstream my_file;
      for (int ii = 1; ii < 4; ii++) {

         my_file.open(Form("/home/morjean/E416a/calib/calib_SIB%d.dat", ii), ios::in);
         if (!my_file.is_open())
            cout << "Erreur ouverture calibration localisation" << endl;

         int jj = ii - 1;
         char buf[1024];
         double chi2;
         for (int i = 1; i < 5; i++) {
            my_file.getline(buf, 1024);
            cout << buf << endl;
         }
         my_file >> ap1_sib[jj] >> bp1_sib[jj] >> chi2;
         ap1_sib[jj] = 1. / ap1_sib[jj]; // passage de K=aG+b à G=aK+b
         bp1_sib[jj] = -(bp1_sib[jj] * ap1_sib[jj]);

         for (int i = 1; i < 7; i++) {
            my_file.getline(buf, 1024);
            cout << buf << endl;
         }
         my_file >> ap2_sib[jj] >> bp2_sib[jj] >> chi2;
         ap2_sib[jj] = 1. / ap2_sib[jj]; // passage de K=aG+b à G=aK+b
         bp2_sib[jj] = -(bp2_sib[jj] * ap2_sib[jj]);

         for (int i = 1; i < 7; i++) {
            my_file.getline(buf, 1024);
            cout << buf << endl;
         }
         my_file >> ap3_sib[jj] >> bp3_sib[jj] >> chi2;
         ap3_sib[jj] = 1. / ap3_sib[jj]; // passage de K=aG+b à G=aK+b
         bp3_sib[jj] = -(bp3_sib[jj] * ap3_sib[jj]);

         for (int i = 1; i < 7; i++) {
            my_file.getline(buf, 1024);
            cout << buf << endl;
         }
         my_file >> ap4_sib[jj] >> bp4_sib[jj] >> chi2;
         ap4_sib[jj] = 1. / ap4_sib[jj]; // passage de K=aG+b à G=aK+b
         bp4_sib[jj] = -(bp4_sib[jj] * ap4_sib[jj]);

         my_file.close();

      }
      initial = 999;
   }




   Double_t som_pos = p1 + p2 + p3 + p4;

   if (som_pos) {
      Double_t xpos = (p3 + p4 - p1 - p2) / som_pos;
      Double_t ypos = (p3 + p2 - p1 - p4) / som_pos;

      if ((E > seuil_E) && (p1 > seuil_pos) && (p2 > seuil_pos) &&
            (p3 > seuil_pos) && (p4 > seuil_pos)) {
         int jdet = tel - 1;
         Double_t pcal1 = p1 * ap1_sib[jdet] + bp1_sib[jdet];
         Double_t pcal2 = p2 * ap2_sib[jdet] + bp2_sib[jdet];
         Double_t pcal3 = p3 * ap3_sib[jdet] + bp3_sib[jdet];
         Double_t pcal4 = p4 * ap4_sib[jdet] + bp4_sib[jdet];
// Test positions supérieures seuils
         som_pos = pcal1 + pcal2 + pcal3 + pcal4;
         Double_t xpos = (pcal3 + pcal4 - pcal1 - pcal2) / som_pos;
         Double_t ypos = (pcal3 + pcal2 - pcal1 - pcal4) / som_pos;

         Double_t emin = som_pos * alim_min[jdet] + blim_min[jdet];
         Double_t emax = som_pos * alim_max[jdet] + blim_max[jdet];

         if ((E > emin) && (E < emax)) {
            xred, yred;
            red(tel, xpos, ypos, xred, yred);
         } else {
            xred = -9999. ;
            yred = -9999.;
            return;
         }
      } else {
         xred = -9999. ;
         yred = -9999.;
         return;
      }
   } else {
      xred = -9999. ;
      yred = -9999.;
      return;
   }
   return;
}

