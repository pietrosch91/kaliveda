/***************************************************************************
$Id: KVIDLine.cpp,v 1.18 2007/09/04 14:36:47 ebonnet Exp $
                          KVIDLine.cpp  -  description
                             -------------------
    begin                : Nov 10 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "KVIDLine.h"
#include "Riostream.h"
#include "TF1.h"
#include "TProfile.h"
#include "TH2.h"
#include "TCutG.h"

ClassImp(KVIDLine)
/////////////////////////////////////////////////////////////////////////////////////////
//KVIDLine
//
//Base class for lines/cuts used for particle identification in 2D data maps.
//This provides all methods for calculating the shortest distance between a point (x,y)
//in the map and the line, or for finding the relative orientation of the point and
//the line (above/below, left/right). These methods are modified versions of methods
//existing in the TGraph base class.
//
//Note: by default, a KVIDLine cannot be graphically modified using the mouse.
//This is to avoid accidents! This can of course be changed either with the context menu
//of the line, or when using the KVIDGridManagerGUI in Edit mode. 
//
//Double_t KVIDLine::DistanceToLine(Double_t px, Double_t py, Int_t &i_near)
//
    //Compute the closest distance of approach from point px,py to this line.
    //The units of px, py are the same as the coordinates of the graph
    //WARNING: can be NEGATIVE - see below.
    //
    //The distance from the point to each segment is calculated in turn.
    //
    //If the point lies between the endpoints of at least one segment, the returned
    //distance will be the closest of these segments. In this case i_near gives the
    //index of the first point of the closest segment.
    //
    //If not then the distance is the smallest distance between the endpoint
    //of a segment and the point. In this case i_near gives the index of the
    //closest endpoint.
    //If the closest segment-endpoint is not one of the endpoints of the whole line,
    //the returned distance is positive.
    //On the other hand, if the closest part of the line to the point is one of the
    //two endpoints of the line, the returned distance is NEGATIVE.
//
//Double_t KVIDLine::DistanceToLine(Double_t px, Double_t py, Double_t xp1, Double_t yp1, Double_t xp2, Double_t yp2, Int_t &i_nearest_point)
//
    //Given a line segment defined by endpoints (xp1,yp1) and (xp2,yp2) find the
    //  shortest distance between point (px,py) and the line.
    //
    //                                                   M
    //      P1 (xp1,yp1) x---------------------------------------x P2 (xp2,yp2)
    //                                                      |
    //                                                      |
    //                                                      |
    //                                                      |
    //                                                      |
    //                                                      x
    //                                                                P (px,py)
    //This is simply the magnitude of the component of vector P1->P  (or P2->P)
    //perpendicular to P1->P2.
    //
    //If the point is indeed between the two endpoints as shown, then
    //P1M + P2M = P1P2
    //If not, P1M + P2M > P1P2. In this case the closest distance is that to the nearer
    //of the two endpoints, but the value returned is negative; i_nearest_point gives the index (0 or 1)
    //of the nearer endpoint
//
//Bool_t KVIDLine::PosRelToLine(Option_t* opt, Double_t px, Double_t py, Double_t xp1, Double_t yp1, Double_t xp2, Double_t yp2 )
//
    //Given a line segment defined by endpoints (xp1,yp1) and (xp2,yp2) test the
    //  direction of (px,py) with respect to the line in the (x,y) plane.
    //
    //                                                        M
    //      P1 (xp1,yp1) x---------------------------------------x P2 (xp2,yp2)
    //                                                         |
    //                                                         |
    //                                                         |
    //                                                         |
    //                                                         |
    //                                                         x
    //                                                                P (px,py)
    //This is given by the angle phi between vector MP (component of vector P1->P  (or P2->P)
    //perpendicular to P1->P2) and the +ve x-axis.
    //Point is left of line if 90<phi<270
    //Point is right of line if 270<phi<90
    //Point is above line if 0<phi<180
    //Point is below line if 180<phi<360
    //The option string is "left", "right", "above" or "below", and the result is either kTRUE or kFALSE
    //depending on the relative position of the point and the line. 
    //E.g. in the diagram shown above, PosRelToLine("below", ...) would give kTRUE - the point is
    //below the line.
//
//Bool_t KVIDLine::WhereAmI(Double_t px, Double_t py, Option_t *opt)
//
    //The relative position of point (px,py) with respect to the line is tested.
    //The option string can be "left", "right", "above" or "below".
    //WhereAmI( px, py, "above") returns kTRUE if the point is above the line etc. etc.
    //
    //First of all, the closest segment/point of the line to the point is found.
    //Then the relative position of the point and this segment/point is tested.
//
//void KVIDLine::GetStartPoint(Double_t& x, Double_t& y) const
//
//Coordinates of first point in line
//
//void KVIDLine::GetEndPoint(Double_t& x, Double_t& y) const
//
//Coordinates of last point in line
//
//Bool_t KVIDLine::IsBetweenEndPoints(Double_t x, Double_t y, const Char_t* axis) const
//
    //Returns kTRUE for point (x,y) if :
    // axis = "" (default) and both x and y lie inside the endpoints of the line x1 < x < x2, y1 < y < y2
    // axis = "x" and x lies inside the endpoints x1 < x < x2
    // axis = "y" and y lies inside the endpoints y1 < y < y2
    KVIDLine::KVIDLine()
{
   //Default ctor
   //We set marker size and style for the underlying TGraph, this way a small circle is drawn at
   //each point in the line
   //Also SetEditable(kFALSE) is used to stop the line being modified using the mouse.
   SetMarkerStyle(kCircle);
   SetMarkerSize(0.8);
   SetLineWidth(2);
   SetEditable(kFALSE);
}

KVIDLine::~KVIDLine()
{
   //Default dtor
}

KVIDLine::KVIDLine(const TGraph & gr):TGraph(gr)
{
   //initialize KVIDLine using TGraph copy ctor
}

//_____________________________________________________________________________________________

Int_t KVIDLine::Compare(const TObject * obj) const
{
   //Dummy method used for sorting, can be redefined in more specific implementations.
   //A priori we don't know how to "order" the identification cuts.

   return 0;
}

//_____________________________________________________________________________________________

void KVIDLine::WriteAsciiFile(ofstream & file, const Char_t * name_prefix)
{
   //Write name and coordinates of line in file buffer stream
   //Optional string name_prefix will be written just in front of the name of the line.
   //Format is :
   //
   //+classname_of_line
   //name_prefix:name_of_line
   //number_of_points
   //x1    y1
   //x2    y2
   //...
   //etc. etc.
   file << '+' << ClassName() << endl;
   if (name_prefix)
      file << name_prefix << ":";
   file << GetName() << endl;
   file << GetN() << endl;
   for (Int_t i = 0; i < GetN(); i++) {
      Double_t x, y;
      GetPoint(i, x, y);
      file << x << "   " << y << endl;
   }
}

//_____________________________________________________________________________________________

void KVIDLine::ReadAsciiFile(ifstream & file)
{
   //Read coordinates of line in file buffer stream
   //Format is :
   //
   //number_of_points
   //x1    y1
   //x2    y2
   //...
   //etc. etc.
   Int_t N;
   file >> N;
   for (Int_t i = 0; i < N; i++) {
      Double_t x, y;
      file >> x >> y;
      SetPoint(i, x, y);
   }
}

//_____________________________________________________________________________________________

void KVIDLine::Print(Option_t * opt) const
{
   //Print out for line
   //The optional "opt" string, if given, is printed in parentheses after the line's name
   //This is used by KVIDGrid in order to show which lines are "ID" lines and which are
   //"OK" lines (i.e. used to define an identifiable area in a data map).
   cout << ClassName() << " : " << GetName() << "(" << opt << ")" << endl;
   TGraph::Print();
}

//_____________________________________________________________________________________________

void KVIDLine::Scale(Double_t sx, Double_t sy)
{
   //Scale the coordinates of every point of the line according to
   //      X_0 ---> sx*X_0
   //      Y_0 ---> sy*Y_0
   //Default argument value '-1' means leave coordinate unchanged
   if (TMath::Abs(sx) == 1 && TMath::Abs(sy) == 1)
      return;
   for (int i = 0; i < fNpoints; i++) {
      if (sx > 0.)
         fX[i] *= sx;
      if (sy > 0.)
         fY[i] *= sy;
   }
}

//_____________________________________________________________________________________________

void KVIDLine::Scale(TF1 *sx, TF1 *sy)
{
   //Apply to the coordinates of every point the transformation
   //      X_0 ---> sx(X_0)
   //      Y_0 ---> sy(Y_0)
   if (!sx && !sy) return;	// if the two functions are NULL we leave coordinate unchanged
   for (Int_t  ii = 0; ii < fNpoints; ii++) {
      if (sx) fX[ii] = sx->Eval(fX[ii]);	
      if (sy) fY[ii] = sy->Eval(fY[ii]);
   }
}

//_____________________________________________________________________________________________

void KVIDLine::CopyGraph(TGraph * graph)
{
   //Copy coordinates of points from the TGraph
   Double_t x, y;
   //change number of points
   Set(graph->GetN());
   for (int i = 0; i < GetN(); i++) {
      graph->GetPoint(i, x, y);
      SetPoint(i, x, y);
   }
}

//_____________________________________________________________________________________________

KVIDLine *KVIDLine::MakeIDLine(TObject *obj,Double_t xdeb,Double_t xfin,Double_t np,Bool_t save)
{
   // create an IDLine from different objects (Inherited from TProfile, TGraph, TF1)
	// - "xdeb" and "xfin" stand for the delimitation of the line - dafault values are -1
	// in this case range is the one of the considered object
	// if the choosen range is greater than the object one, one point for each limit is added with the value 
	// corresponding to the first and/or last of the object
	// - "np" is in the TF1 case the number point (default value 20), in the TProfile case is the minimum threshold
	// for the bin entries (default value is one); for the TGraph case it has no effect; 
	// In the TProfile and TGraph cases the number of points are determined by the object considering the interval
	// - "save" allow to save the TF1 or TProfile object	which can be recall with the GetListofFunction() and GetHistogram() TGraph methods
	Double_t xdeb_bis=xdeb,xfin_bis=xfin,np_bis=np;
	if (obj){
		KVIDLine *line = new KVIDLine();
		if (obj->InheritsFrom("TF1")) {
			Double_t xmin,xmax; dynamic_cast<TF1*>(obj)->GetRange(xmin,xmax);
			if (xdeb_bis==-1) xdeb_bis=xmin; if (xfin_bis==-1) xfin_bis=xmax;
			if (np_bis==1) np_bis = Double_t(dynamic_cast<TF1*>(obj)->GetNpx());
			Double_t inter = (xfin_bis-xdeb_bis)/(np_bis);
			Int_t pp=0;
			for (Double_t xx=xdeb_bis;xx<=xfin_bis;xx+=inter){
				line->SetPoint(pp++,xx,dynamic_cast<TF1*>(obj)->Eval(xx));
			}
			if (save) line->Fit(dynamic_cast<TF1*>(obj),"0+","",xdeb_bis,xfin_bis);
		}
		else if (obj->InheritsFrom("TGraph")) {	// np has no effect in this case
			Int_t nx=dynamic_cast<TGraph*>(obj)->GetN(),np2=0;
			Double_t *xtab = dynamic_cast<TGraph*>(obj)->GetX();
			Double_t *ytab = dynamic_cast<TGraph*>(obj)->GetY();
			if (xdeb_bis==-1) xdeb_bis = xtab[0]; if (xfin_bis==-1) xfin_bis = xtab[nx-1];
			if (xdeb_bis < xtab[0]) line->SetPoint(np2++,xdeb_bis,ytab[0]);
			for (Int_t pp=0; pp<nx; pp+=1) if ( xdeb_bis<=xtab[pp] && xtab[pp]<=xfin_bis ) line->SetPoint(np2++,xtab[pp],ytab[pp]);
			if (xfin_bis > xtab[nx-1]) line->SetPoint(np2,xfin_bis,ytab[nx-1]);
		}
		else if (obj->InheritsFrom("TProfile")) {
			TProfile *pf = dynamic_cast<TProfile*>(obj);
			Int_t nx=pf->GetNbinsX(),np2=0;
			Int_t pp=1;	 while ( pf->GetBinEntries(pp)<np && pp<nx ) pp+=1; Int_t xmin=pp;
			pp=nx; while ( pf->GetBinEntries(pp)<np && pp>xmin ) pp-=1; Int_t xmax=pp;
			if (xdeb_bis==-1) xdeb_bis = pf->GetBinCenter(xmin); if (xfin_bis==-1) xfin_bis = pf->GetBinCenter(xmax);
			if (xdeb_bis < pf->GetBinCenter(xmin)) line->SetPoint(np2++,xdeb_bis,pf->GetBinContent(xmin));
			for (pp=xmin; pp<=xmax; pp+=1){
				Double_t xx = pf->GetBinCenter(pp);
   			if ( xdeb_bis<=xx && xx<=xfin_bis && pf->GetBinEntries(pp)>=np ) line->SetPoint(np2++,xx,pf->GetBinContent(pp));
			}
			if (xfin_bis > pf->GetBinCenter(xmax)) line->SetPoint(np2,xfin_bis,pf->GetBinContent(xmax));
			if (save) line->SetHistogram(pf);
		}
		else cout << "le type ne correspond pas " << endl;
		line->SetName(Form("from_%s",obj->GetName()));
		return line;
	}
	else return NULL;
}

//_____________________________________________________________________________________________

/*
KVIDLine *KVIDLine::MakeIDLine(TH2 *hh,TCutG *cut,Double_t xdeb,Double_t xfin,Double_t np,Bool_t save)
{
   if (hh){
		TProfile *gg = NULL;
		if (cut){
			if (cut->InheritsFrom("TCutG")){
				Double_t xmin=1e6,xmax=-1e6;
				Double_t ymin=1e6,ymax=-1e6;
				for (Int_t pp=0;pp<cut->GetN();pp+=1){
					Double_t xx,yy; cut->GetPoint(pp,xx,yy);
					if (xx<xmin) xmin=xx; if (xx>xmax) xmax=xx;
					if (yy<ymin) ymin=yy; if (yy>ymax) ymax=yy;
				}

				Int_t bxmin = hh->GetXaxis()->FindBin(xmin); xmin = hh->GetXaxis()->GetBinLowEdge(bxmin);
				Int_t bxmax = hh->GetXaxis()->FindBin(xmax); xmax = hh->GetXaxis()->GetBinUpEdge(bxmax);
				Int_t bymin = hh->GetYaxis()->FindBin(ymin); ymin = hh->GetYaxis()->GetBinLowEdge(bymin);
				Int_t bymax = hh->GetYaxis()->FindBin(ymax); ymax = hh->GetYaxis()->GetBinUpEdge(bymax);
			
				Int_t dx = bxmax - bxmin +1;
				gg = new TProfile(Form("%s_%s",hh->GetName(),cut->GetName()),"prof",dx,xmin,xmax,ymin,ymax);
				for (Int_t xx=bxmin;xx<=bxmax;xx+=1)
					for (Int_t yy=bymin;yy<=bymax;yy+=1)
						if ( cut->IsInside(hh->GetXaxis()->GetBinCenter(xx),hh->GetYaxis()->GetBinCenter(yy)) ) 
							gg->Fill(hh->GetXaxis()->GetBinCenter(xx),hh->GetYaxis()->GetBinCenter(yy),hh->GetBinContent(xx,yy));
			}
		}	
		else { gg = hh->ProfileX(); }
		return KVIDLine::MakeIDLine(gg,xdeb,xfin,np,save);
	}
	else return NULL;
}
*/
