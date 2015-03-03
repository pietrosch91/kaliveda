/*
 * GenericModel.h
 *
 *  Created on: Mar 3, 2015
 *      Author: john
 */

#ifndef GENERICMODEL_H_
#define GENERICMODEL_H_

#include "RooStats/ModelConfig.h"

using namespace RooStats;

namespace BackTrack {

class GenericModel : public ModelConfig {
public:
	GenericModel();
	virtual ~GenericModel();
   
   ClassDef(GenericModel,1)//Generic model for backtracing studies
};

} /* namespace BackTrack */

#endif /* GENERICMODEL_H_ */
