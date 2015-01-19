Int_t SETUP()
{
   if (gSystem->Load("libKVMultiDetbase") == -1)
      return -1;
   KVBase::InitEnvironment();
   return 0;
}
