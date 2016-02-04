#include "ThreadedMinimiserData.h"

ClassImp(ThreadedMinimiserData)

ThreadedMinimiserData::ThreadedMinimiserData() :
   forward_counter_(0),
   backward_counter_(0)
{

}

ThreadedMinimiserData::~ThreadedMinimiserData()
{

}

Int_t ThreadedMinimiserData::GetForwardCounter() const
{
   return forward_counter_;
}

Int_t ThreadedMinimiserData::GetBackwardCounter() const
{
   return backward_counter_;
}

void ThreadedMinimiserData::SetForwardCounter(Int_t value)
{
   forward_counter_ = value;
}

void ThreadedMinimiserData::SetBackwardCounter(Int_t value)
{
   backward_counter_ = value;
}



